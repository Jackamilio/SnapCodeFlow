#include "InstructionBlock.h"
#include <vector>
#include <stdio.h>
#include "utils.h"
#include <misc/cpp/imgui_stdlib.h>
#include <sstream>

InstructionBlock::InstructionBlock(const SnapBlocDesc& description, Vector2 startpos)
    : SnapBlock(startpos)
    , desc(description)
    , anchor(nullptr)
    , topsibling(nullptr)
    , bottomsibling(nullptr)
{
    size.x = (float)GetRandomValue(80,250);
    size.y = 30.0f;

    for(const auto& p : desc.params) {
        const TypeDesc::Memory& mem = p.type.memory;
        void* val = nullptr;
        if (mem.size > 0) {
            if (p.type.isString()) {
                val = new std::string;
            }
            else if (p.type.isPointer()) {
                // todo pointers
            }
            else {
                val = ::operator new(mem.size, std::align_val_t(mem.alignment));
                memset(val,0,mem.size);
            }
        }
        values.push_back(val);
    }
}

InstructionBlock::~InstructionBlock() {
    unsigned int i = 0;
    for(const auto& p : desc.params) {
        if (values[i]) {
            if (p.type.isString()) {
                delete static_cast<std::string*>(values[i]);
            }
            else if (p.type.isPointer()) {
                // todo pointers
            }
            else {
                ::operator delete(values[i], p.type.memory.size, std::align_val_t(p.type.memory.alignment));
            }
        }
        ++i;
    }
}

void ToolTip(const char* desc)
{
    if (ImGui::BeginItemTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

inline void Vec2xyMax(const Vector2& compare, Vector2& inout) {
    if (compare.x > inout.x) inout.x = compare.x;
    if (compare.y > inout.y) inout.y = compare.y;
}

void SetNextItemWidthFromText(const char* text, float min = 0.0f) {
    float width = ImGui::CalcTextSize(text).x + ImGui::GetStyle().FramePadding.x * 2.0f;
    ImGui::SetNextItemWidth(min > width ? min : width);
}

template<typename T>
void SetNextItemWidthFromValue(T value, const char* format) {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), format, value);
    SetNextItemWidthFromText(buffer);
}

const std::map<ImGuiDataType, const char*> formats = {
    {ImGuiDataType_S8,"%d"},
    {ImGuiDataType_U8,"%u"},
    {ImGuiDataType_S16,"%d"},
    {ImGuiDataType_U16,"%u"},
    {ImGuiDataType_S32,"%d"},
    {ImGuiDataType_U32,"%u"},
    {ImGuiDataType_S64,"%lld"},
    {ImGuiDataType_U64,"%llu"},
    {ImGuiDataType_Float,"%.2f"},
    {ImGuiDataType_Double,"%f"}
};

const std::map<std::pair<std::string,bool>,ImGuiDataType> toImGuiType = {
    {{"char",false}, ImGuiDataType_S8},
    {{"char",true}, ImGuiDataType_U8},
    {{"short",false}, ImGuiDataType_S16},
    {{"short",true}, ImGuiDataType_U16},
    {{"int",false}, ImGuiDataType_S32},
    {{"int",true}, ImGuiDataType_U32},
    {{"long",false}, ImGuiDataType_S64}, // not sure if it should be "long long"
    {{"long",true}, ImGuiDataType_U64}, // idem
    {{"float",false}, ImGuiDataType_Float},
    {{"double",false}, ImGuiDataType_Double},
};

const float defaultWidgetWidth = 60.0f;

