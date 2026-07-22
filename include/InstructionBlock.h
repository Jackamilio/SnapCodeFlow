#pragma once

#include <map>

#include "SnapBlock.h"

struct InstructionBlock : SnapBlock
{
    InstructionBlock* topsibling;
    InstructionBlock* bottomsibling;

    InstructionBlock(Vector2 startpos = Vector2{0.0f, 0.0f});

    //void Draw(ImDrawList* drawList, const Vector2& pos) override;

    float GetClusterHeight() const;

    bool CanSnap(Vector2& io_at, const SnapBlock* from) const override;

    void WhenDragStarts() override;

    InstructionBlock* GetFirstSibling();

    InstructionBlock* GetLastSibling();

    void WhenSnapped(SnapBlock& other) override;

    void Unsnap() override;
};