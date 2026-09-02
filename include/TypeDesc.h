#pragma once
#include <string>
#include <map>
#include <imgui.h>
#include <nlohmann/json.hpp>

struct StructDesc;

struct TypeDesc {
    TypeDesc() = default;
    TypeDesc(const std::string& parsetype);

    const std::string* baseType = nullptr;
    const StructDesc* structref = nullptr;

    const std::string& GetTypeName() const;
    
    bool isConst = false;
    bool isUnsigned = false;

    int pointerDepth = 0;
    int arraySize = -1;

    inline bool isPointer() const {return pointerDepth > 0;}
    inline bool isArray() const {return arraySize >= 0;}
    inline bool isString() const {return baseType && *baseType == "char" && pointerDepth == 1;}

    struct Memory {
        size_t size = 0;
        size_t alignment = 0;
    };

    Memory memory;

    static void Init(const nlohmann::json& jsonstructs, const nlohmann::json& jsonaliases, const nlohmann::json& jsoncallbacks);
};

struct StructDesc { // ModelAnimPose referring to Transform* as a pointer not handled atm
    std::string name;
    std::string desc;

    struct Field {
        TypeDesc type;
        std::string name;
        std::string desc;
    };

    std::vector<Field> fields;
};