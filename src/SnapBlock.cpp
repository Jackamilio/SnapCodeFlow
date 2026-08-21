#include "SnapBlock.h"
#include "utils.h"

#include "raymath.h"

#include <iostream>

std::map<SnapBlock*,bool> dropFail;
std::set<SnapBlock*> deleteRequests;

SnapBlock* setDraggedBlock = nullptr;
SnapBlock* draggedBlock = nullptr;

void SnapBlock::Prepare() {
    for(auto& sb : dropFail) {
        sb.first->WhenDropFailed();
        if (deleteRequests.find(sb.first) == deleteRequests.end()) {
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
    }
    dropFail.clear();
    for(auto& dr : deleteRequests) {
        if (dr->owner) {
            dr->owner->collec.erase(dr);
        }
        delete dr;
    }
    deleteRequests.clear();

    if (setDraggedBlock) {
        draggedBlock = setDraggedBlock;
        // if (draggedBlock->owner) {
        //     draggedBlock->pos += draggedBlock->owner->windowpos;
        //     draggedBlock->owner->collec.erase(draggedBlock);
        //     draggedBlock->owner = nullptr;
        // }
        draggedBlock->WhenDragStarts();
        for (auto& sb : *draggedBlock->cluster) {
            sb->clicpos = sb->pos;
        }
        setDraggedBlock = nullptr;
    }

    // if (draggedBlock) {
    //     ImGui::SetNextWindowPos(draggedBlock->pos);
    //     ImGui::SetNextWindowSize(draggedBlock->size);
    //     ImGui::SetNextWindowFocus();
    //     ImGui::Begin("DraggedSnapBlock",nullptr,ImGuiWindowFlags_NoDecoration);
    //     draggedBlock->Update();
    //     ImGui::End();
    // }

}

std::set<SnapBlock::Container*> allcontainers;

SnapBlock::Container::Container(const char* type) : type(type) {
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

    for(SnapBlock* sb : collec) {
        //if (sb != draggedBlock)
            sb->Update();
    }

    // for(auto it = collec.rbegin(); it != collec.rend(); ++it) {
    //     auto& sb = *it;
    //     ImDrawList *drawList = (sb->cluster->find(draggedBlock) != sb->cluster->end()) ? ImGui::GetForegroundDrawList() : ImGui::GetWindowDrawList();
    //     sb->Draw(drawList, windowpos + sb->pos);
    // }

    ImGui::PushID(this);
    ImGui::Dummy(ImGui::GetWindowSize());
    ImGui::SetCursorScreenPos(savecursorpos);

    if (type && ImGui::BeginDragDropTarget()) {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type);
        SnapBlock* dropped = payload ? *(SnapBlock**)payload->Data : nullptr;

        Vector2 droploc;
        SnapBlock* snapto = nullptr;
        SnapBlock* snaptryer = draggedBlock ? draggedBlock : dropped;

        if (snaptryer) {
            snapto = snaptryer->GetSnapDrop(droploc,*this);
            if (snapto) {
                snaptryer->DrawShadow(ImGui::GetWindowDrawList(), windowpos + droploc);
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

SnapBlock::SnapBlock(Vector2 startpos, const char* containerType)
    : containerType(containerType)
    , owner(nullptr)
    , cluster(new Set)
    , pos(startpos)
    , clicpos(0.0f, 0.0f)
    , size(60.0f, 60.0f)
{
    cluster->emplace(this);
}

SnapBlock::~SnapBlock() {}

void SnapBlock::Update() {
    // if (lastloopid == loopcounter) return;
    // lastloopid = loopcounter;

    //if (!owner) return;
    Vector2 orig = GetOrigin();

    Draw(ImGui::GetWindowDrawList(), orig + pos);
    
    Vector2 savescreenpos = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(orig + pos);

    ImGui::PushID(this);

    ImGui::InvisibleButton("Effing Rect", size);

    if (ImGui::IsItemClicked()) {
        setDraggedBlock = this;
    }

    bool dragging = (draggedBlock == this) && ImGui::IsItemActive();

    if (dragging && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip)) {
        SnapBlock* ptrvalue = this;
        ImGui::SetDragDropPayload(containerType, &ptrvalue, sizeof(SnapBlock*));
        ImGui::EndDragDropSource();
    }

    if (dragging) {
        for (auto& sb : *cluster) {
            sb->pos = sb->clicpos + ImGui::GetMouseDragDelta();
        }
    }
    else if (draggedBlock == this) {
        draggedBlock = nullptr;
        if (!ImGui::IsWindowHovered() || !owner->type) {
            for (auto& sb: *cluster) {
                dropFail[sb] = (sb == this);
            }
        }
    }

    Widget();

    ImGui::PopID();
    ImGui::SetCursorScreenPos(savescreenpos);
}

Vector2 SnapBlock::GetOrigin() const {
    return owner ? owner->windowpos : Vector2{0.f,0.f};
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

void SnapBlock::Widget() {
}

void SnapBlock::Draw(ImDrawList *drawList, const Vector2 &pos)
{
    drawList->AddRectFilled(pos, pos+size, toImGuiCol(ImGui::IsItemHovered() ? LIGHTGRAY : GRAY));
}

void SnapBlock::DrawShadow(ImDrawList *drawList, const Vector2 &pos)
{
    //drawList->AddRect(pos, pos + size, toImGuiCol(YELLOW));

    Vector2 shift = pos - this->pos;

    const float dt = 8.0f;
    for(auto& sb : *cluster) {
        Vector2 diag{sb->size.y,sb->size.y};
        Vector2 cursor = sb->pos + shift;
        cursor.x = floorf(cursor.x / dt) * dt - sb->size.y;
        float targety = cursor.y;
        cursor.y = floorf(cursor.y / dt) * dt;
        cursor.x += targety - cursor.y;
        cursor.y = targety;
        Vector2 sbpos = sb->pos + shift;
        if (cursor.x < sb->pos.x + shift.x - sb->size.y) cursor.x += dt;
        for(;cursor.x < sbpos.x + sb->size.x; cursor.x += dt) {
            Vector2 a = cursor;
            Vector2 b = cursor + diag;
            if (a.x < sbpos.x) {
                a.y += sbpos.x - a.x;
                a.x = sbpos.x;
            }
            if (b.x > sbpos.x + sb->size.x) {
                b.y -= b.x - (sbpos.x + sb->size.x);
                b.x = sbpos.x + sb->size.x;
            }
            drawList->AddLine(a, b, toImGuiCol(YELLOW));
        }
    }
}

bool SnapBlock::CanSnap(Vector2 &io_at, const SnapBlock* from) const
{
    Vector2 validpoints[] = {{0,-size.y},{size.x,0},{0,size.y},{-size.x,0}};

    // return CheckSnapLocationsToSelf(validpoints, io_at);
    for(auto& sb : *from->cluster) {
        Vector2 shift = sb->pos - from->pos;
        Vector2 io_atbis = io_at + shift;
        if (CheckSnapLocationsToSelf(validpoints, io_atbis)) {
            io_at = io_atbis - shift;
            return true;
        }
    }
    return false;
}

void SnapBlock::WhenDragStarts()
{
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
        Unsnap();
    }
}

void SnapBlock::WhenDropFailed()
{
    // nothing
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

void SnapBlock::RequestDeletion()
{
    deleteRequests.emplace(this);
}

bool SnapBlock::CheckSnapLocationsToSelf(const std::span<Vector2>& locations, Vector2 &io_at, float distance) const
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
