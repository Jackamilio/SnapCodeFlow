#pragma once
#include "imgui_ext.h"
#include <raymath.h>
#include<unordered_set>
#include <set>
#include <span>
#include <map>

struct SnapBlock {
    struct CompareY {
        bool operator()(const SnapBlock* a, const SnapBlock*b) const {
            return a->pos.y == b->pos.y ? a < b : a->pos.y < b->pos.y;
        }
    };
    typedef std::set<SnapBlock*> Set;
    typedef std::set<SnapBlock*,CompareY> OrderedSet;

    struct Container {
        const char* type;
        Set collec;
        Vector2 windowpos;
        Vector2 windowsize;

        Container(const char* type = "default");
        ~Container();

        void Update();
        void Add(SnapBlock* r);
    };

    const char* containerType;
    Container* owner;
    Set* cluster;

    Vector2 pos;
    Vector2 size;

    static void Prepare();

    SnapBlock(Vector2 startpos = Vector2{0.0f,0.0f}, const char* containerType = "default");
    virtual ~SnapBlock();
    void Update();

    Vector2 GetOrigin() const;

    SnapBlock* GetSnapDrop(Vector2& o_droploc, Container& cont);
    void SnapWith(SnapBlock& other, const Vector2& at);

    virtual void Widget();
    virtual void DrawLining(ImDrawList *drawList, const Vector2& pos);
    virtual void DrawShadow(ImDrawList *drawList, const Vector2& pos);
    virtual bool CanSnap(Vector2& io_at, const SnapBlock* from) const;

    virtual void WhenDragStarts();
    virtual void WhenDropFailed();
    virtual void WhenSnapped(SnapBlock& other);
    virtual void Unsnap();

    void RequestDeletion();

    void MergeClusters(SnapBlock& other);
    bool CheckSnapLocationsToSelf(const std::span<Vector2>& locations, Vector2& io_at, float distance = 20.0f) const;
};