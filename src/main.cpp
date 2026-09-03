#include <raylib.h>
#include "rlImGui.h"
#include <vector>
#include <unordered_set>
#include "imgui_ext.h"
#include <map>
#include <stdio.h>
#include <string>
#include <sstream>
#include <span>
#include <iostream>
#include <fstream>

#include "SnapBlock.h"
#include "PuzzlePiece.h"
#include "InstructionBlock.h"
#include "DataDesc.h"

#include "utils.h"
#include "bindings.h"
#include <lua.hpp>

std::map<const SnapBlock*, unsigned int> blockIDs;

// struct InstructionBlockDebug : InstructionBlock {

//     InstructionBlockDebug(Vector2 startpos = Vector2{0.0f, 0.0f})
//         : InstructionBlock(startpos)
//     {
//         static unsigned int idcounter = 0;
//         blockIDs[this] = ++idcounter;
//     }

//     void Draw(ImDrawList *drawList, const Vector2& pos) override {
//         SnapBlock::Draw(drawList,pos);

//         char buf[64];
//         sprintf(buf,"%d",blockIDs[this]);

//         drawList->AddText(pos,toImGuiCol(GREEN), buf);
//     }
// };

struct Main {
    Main() {
        int width = 1280;
        int height = 720;
        char* settings = LoadFileText("settings.txt");
        if (settings) {
            std::stringstream ss(settings);
            ss >> width >> height;
            UnloadFileText(settings);
        }
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
        InitWindow(width, height, "Test");

        std::cout << "dpi x is " << GetWindowScaleDPI().x << std::endl;

        rlImGuiBeginInitImGui();
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();
        rlImGuiEndInitImGui();
    }

    void MainLoop() {
        while (!WindowShouldClose())
        {
            rlImGuiBegin();
            SnapBlock::Prepare();

            ImGui::ShowDemoWindow();
            Loop();

            BeginDrawing();
            ClearBackground(DARKGRAY);
            rlImGuiEnd();
            EndDrawing();
        }
    }

    virtual void Loop() = 0;

    virtual ~Main() {
        std::stringstream ss;
        ss << GetScreenWidth() << ' ' << GetScreenHeight();
        SaveFileText("settings.txt",ss.str().c_str());
        rlImGuiShutdown();
        CloseWindow();
    }
};

struct GameContext {
    unsigned int width;
    unsigned int height;
    RenderTexture2D render;
    ImTextureRef imguiTexture;
    Color clearColor;
    lua_State* lua;
    std::string code;
    std::string error;

    GameContext();
    ~GameContext();

    void Loop();
    void ImGuiDebug();

    void SubmitCode(const std::string& codestr);
};

GameContext::GameContext()
    : width(640)
    , height(480)
    , render(LoadRenderTexture(width,height))
    , imguiTexture((ImTextureID)render.texture.id)
    , clearColor({225,255,255,255})
    , lua(luaL_newstate()) {
    luaL_openlibs(lua);
    if (luaL_dostring(lua, "rl = require(\"raylib\")") != LUA_OK) {
        error = lua_tostring(lua, -1);
        lua_pop(lua, 1);
    }
}

GameContext::~GameContext() {
    lua_close(lua);
    UnloadRenderTexture(render);
}

void GameContext::Loop() {
    BeginTextureMode(render);

    if (!error.empty()) {
        ClearBackground(BLACK);
        DrawText(error.c_str(), 10, 10, 20, WHITE);
    }
    else {
        ClearBackground(clearColor);

        if (!code.empty()) {
            if (luaL_dostring(lua, code.c_str()) != LUA_OK) {
                error = lua_tostring(lua, -1);
                code.clear();
                lua_pop(lua, 1);
            }
        }
    }

    EndTextureMode();
}

void GameContext::ImGuiDebug() {
    if (ImGui::Begin("Game Preview")) {
        ImGui::Image(imguiTexture, Vector2{(float)width,(float)height}, Vector2(0,1), Vector2(1,0));
        ImGui::End();
    }
}

