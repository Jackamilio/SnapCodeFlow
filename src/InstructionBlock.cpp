#include "InstructionBlock.h"
#include <vector>
#include <stdio.h>
#include "utils.h"

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
            val = ::operator new(mem.size, std::align_val_t(mem.alignment));
            memset(val,0,mem.size);
        }
        values.push_back(val);
    }
}

InstructionBlock::~InstructionBlock() {
    unsigned int i = 0;
    for(const auto& p : desc.params) {
        if (values[i]) {
            ::operator delete(values[i], std::align_val_t(p.type.memory.alignment));
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

template<typename T>
void SetNexItemWidthFromValue(T value, const char* format) {
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), format, value);

    float width = ImGui::CalcTextSize(buffer).x + ImGui::GetStyle().FramePadding.x * 2.0f;

    ImGui::SetNextItemWidth(width);
}

bool ImGuiInputParam(const TypeDesc& td, const char* id, void* data) {
    if (!data) {
        ImGui::Text("No data");
        return false;
    }

    if (td.baseType == "bool")
        return ImGui::Checkbox(id, static_cast<bool*>(data));

    if (td.baseType == "int") {
        SetNexItemWidthFromValue(*static_cast<int*>(data), "%d");
        if (td.isUnsigned)
            return ImGui::InputScalar(id, ImGuiDataType_U32, static_cast<int*>(data));
        else
            return ImGui::InputInt(id, static_cast<int*>(data), 0, 0);
    }

    if (td.baseType == "float") {
        const char* format = "%.2f";
        SetNexItemWidthFromValue(*static_cast<float*>(data), format);
        return ImGui::InputFloat(id, static_cast<float*>(data), 0.0f, 0.0f, format);
    }

    if (td.baseType == "Color") {
        return ImGui::ColorEdit4(id, static_cast<float*>(data), ImGuiColorEditFlags_NoInputs);
    }

    if (td.baseType == "Vector2") {
        ImGui::SetNextItemWidth(80.0f);
        return ImGui::InputFloat2(id, static_cast<float*>(data), "%.2f");
    }

    ImGui::Text("%s", td.baseType.c_str());
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