bool DragScalar(const char* id, ImGuiDataType type, void* val) {
    auto it = formats.find(type);
    const char* format = it == formats.end() ? nullptr : it->second;
    switch(type) {
        case ImGuiDataType_S8: SetNextItemWidthFromValue(*static_cast<char*>(val), format); break;
        case ImGuiDataType_U8: SetNextItemWidthFromValue(*static_cast<unsigned char*>(val), format); break;
        case ImGuiDataType_S16: SetNextItemWidthFromValue(*static_cast<short*>(val), format); break;
        case ImGuiDataType_U16: SetNextItemWidthFromValue(*static_cast<unsigned short*>(val), format); break;
        case ImGuiDataType_S32: SetNextItemWidthFromValue(*static_cast<int*>(val), format); break;
        case ImGuiDataType_U32: SetNextItemWidthFromValue(*static_cast<unsigned int*>(val), format); break;
        case ImGuiDataType_S64: SetNextItemWidthFromValue(*static_cast<long long*>(val), format); break;
        case ImGuiDataType_U64: SetNextItemWidthFromValue(*static_cast<unsigned long long*>(val), format); break;
        case ImGuiDataType_Float: SetNextItemWidthFromValue(*static_cast<float*>(val), format); break;
        case ImGuiDataType_Double: SetNextItemWidthFromValue(*static_cast<double*>(val), format); break;
        default: ImGui::SetNextItemWidth(defaultWidgetWidth); break;
    }
    return ImGui::DragScalar(id,type,val,1.0f,nullptr,nullptr,format);
}

bool ImGuiInputParam(const TypeDesc& td, const char* id, void* data) {
    if (!data) {
        ImGui::Text("No data");
        return false;
    }

    if (td.isString()) {
        std::string* str = static_cast<std::string*>(data);
        SetNextItemWidthFromText(str->c_str(), defaultWidgetWidth);
        return ImGui::InputText(id, str);
    }

    if (td.isPointer()) {
        ImGui::Text("Pointer");
        return false;
    }

    const std::string& tn = td.GetTypeName();

    if (tn == "bool")
        return ImGui::Checkbox(id, static_cast<bool*>(data));

    // basic types
    auto it = toImGuiType.find({tn, td.isUnsigned});
    if (it != toImGuiType.end()) {
        return DragScalar(id, it->second, data);
    }

    if (tn == "Color") {
        Color& col = *static_cast<Color*>(data);
        float fcol[4] = {col.r/255.0f,col.g/255.0f,col.b/255.0f,col.a/255.0f};
        bool ret = ImGui::ColorEdit4(id, fcol, ImGuiColorEditFlags_NoInputs);
        col = {(unsigned char)(fcol[0]*255.0f),(unsigned char)(fcol[1]*255.0f),(unsigned char)(fcol[2]*255.0f),(unsigned char)(fcol[3]*255.0f)};
        return ret;
    }

    if (tn == "Vector2") {
        Vector2& vec = *static_cast<Vector2*>(data);
        ImGui::PushID(id);
        ImGui::BeginGroup();
        bool ret1 = DragScalar("##x", ImGuiDataType_Float, &vec.x);
        ImGui::SameLine(0.0f,ImGui::GetStyle().ItemInnerSpacing.x);
        bool ret2 = DragScalar("##y", ImGuiDataType_Float, &vec.y);
        ImGui::EndGroup();
        ImGui::PopID();
        return ret1 || ret2;
    }

    ImGui::Text("%s", tn.c_str());
    return false;
}

bool InstructionBlock::Widget() {
    ToolTip(desc.desc.c_str()); // should apply to the invisible button made by SnapBlock
    bool ret = true;
    Vector2 start = GetOrigin() + pos;
    ImGui::SetCursorScreenPos(start + Vector2{ImGui::GetStyle().ItemInnerSpacing.x,0.0f});
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", desc.name.c_str());
    Vector2 rectmax = ImGui::GetItemRectMax();
    unsigned int i=0;
    for(const auto& p : desc.params) {
        std::string label = std::string("##") + p.name;
        //static std::array<std::byte, 16> test;
        ImGui::SameLine();
        ImGuiInputParam(p.type, label.c_str(), values[i++]);
        //ImGui::InputFloat(label.c_str(),&test);
        Vec2xyMax(ImGui::GetItemRectMax(), rectmax);
        if (ImGui::IsItemActive()) ret = false;
        ToolTip(p.name.c_str());
    }
    size = rectmax - start;
    return ret;
}

