#pragma once

#include <map>

#include "SnapBlock.h"
#include "DataDesc.h"

struct InstructionBlock : SnapBlock
{
    typedef std::vector<void*> DataArray;

    const FunctionDesc& desc;
    DataArray values;

    Vector2* anchor;

    InstructionBlock* topsibling;
    InstructionBlock* bottomsibling;

    InstructionBlock(const FunctionDesc& description, Vector2 startpos = Vector2{0.0f, 0.0f});

    bool Widget() override;

    float GetClusterHeight() const;

    bool CanSnap(Vector2& io_at, const SnapBlock* from) const override;


    InstructionBlock* GetFirstSibling();
    InstructionBlock* GetLastSibling();

    void WhenDragStarts() override;
    void WhenSnapped(SnapBlock& other) override;
    void WhenDropFailed() override;

    void Unsnap() override;

    std::string ToLuaString() const;

protected:
    ~InstructionBlock() override;
};