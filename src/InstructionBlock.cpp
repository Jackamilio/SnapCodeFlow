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
}

void InstructionBlock::Draw(ImDrawList *drawList, const Vector2& pos) {
    SnapBlock::Draw(drawList,pos);
    drawList->AddText(pos, toImGuiCol(BLACK), desc.name.c_str());
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
    if (anchor && owner) {
        InstructionBlock* duplicate = new InstructionBlock(desc);
        duplicate->pos = *anchor;
        duplicate->anchor = anchor;
        duplicate->size = size;
        anchor = nullptr;
        owner->Add(duplicate);
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

        Set* nc = new Set;
        InstructionBlock* current = this;
        while (current) {
            current->cluster->erase(current);
            current->cluster = nc;
            current->cluster->emplace(current);
            current = current->bottomsibling;
        }
    }
}
