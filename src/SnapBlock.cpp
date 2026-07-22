#include "SnapBlock.h"
#include "utils.h"

#include "raymath.h"

#include <iostream>

//unsigned int loopcounter = 0;
std::map<SnapBlock*,bool> dropFail;

SnapBlock* setDraggedBlock = nullptr;
SnapBlock* draggedBlock = nullptr;

void SnapBlock::Prepare() {
    //++loopcounter;

    for(auto& sb : dropFail) {
        sb.first->pos = sb.first->clicpos;
        //restore previous connexion
        if (sb.second) {
            Vector2 droploc;
            SnapBlock* snapto = sb.first->GetSnapDrop(droploc,*sb.first->owner);
            if (snapto) {
                sb.first->SnapWith(*snapto,droploc);
            }
        }
    }
    dropFail.clear();

    if (setDraggedBlock) {
        draggedBlock = setDraggedBlock;
        setDraggedBlock = nullptr;
    }
}

std::set<SnapBlock::Container*> allcontainers;

SnapBlock::Container::Container() {
    allcontainers.emplace(this);
}

SnapBlock::Container::~Container() {
    allcontainers.erase(this);
}

void SnapBlock::Container::Update()
{
    Vector2 savecursorpos = ImGui::GetCursorScreenPos();
    ImGui::SetCursorPos(ImVec2());
    windowpos = ImGui::GetCursorScreenPos();
    windowsize = ImGui::GetWindowSize();

    for(SnapBlock* r : collec) {
        r->Update();
    }

    ImGui::PushID(this);
    ImGui::Dummy(ImGui::GetWindowSize());
    ImGui::SetCursorScreenPos(savecursorpos);

    if (ImGui::BeginDragDropTarget()) {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Efrect");
        SnapBlock* dropped = payload ? *(SnapBlock**)payload->Data : nullptr;

        Vector2 droploc;
        SnapBlock* snapto = nullptr;
        SnapBlock* snaptryer = draggedBlock ? draggedBlock : dropped;

        if (snaptryer) {
            snapto = snaptryer->GetSnapDrop(droploc,*this);
            if (snapto) {
                ImGui::GetWindowDrawList()->AddRect(windowpos + droploc, windowpos + droploc + snaptryer->size, toImGuiCol(YELLOW));
            }
        }
        if (dropped) {
            if (collec.find(dropped) == collec.end()) {
                draggedBlock = nullptr;
                Vector2 tr = (dropped->owner ? dropped->owner->windowpos : Vector2{0.0f,0.0f}) - windowpos;
                for(auto& r : *dropped->cluster) {
                    dropFail.erase(r);
                    r->pos += tr;
                    Add(r);
                }
            }
            if (snapto) {
                dropped->SnapWith(*snapto,droploc);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::PopID();
}

void SnapBlock::Container::Add(SnapBlock* r) {
    if (r->owner != this && collec.find(r) == collec.end()) {
        if (r->owner) {
            r->owner->collec.erase(r);
        }
        r->owner = this;
        collec.emplace(r);
    }
}

SnapBlock::SnapBlock(Vector2 startpos)
    : owner(nullptr), cluster(new Set), pos(startpos), clicpos({0.0f, 0.0f}), size(60.0f, 60.0f)//, wasdragging(false)//, lastloopid(0)
{
    cluster->emplace(this);
}

SnapBlock::~SnapBlock() {}

void SnapBlock::Update() {
    // if (lastloopid == loopcounter) return;
    // lastloopid = loopcounter;

    if (!owner) return;
    
    Vector2 savescreenpos = ImGui::GetCursorScreenPos();

    ImGui::PushID(this);

    ImGui::SetCursorScreenPos(owner->windowpos + pos);
    ImGui::InvisibleButton("Effing Rect", size);

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip)) {
        SnapBlock* ptrvalue = this;
        ImGui::SetDragDropPayload("Efrect", &ptrvalue, sizeof(SnapBlock*));
        ImGui::EndDragDropSource();
    }

    if (ImGui::IsItemClicked()) {
        setDraggedBlock = this;
        WhenDragStarts();
        for (auto& sb : *cluster) {
            sb->clicpos = sb->pos;
        }
    }
    bool dragging = (draggedBlock == this) && ImGui::IsItemActive();

    if (dragging) {
        for (auto& sb : *cluster) {
            sb->pos = sb->clicpos + ImGui::GetMouseDragDelta();
        }
    }
    else if (draggedBlock == this) {
        draggedBlock = nullptr;
        if (!ImGui::IsWindowHovered()) {
            for (auto& sb: *cluster) {
                dropFail[sb] = (sb == this);
            }
        }
    }

    ImDrawList *drawList = (cluster->find(draggedBlock) != cluster->end()) ? ImGui::GetForegroundDrawList() : ImGui::GetWindowDrawList();

    Draw(drawList, owner->windowpos + pos);

    ImGui::SetCursorScreenPos(savescreenpos);
    ImGui::PopID();
}

SnapBlock* SnapBlock::GetSnapDrop(Vector2& o_droploc, Container& cont) {
    OrderedSet orderedset(cont.collec.begin(), cont.collec.end());
    for(auto& otherblock : orderedset) {
        if (otherblock != this && cluster != otherblock->cluster) {
            o_droploc = pos + owner->windowpos - cont.windowpos;
            if (otherblock->CanSnap(o_droploc,this)) {
                return otherblock;
            }
        }
    }
    return nullptr;
}

void SnapBlock::SnapWith(SnapBlock &other, const Vector2 &at)
{
    Vector2 tosnap = at - pos;
    for (auto &sb : *cluster) {
        sb->pos += tosnap;
    }
    WhenSnapped(other);
    MergeClusters(other);
}

void SnapBlock::MergeClusters(SnapBlock &other)
{
    Set* old = cluster;
    for (auto& sb : *old) {
        sb->cluster = other.cluster;
    }
    //other.cluster->insert(old->begin(), old->end());
    other.cluster->merge(*old);
    delete old;
}

void SnapBlock::Draw(ImDrawList *drawList, const Vector2& pos)
{
    drawList->AddRectFilled(pos, pos+size, toImGuiCol(ImGui::IsItemHovered() ? LIGHTGRAY : GRAY));
}

bool SnapBlock::CanSnap(Vector2 &io_at, const SnapBlock* from) const
{
    (void)from;
    Vector2 validpoints[] = {{0,-size.y},{size.x,0},{0,size.y},{-size.x,0}};

    return CheckSnapLocations(validpoints, io_at);
}

void SnapBlock::WhenDragStarts()
{
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
        Unsnap();
    }
}

void SnapBlock::WhenSnapped(SnapBlock& other)
{
    (void)other;
}

void SnapBlock::Unsnap()
{
    if (cluster->size() > 1) {
        cluster->erase(this);
        cluster = new Set;
        cluster->emplace(this);
    }
}

bool SnapBlock::CheckSnapLocations(const std::span<Vector2>& locations, Vector2 &io_at, float distance) const
{
    for (Vector2& vp : locations) {
        Vector2 l = pos + vp;
        //if (Vector2DistanceSqr(l,io_at) < distance*distance) {
        if (abs(io_at.x - l.x) < distance && abs(io_at.y - l.y) < distance) {
            io_at = l;
            return true;
        }
    }

    return false;
}
