#pragma once
#include "imgui_ext.h"
#include <raymath.h>
#include <unordered_set>
#include <set>
#include <span>
#include <map>

// IMPORTANT:
// Derived SnapBlocks must keep their destructor protected.
// SnapBlocks are owned/destroyed by its static managing functions.
struct SnapBlock {
    struct CompareY {
        bool operator()(const SnapBlock* a, const SnapBlock*b) const {
            return a->pos.y == b->pos.y ? a < b : a->pos.y < b->pos.y;
        }
    };
    typedef std::set<SnapBlock*> Set;
    typedef std::set<SnapBlock*,CompareY> OrderedSet;

    struct Container;
    struct Cluster : public Set {
        Container* owner = nullptr;
    };

    struct Container {
        const char* type;

        Vector2 windowpos;
        Vector2 windowsize;

        Container(const char* type = "default");
        ~Container();

        void Update();
        void Add(SnapBlock* sb);
        void Add(Cluster* cl);
        void Erase(SnapBlock* sb);
        void Erase(Cluster* cl);

        OrderedSet GetAllOrdered() const;

        struct Iterator {
            Iterator(std::set<Cluster*>::iterator it, std::set<Cluster*>::iterator it_end);
            Iterator& operator++();

            inline bool operator==(const Iterator& other) {return cit == other.cit && (cit == cit_end || sbit == other.sbit);}
            inline bool operator!=(const Iterator& other) { return !(*this == other); }
            inline SnapBlock* const& operator*() {return *sbit;}

            private:
            void SkipEmptyClusters();

            std::set<Cluster*>::iterator cit;
            std::set<Cluster*>::iterator cit_end;
            Set::iterator sbit;
        };

        inline Iterator begin() const {return Iterator(clusters.begin(), clusters.end());}
        inline Iterator end() const {return Iterator(clusters.end(), clusters.end());}

        private:
        std::set<Cluster*> clusters;
    };

    const char* containerType;

    Cluster* cluster;

    Vector2 pos;
    Vector2 size;

    static void Prepare(); // call once at the beginning of every frame
    static void Clean(); // deletes every block that requested it. Call it outside any iterating snapblock loop. Called by Prepare(). Useful for exiting the program cleanly.

    SnapBlock(Vector2 startpos = Vector2{0.0f,0.0f}, const char* containerType = "default");
protected:
    virtual ~SnapBlock();
public:
    void Update();

    Vector2 GetOrigin() const;

    SnapBlock* GetSnapDrop(Vector2& o_droploc, Container& cont);
    void SnapWith(SnapBlock& other, const Vector2& at);

    virtual bool Widget(); // Child must return true if it allows the block to be dragged, false otherwise. Useful when input widgets are edited.
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