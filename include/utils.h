#pragma once

#include <raylib.h>
#include "imgui_ext.h"
#include <string>

inline ImU32 toImGuiCol(Color c)
{
    return IM_COL32(c.r, c.g, c.b, c.a);
}

std::string OpenFileDialog();