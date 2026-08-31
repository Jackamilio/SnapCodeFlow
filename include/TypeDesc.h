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
    int arraySize = -1;

    inline bool isPointer() const {return pointerDepth > 0;}
    inline bool isArray() const {return arraySize >= 0;}
    inline bool isString() const {return baseType == "char" && pointerDepth == 1;}

    struct Memory {
        size_t size = 0;
        size_t alignment = 0;
    };

    Memory memory;

    static void Init();
};