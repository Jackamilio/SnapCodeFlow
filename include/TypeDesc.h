#pragma once
#include <string>
#include <map>
#include <imgui.h>

struct TypeDesc {
    TypeDesc() = default;
    TypeDesc(const std::string& parsetype);

    std::string baseType;

    bool isConst = false;
    bool isUnsigned = false;

    int pointerDepth = 0;

    bool isArray = false;
    int arraySize = 0;

    struct Memory {
        size_t size = 0;
        size_t alignment = 0;
    };

    Memory memory;

    static void Init();
};