void GameContext::SubmitCode(const std::string& codestr) {
    error.clear();
    code = codestr;
}

struct TestInstructions : Main {
    SnapBlock::Container coding;

    SnapBlock::Container models;

    GameContext game;

    char commandtest[512];

    // void NewRandomBlock() {
    //     c1.Add(new InstructionBlockDebug({(float)GetRandomValue(10,210),(float)GetRandomValue(10,210)}));
    // }

    TestInstructions() : models(nullptr), commandtest("rl.DrawFPS(10,10)") {
        DataDesc::Load();

        const FunctionDesc::List* tm = DataDesc::GetFunctionsListFromTag("test");

        if (!tm) return;
        Vector2 mpos{10,50};
        for(const auto& sbd : *tm) {
            InstructionBlock* ib = new InstructionBlock(*sbd, mpos);
            ib->anchor = new Vector2(mpos);
            models.Add(ib);
            mpos.y += 35.0f;
        }
    }

    ~TestInstructions() {
        for(auto& sb : models) {
            InstructionBlock* ib = dynamic_cast<InstructionBlock*>(sb);
            if (ib) {
                delete ib->anchor;
            }
            sb->RequestDeletion();
        }
        
        SnapBlock::Clean();
        DataDesc::Unload();
    }

    static void TrackInstructionBlockClusters(const SnapBlock::Container& container) {
        std::unordered_set<SnapBlock::Cluster*> clusters;

        for (auto& b : container) {
            if (clusters.find(b->cluster)==clusters.end()) {
                clusters.emplace(b->cluster);

                InstructionBlock* ib = dynamic_cast<InstructionBlock*>(*b->cluster->begin());
                if (ib) {
                    ib = ib->GetFirstSibling();
                    std::stringstream ss;
                    while (ib) {
                        ss << blockIDs[ib] << "; ";
                        ib = ib->bottomsibling;
                    }

                    char buf[64];
                    sprintf(buf, "Cluster #%p of size %zu", b->cluster, b->cluster->size());
                    ImGui::LabelText(buf, "%s", ss.str().c_str());
                }
            }
        }
    }

    void Loop() override {

        // TrackInstructionBlockClusters(c1);
        // c1.Update();

        ImGui::Begin("Coding area");
        ImGui::InputText("Lua command", commandtest, 512);
        if (ImGui::Button("Try command")) {
            game.SubmitCode(commandtest);
        }
        coding.Update();
        ImGui::End();

        ImGui::Begin("Models");
        models.Update();
        ImGui::End();

        game.Loop();
        game.ImGuiDebug();
    }
};

struct TestPuzzle : Main {
    SnapBlock::Container board;
    SnapBlock::Container side;
    PuzzlePiece::WholePuzzle puzzle;
    double loadfailtime;

    TestPuzzle(const char* file="great-job-meme.png", int columns=4, int lines=4) {
        TryNewPuzzle(file, columns, lines);
    }

    bool TryNewPuzzle(const char* file, int columns, int lines) {
        Texture t = LoadTexture(file);
        if (t.id == 0) return false;

        if (puzzle.image.id != 0) UnloadTexture(puzzle.image);
        for(auto& pp: board) {
            pp->RequestDeletion();
        }
        SnapBlock::Clean();

        puzzle.image = t;
        puzzle.columns = columns;
        puzzle.lines = lines;

        for(int i=0; i<columns*lines;++i) {
            board.Add(new PuzzlePiece(Vector2{(float)GetRandomValue(50,550),(float)GetRandomValue(50,550)},puzzle,i));
        }
        return true;
    }

