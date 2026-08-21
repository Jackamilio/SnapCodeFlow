#pragma once
#include "SnapBlock.h"

struct PuzzlePiece : SnapBlock {
    struct WholePuzzle {
        Texture image;

        int columns;
        int lines;
    };

    WholePuzzle& puzzle;

    int x;
    int y;

    Vector2 uv_min;
    Vector2 uv_max;

    PuzzlePiece(Vector2 p, WholePuzzle& puzzle, int id);

    virtual void DrawLining(ImDrawList *drawList, const Vector2& pos);
    virtual bool CanSnap(Vector2& io_at, const SnapBlock* from) const;
};