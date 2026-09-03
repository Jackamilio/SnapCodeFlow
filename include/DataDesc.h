#pragma once

#include <string>
#include <map>
#include <set>

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
};

struct FieldDesc {
    TypeDesc type;
    std::string name;
    std::string desc;
};

typedef std::vector<FieldDesc> FieldsDesc;

struct StructDesc { // ModelAnimPose referring to Transform* as a pointer not handled atm
    std::string name;
    std::string desc;

    FieldsDesc fields;
};

struct FunctionDesc {
    int order;

    TypeDesc returntype;
    std::string name;
    std::string desc;

    FieldsDesc params;

    std::set<std::string> tags;

    struct CompareOrder {
        bool operator()(const FunctionDesc* a, const FunctionDesc*b) const {
            return a->order < b->order;
        }
    };
    typedef std::set<FunctionDesc*,CompareOrder> OrderedSet;

    struct List {
        friend struct DataDesc;
        struct CompareOrder {
            bool operator()(const FunctionDesc* a, const FunctionDesc*b) const {
                return a->order < b->order;
            }
        };
        typedef std::set<FunctionDesc*,CompareOrder> OrderedSet;
        private:
        std::map<std::string, FunctionDesc*> map;
        OrderedSet set;
        void add(FunctionDesc* d);
        public:
        const FunctionDesc* operator[](const std::string& str) const;
        inline OrderedSet::const_iterator begin() const { return set.begin(); }
        inline OrderedSet::const_iterator end() const { return set.end(); }
        inline size_t size() const { return set.size(); };
    };
};

struct DataDesc {
    static const char* Load();
    static void Unload();

    static const FunctionDesc::List* GetFunctionsListFromTag(const std::string& tag);
    static const FunctionDesc::List& GetCompleteFunctionsList();
    static const std::set<std::string> GetFunctionTags();

    static const char* prefix;
};