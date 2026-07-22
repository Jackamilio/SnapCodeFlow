#include "PuzzlePiece.h"

PuzzlePiece::PuzzlePiece(Vector2 p, WholePuzzle& puzzle, int id) : SnapBlock(p), puzzle(puzzle) {    
    size.x = puzzle.image.width / float(puzzle.columns);
    size.y = puzzle.image.height / float(puzzle.lines);

    x = id%puzzle.columns;
    y = id/puzzle.columns;

    float fw = float(puzzle.image.width);
    float fh = float(puzzle.image.height);

    uv_min = {float(x)   * size.x / fw, float(y)   * size.y / fh};
    uv_max = {float(x+1) * size.x / fw, float(y+1) * size.y / fh};
}

void PuzzlePiece::Draw(ImDrawList *drawList, const Vector2& pos) {
    drawList->AddImage(ImTextureID(puzzle.image.id),pos,pos+size,uv_min,uv_max);
}

bool PuzzlePiece::CanSnap(Vector2& io_at, const SnapBlock* from) const {

    const PuzzlePiece*p = dynamic_cast<const PuzzlePiece*>(from);
    if (!p) {
        return false;
    }

    if (
        (
            (x==p->x && abs(y-p->y)==1) ||
            (y==p->y && abs(x-p->x)==1)
        ) &&
        SnapBlock::CanSnap(io_at, from)
    ) {
        return
            (x==p->x && pos.x==io_at.x && signbit(y-p->y)==signbit(pos.y - io_at.y)) ||
            (y==p->y && pos.y==io_at.y && signbit(x-p->x)==signbit(pos.x - io_at.x));
    }
    return false;
}