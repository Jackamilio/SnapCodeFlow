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

#include "Utils.h"
#include "bindings.h"
#include <lua.hpp>

std::map<const SnapBlock*, unsigned int> blockIDs;

struct InstructionBlockDebug : InstructionBlock {

    InstructionBlockDebug(Vector2 startpos = Vector2{0.0f, 0.0f})
        : InstructionBlock(startpos)
    {
        static unsigned int idcounter = 0;
        blockIDs[this] = ++idcounter;
    }

    void Draw(ImDrawList *drawList, const Vector2& pos) override {
        SnapBlock::Draw(drawList,pos);

        char buf[64];
        sprintf(buf,"%d",blockIDs[this]);

        drawList->AddText(pos,toImGuiCol(GREEN), buf);
    }
};

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
        //SetWindowState(FLAG_WINDOW_RESIZABLE);
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
        InitWindow(width, height, "Test");
        rlImGuiSetup(true);
    }

    void MainLoop() {
        while (!WindowShouldClose())
        {
            rlImGuiBegin();
            SnapBlock::Prepare();

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

struct TestInstructions : Main {
    SnapBlock::Container c1;
    SnapBlock::Container c2;

    void NewRandomBlock() {
        c1.Add(new InstructionBlockDebug({(float)GetRandomValue(10,210),(float)GetRandomValue(10,210)}));
    }

    TestInstructions() {
        NewRandomBlock();
    }

    static void TrackInstructionBlockClusters(const SnapBlock::Container& container) {
        std::unordered_set<SnapBlock::Set*> clusters;

        for (auto& b : container.collec) {
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
                    sprintf(buf, "Cluster #%p of size %lld", b->cluster, b->cluster->size());
                    ImGui::LabelText(buf, ss.str().c_str());
                }
            }
        }
    }

    void Loop() override {

        TrackInstructionBlockClusters(c1);
        c1.Update();

        ImGui::Begin("Fenetre");

        c2.Update();
        ImGui::Text("Bonjour");
        ImGui::LabelText("C1 size", "%lld", c1.collec.size());
        ImGui::LabelText("C2 size", "%lld", c2.collec.size());
        if (ImGui::Button("Spawn new block")) {
            NewRandomBlock();
        }

        ImGui::End();
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
        for(auto& pp: board.collec) {
            delete pp;
        }
        board.collec.clear();

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
        ImGui::InputText("Lua command", commandtest, 512);
        if (ImGui::Button("Try command")) {
            ExecCommand();
        }
        ImGui::Checkbox("Loop command", &loopcommand);
        if (loopcommand) {
            ExecCommand();
        }
        ImGui::End();
    }

    ~TestLuaBindings() {
        lua_close(lua);
    }
};

int main()
{
    //TestSimpleBlocks().MainLoop();
    //TestPuzzle().MainLoop();
    //TestInstructions().MainLoop();
    TestLuaBindings().MainLoop();
    return 0;
}