float InstructionBlock::GetClusterHeight() const {
    float clusterheight = 0.0f;
    for (auto& sb : *cluster) {
        clusterheight += sb->size.y;
    }
    return clusterheight;
}

bool InstructionBlock::CanSnap(Vector2& io_at, const SnapBlock* from) const {
    const InstructionBlock* ib = dynamic_cast<const InstructionBlock*>(from);
    if (anchor || (ib && ib->anchor)) return false;

    std::vector<Vector2> validpoints;
    if (!topsibling) {
        validpoints.push_back({0,-size.y});
    }
    validpoints.push_back({0,size.y});

    if (CheckSnapLocationsToSelf(validpoints, io_at, size.y)) {
        return true;
    }
    else if (ib && from->cluster->size() > 1) {
        io_at.y += ib->GetClusterHeight();
        if (CheckSnapLocationsToSelf(validpoints, io_at, size.y)) {
            return true;
        }
    }
    return false;
}

InstructionBlock* InstructionBlock::GetFirstSibling() {
    InstructionBlock* current = this;
    while(current->topsibling) {
        current = current->topsibling;
    }
    return current;
}

InstructionBlock* InstructionBlock::GetLastSibling() {
    InstructionBlock* current = this;
    while(current->bottomsibling) {
        current = current->bottomsibling;
    }
    return current;
}

void InstructionBlock::WhenDragStarts() {
    if (anchor && cluster->owner) {
        InstructionBlock* duplicate = new InstructionBlock(desc);
        duplicate->pos = *anchor;
        duplicate->anchor = anchor;
        duplicate->size = size;
        anchor = nullptr;
        cluster->owner->Add(duplicate);
    }
    Unsnap();
}

void InstructionBlock::WhenSnapped(SnapBlock& other) {
    InstructionBlock* otheri = dynamic_cast<InstructionBlock*>(&other);
    if (!otheri) return;

    InstructionBlock* ls = GetLastSibling();
    if (pos.y <= other.pos.y) {
        ls->bottomsibling = otheri;
        otheri->topsibling = ls;
    } else {
        topsibling = otheri;
        if (otheri->bottomsibling) {
            ls->bottomsibling = otheri->bottomsibling;
            otheri->bottomsibling->topsibling = ls;
        }
        otheri->bottomsibling = this;
    }

    if (!topsibling) {
        InstructionBlock* current = otheri->topsibling;
        Vector2 p = otheri->pos;
        while(current) {
            p.y -= current->size.y;
            current->pos = p;
            current = current->topsibling;
        }
    }
    else {
        InstructionBlock* current = bottomsibling;
        Vector2 p = pos;
        p.y += size.y;
        while(current) {
            current->pos = p;
            p.y += current->size.y;
            current = current->bottomsibling;
        }
    }
}

void InstructionBlock::WhenDropFailed()
{
    RequestDeletion();
}

void InstructionBlock::Unsnap() {
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
        InstructionBlock* current = bottomsibling;
        while (current) {
            current->pos.y -= size.y;
            current = current->bottomsibling;
        }

        if (topsibling) topsibling->bottomsibling = bottomsibling;
        if (bottomsibling) bottomsibling->topsibling = topsibling;
        topsibling = nullptr;
        bottomsibling = nullptr;

        SnapBlock::Unsnap();
    }
    else if (cluster->size() > 1) {
        if (topsibling) {
            topsibling->bottomsibling = nullptr;
            topsibling = nullptr;
        }

        Cluster* nc = new Cluster;
        nc->owner = cluster->owner;
        InstructionBlock* current = this;
        while (current) {
            current->cluster->erase(current);
            current->cluster = nc;
            current->cluster->emplace(current);
            current = current->bottomsibling;
        }
    }
}

std::string InstructionBlock::ToLuaString() const {
    std::stringstream ss;

    ss << desc.prefix << desc.name << '(';
    bool first = true;
    for (const auto& p : desc.params) {
        if (first) {
            first = false;
        } else {
            ss << ", ";
        }
        if (p.type.baseType) {
            // TODO
        }
    }
    ss << ")\n";

    return ss.str();
}
