#include "SnapBlock.h"
#include "utils.h"

#include "raymath.h"

#include <unordered_map>
#include <iostream>

std::set<SnapBlock*> deleteRequests;

std::unordered_map<SnapBlock*,Vector2> clicpos;
SnapBlock* setDraggedBlock = nullptr;
SnapBlock* draggedBlock = nullptr;
SnapBlock::Container* draggedBlockPreviousOwner = nullptr;
SnapBlock* droppedBlock = nullptr;

Vector2 dragWindowOffset;
Vector2 dragWindowSize;

void SnapBlock::Prepare() {
    // dropped block at the begining of the frame means nothing accepted it, it's a failed drop
    if (droppedBlock) {
        // restore state when clicked
        if (draggedBlockPreviousOwner) draggedBlockPreviousOwner->Add(droppedBlock->cluster);
        for(auto& sb : SnapBlock::Cluster(*droppedBlock->cluster)) {
            sb->WhenDropFailed();
            if (deleteRequests.find(sb) == deleteRequests.end()) {
                sb->pos = clicpos[sb] - sb->GetOrigin();
                //restore previous connexion
                if (sb == droppedBlock) {
                    Vector2 droploc;
                    SnapBlock* snapto = sb->GetSnapDrop(droploc,*sb->cluster->owner);
                    if (snapto) {
                        sb->SnapWith(*snapto,droploc);
                    }
                }            
            }
        }
        droppedBlock = nullptr;
        draggedBlockPreviousOwner = nullptr;
        clicpos.clear();
    }

    Clean();

    if (setDraggedBlock && !draggedBlock) {
        clicpos.clear();
        draggedBlock = setDraggedBlock;
        draggedBlockPreviousOwner = draggedBlock->cluster->owner;
        draggedBlock->WhenDragStarts();
        Vector2 min{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
        Vector2 max{std::numeric_limits<float>::min(), std::numeric_limits<float>::min()};
        Vector2 offset = draggedBlock->GetOrigin();
        SnapBlock::Container* dbowner = draggedBlock->cluster->owner;
        if (dbowner) dbowner->Erase(draggedBlock->cluster);
        for (auto& sb : *draggedBlock->cluster) {
            sb->pos += offset;
            clicpos[sb] = sb->pos;
            const Vector2 tl = sb->pos;
            const Vector2 br = sb->pos + sb->size;
            if (tl.x < min.x) min.x = tl.x;
            if (tl.y < min.y) min.y = tl.y;
            if (br.x > max.x) max.x = br.x;
            if (br.y > max.y) max.y = br.y;
        }
        dragWindowOffset = min - draggedBlock->pos;
        dragWindowSize = max - min;
        setDraggedBlock = nullptr;
    }

    if (draggedBlock) {
        ImGui::SetNextWindowPos(draggedBlock->pos + dragWindowOffset);
        ImGui::SetNextWindowSize(dragWindowSize);
        ImGui::SetNextWindowFocus();
        if (ImGui::Begin("##DraggedSnapBlock",nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoSavedSettings)) {

            for (auto& sb : *draggedBlock->cluster) {
                sb->pos = clicpos[sb] + ImGui::GetMouseDragDelta();
                sb->Update();
            }

            // Vector2 min = draggedBlock->pos + dragWindowOffset + Vector2{1,1};
            // ImGui::GetWindowDrawList()->AddRect(min, min+ dragWindowSize - Vector2{2,2}, toImGuiCol(YELLOW));

            if(ImGui::IsMouseReleased(0)) {
                droppedBlock = draggedBlock;
                draggedBlock = nullptr;
            }

            ImGui::End();
        }
    }

}

void SnapBlock::Clean() {
    for(auto& dr : deleteRequests) {
        delete dr;
    }
    deleteRequests.clear();
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

    for(auto& sb : *this) {
            sb->Update();
    }

    ImGui::PushID(this);
    ImGui::Dummy(ImGui::GetWindowSize());
    ImGui::SetCursorScreenPos(savecursorpos);

    if (type && (draggedBlock || droppedBlock) && ImGui::IsWindowHovered()) {
        Vector2 droploc;
        SnapBlock* snapto = nullptr;
        SnapBlock* snaptryer = draggedBlock ? draggedBlock : droppedBlock;

        if (snaptryer) {
            snapto = snaptryer->GetSnapDrop(droploc,*this);
            if (snapto) {
                snaptryer->DrawShadow(ImGui::GetWindowDrawList(), windowpos + droploc);
            }
        }
        if (droppedBlock) {
            if (clusters.find(droppedBlock->cluster) == clusters.end()) {
                Vector2 tr = droppedBlock->GetOrigin() - windowpos;
                Add(droppedBlock->cluster);
                for(auto& r : *droppedBlock->cluster) {
                    r->pos += tr;
                }
            }
            if (snapto) {
                droppedBlock->SnapWith(*snapto,droploc);
            }
            droppedBlock = nullptr;
        }
    }

    ImGui::PopID();
}

void SnapBlock::Container::Add(SnapBlock* sb) {
    if (sb->cluster->owner != this && clusters.find(sb->cluster) == clusters.end()) {
        if (sb->cluster->owner) {
            sb->cluster->owner->Erase(sb);
        }
        sb->cluster->owner = this;
        clusters.emplace(sb->cluster);
    }
}

void SnapBlock::Container::Add(Cluster *cl) {
    if (cl->owner != this) {
        if (cl->owner) cl->owner->clusters.erase(cl);
        cl->owner = this;
        clusters.emplace(cl);
    }
}

void SnapBlock::Container::Erase(SnapBlock *sb) {
    if (sb->cluster->size() > 1) {
        sb->cluster->erase(sb);
        sb->cluster = new Cluster;
        sb->cluster->emplace(sb);
    } else {
        clusters.erase(sb->cluster);
    }
}

void SnapBlock::Container::Erase(Cluster *cl) {
    clusters.erase(cl);
    cl->owner = nullptr;
}

SnapBlock::OrderedSet SnapBlock::Container::GetAllOrdered() const
{
    OrderedSet ret;
    for(auto& cl : clusters) {
        for(auto& sb : *cl) {
            ret.emplace(sb);
        }
    }
    return ret;
}

SnapBlock::SnapBlock(Vector2 startpos, const char* containerType)
    : containerType(containerType)
    , cluster(new Cluster)
    , pos(startpos)
    , size(60.0f, 60.0f)
{
    cluster->emplace(this);
}

SnapBlock::~SnapBlock() {
    cluster->erase(this);
    if (cluster->empty()) {
        if (cluster->owner) {
            cluster->owner->Erase(cluster);
        }
        delete cluster;
    }
}

void SnapBlock::Update() {
    const Vector2 orig = GetOrigin();
    
    Vector2 savescreenpos = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(orig + pos);

    ImGui::PushID(this);

    ImGui::InvisibleButton("##snapblock", size);

    DrawLining(ImGui::GetWindowDrawList(), orig + pos);

    if (ImGui::IsItemClicked()) {
        setDraggedBlock = this;
    }

    Widget();

    ImGui::PopID();
    ImGui::SetCursorScreenPos(savescreenpos);
}

Vector2 SnapBlock::GetOrigin() const {
    return cluster->owner ? cluster->owner->windowpos : Vector2{0.f,0.f};
}

SnapBlock* SnapBlock::GetSnapDrop(Vector2& o_droploc, Container& cont) {
    for(auto& otherblock : cont.GetAllOrdered()) {
        if (otherblock != this && cluster != otherblock->cluster) {
            o_droploc = pos + GetOrigin() - cont.windowpos;
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
    Cluster* old = cluster;
    for (auto& sb : *old) {
        sb->cluster = other.cluster;
    }
    other.cluster->merge(*old);
    if (old->owner) old->owner->Erase(old);
    delete old;
}

void SnapBlock::Widget() {
}

void SnapBlock::DrawLining(ImDrawList *drawList, const Vector2 &pos)
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
        Container* owner = cluster->owner;
        cluster->erase(this);
        cluster = new Cluster();
        cluster->emplace(this);
        if (owner) {
            owner->Add(cluster);
        }
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

SnapBlock::Container::Iterator::Iterator(std::set<Cluster *>::iterator it, std::set<Cluster *>::iterator it_end)
    : cit(it)
    , cit_end(it_end)
{
    if (it != it_end)
        sbit = (*cit)->begin();

    SkipEmptyClusters();
}

SnapBlock::Container::Iterator &SnapBlock::Container::Iterator::operator++()
{
    ++sbit;
    SkipEmptyClusters();
    return *this;
}

void SnapBlock::Container::Iterator::SkipEmptyClusters() {
    while (cit != cit_end && sbit == (*cit)->end()) {
        ++cit;
        if (cit != cit_end)
            sbit = (*cit)->begin();
    }
}
