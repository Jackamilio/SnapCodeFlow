//imgui_ext.h
#pragma once

#include "raylib.h"

//#define IMGUI_DEFINE_MATH_OPERATORS
#define IM_VEC2_CLASS_EXTRA                                             \
    constexpr ImVec2(const Vector2& v) : x(v.x), y(v.y) {}             \
    constexpr operator Vector2() const { return { x, y }; }
#include "imgui.h"