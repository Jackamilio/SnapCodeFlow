#pragma once

#include <raylib.h>
#include "imgui_ext.h"
#include <string>
#include <set>

inline ImU32 toImGuiCol(Color c) {
    return IM_COL32(c.r, c.g, c.b, c.a);
}

extern std::set<std::string> basicTypes;

std::string OpenFileDialog();

#define JSONTOPSHORTCUT(x) if (!rlapi.contains(#x)) {return "Field " #x " is not present in raylib_api.json"; } json x = rlapi[#x]
#define LOAD_RAYLIB_API_JSON \
    /*first open and parse the json*/ \
    ifstream file("raylib_api.json"); \
    if (!file.is_open()) return "Could not open raylib_api.json"; \
    json rlapi; \
    try { rlapi = json::parse(file); } \
    catch (const json::parse_error& e) { \
        static char buf[512]; \
        sprintf(buf, "Error while parsing raylib.json : %s", e.what()); \
        return buf; \
    } \
     \
    /*acquire the main top fields*/ \
    JSONTOPSHORTCUT(defines); \
    JSONTOPSHORTCUT(structs); \
    JSONTOPSHORTCUT(aliases); \
    JSONTOPSHORTCUT(enums); \
    JSONTOPSHORTCUT(callbacks); \
    JSONTOPSHORTCUT(functions);
    