    void Loop() override {
        ImGui::Begin("Puzzle pieces!!!");
        if (puzzle.image.id) {
            board.Update();
        } else {
            ImGui::Text("Couldn't load the image I guess...");
        }
        ImGui::End();

        ImGui::Begin("New Puzzle");
        ImGui::DragInt("Columns", &puzzle.columns, 1.0f, 2, 32);
        ImGui::DragInt("Lines", &puzzle.lines, 1.0f, 2, 32);
        if (ImGui::Button("Choose image")) {
            std::string filepath = OpenFileDialog();
            if (!(!filepath.empty() && TryNewPuzzle(filepath.c_str(), puzzle.columns, puzzle.lines))) {
                loadfailtime = GetTime() + 5.0;
            }
        }
        if (GetTime() < loadfailtime) {
            ImGui::TextColored({1,0,0,1}, "Failed to load the file.");
        }
        side.Update();
        ImGui::End();
    }
};

struct TestSimpleBlocks : Main {
    SnapBlock::Container c;

    TestSimpleBlocks() {
        SpawnBlock();
    }

    void SpawnBlock() {
        c.Add(new SnapBlock(Vector2{(float)GetRandomValue(50,550),(float)GetRandomValue(50,550)}));
    }

    void Loop() override {
        ImGui::Begin("Plain blocks");

        c.Update();

        if (ImGui::Button("Spawn")) {
            SpawnBlock();
        }

        ImGui::End();
    }
};

struct TestLuaBindings : Main {

    lua_State* lua;
    char commandtest[512];
    bool loopcommand;

    TestLuaBindings() : lua(luaL_newstate()), commandtest("print('Hello from Lua!')"), loopcommand(false) {
        luaL_openlibs(lua);
        DataDesc::Load();
    }

    void ExecCommand() {
        if (luaL_dostring(lua, commandtest) != LUA_OK) {
            const char* error = lua_tostring(lua, -1);
            std::cout << "Erreur lua : " << error << std::endl;
            lua_pop(lua, 1);
        }
    }

    void Loop() override {
        ImGui::Begin("Lua workspace");
        if (ImGui::Button("Generate bindings")) {
            GenerateLuaRaylibBindings("raylib.lua");
        }
        if (ImGui::Button("Generate simple function list file")) {
            GenerateSimpleFunctionList("raylib_tags_gen.csv");
        }
        ImGui::InputText("Lua command", commandtest, 512);
        if (ImGui::Button("Try command")) {
            ExecCommand();
        }
        ImGui::Checkbox("Loop command", &loopcommand);
        if (loopcommand) {
            ExecCommand();
        }

        if (ImGui::TreeNode("Functions per tags")) {
            const ImGuiTreeNodeFlags leafflags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            for(const auto& tag : DataDesc::GetFunctionTags()) {
                ImGui::PushID(1);
                const FunctionDesc::List* lt = DataDesc::GetFunctionsListFromTag(tag);
                if (!lt) continue;
                if (ImGui::TreeNode(tag.c_str(), "%s (%zu)", tag.c_str(), lt->size())) {
                    for(const auto& sbd : *lt) {
                        if (ImGui::TreeNode(sbd->name.c_str())) {
                            ImGui::TreeNodeEx(sbd->name.c_str(), leafflags, "ID %i", sbd->order);
                            ImGui::TreeNodeEx(sbd->desc.c_str(), leafflags, "Description : \"%s\"", sbd->desc.c_str());
                            if (ImGui::TreeNode("Tags")) {
                                for(const auto& t : sbd->tags) {
                                    ImGui::TreeNodeEx(t.c_str(), leafflags);
                                }
                                ImGui::TreePop();
                            }
                            ImGui::TreePop();                        
                        }
                    }
                    ImGui::TreePop();                    
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }

        ImGui::End();
    }

    ~TestLuaBindings() {
        DataDesc::Unload();
        lua_close(lua);
    }
};

int main()
{
    //TestSimpleBlocks().MainLoop();
    //TestPuzzle().MainLoop();
    TestInstructions().MainLoop();
    //TestLuaBindings().MainLoop();
    return 0;
}
