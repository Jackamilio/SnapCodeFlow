#include "typedesc.h"
#include <sstream>
#include <raylib.h>
#include <map>
#include "utils.h"

std::map<std::string, StructDesc> structlist;
std::map<std::string, TypeDesc::Memory> typedescmem;

TypeDesc::TypeDesc(const std::string &parsetype) {
    std::istringstream stream(parsetype);
    std::string word;

    std::string foundBaseType;

    while (stream >> word) {
        if (word == "const") {
            isConst = true;
            continue;
        }

        if (word == "unsigned") {
            isUnsigned = true;
            continue;
        }

        // Les * peuvent être collées au nom du type
        while (!word.empty() && word.back() == '*') {
            ++pointerDepth;
            word.pop_back();
        }

        // Tableau : float[4]
        auto arrayStart = word.find('[');

        if (arrayStart != std::string::npos) {
            auto arrayEnd = word.find(']', arrayStart);

            if (arrayEnd != std::string::npos) {
                arraySize = 0;

                std::string size =
                    word.substr(
                        arrayStart + 1,
                        arrayEnd - arrayStart - 1
                    );

                if (!size.empty())
                    arraySize = std::stoi(size);

                word.erase(arrayStart);
            }
        }

        if (!word.empty()) {
            if (!foundBaseType.empty())
                baseType += ' ';

            foundBaseType += word;
        }
    }

    std::set<std::string>::iterator itbasetype = basicTypes.find(foundBaseType);
    if (itbasetype != basicTypes.end()) baseType = &(*itbasetype);

    std::map<std::string, StructDesc>::iterator itstruct = structlist.find(foundBaseType);
    if (itstruct != structlist.end()) structref = &itstruct->second;

    assert(!(baseType != nullptr && structref != nullptr) && "Can't be a base type and a struct at the same time");

    bool opaque = baseType == nullptr && structref == nullptr;

    assert(!(opaque && pointerDepth == 0) && "Can't be opaque and not be a pointer");

    if (opaque) {
        StructDesc& sd = structlist[foundBaseType];
        sd.name = foundBaseType;
        sd.desc = "Opaque type.";
    }

    std::map<std::string, Memory>::iterator it = typedescmem.find(foundBaseType);
    if (it != typedescmem.end()) {
        memory = it->second;
    }
}

const std::string& TypeDesc::GetTypeName() const {
    return baseType ? *baseType : structref->name;
}

void TypeDesc::Init(const nlohmann::json& structs, const nlohmann::json& aliases, const nlohmann::json& callbacks) {
#define REGISTER_MEMORY_DEF(name) typedescmem[#name] = {sizeof(name), alignof(name)};

    REGISTER_MEMORY_DEF(bool)
    REGISTER_MEMORY_DEF(char)
    REGISTER_MEMORY_DEF(double)
    REGISTER_MEMORY_DEF(float)
    REGISTER_MEMORY_DEF(int)
    REGISTER_MEMORY_DEF(long)
    REGISTER_MEMORY_DEF(void*)
    REGISTER_MEMORY_DEF(Color)
    REGISTER_MEMORY_DEF(Matrix)
    REGISTER_MEMORY_DEF(Vector2)
    REGISTER_MEMORY_DEF(Vector3)
    REGISTER_MEMORY_DEF(Vector4)

    for(const auto& c : callbacks) { // callbacks as structs for now for simplicity
        StructDesc& sd = structlist[c["name"]];
        sd.name = c["name"];
        sd.desc = std::string(c["description"]) + std::string(". Returns ") + std::string(c["returnType"]);
    }

    std::map<std::string, std::vector<std::pair<std::string,std::string>>> aliasmap;
    for(const auto& a : aliases) {
        std::string name = a["name"];
        std::string type = a["type"];
        if (name.front() == '*') {
            name.erase(0, 1);
            //type.insert(0, "*");
        }
        aliasmap[type].push_back({name,a["description"]});
    }

    for(const auto& s : structs) {
        std::string name = s["name"];
        StructDesc& sd = structlist[name];
        sd.name = name;
        sd.desc = s["description"];
        for (const auto& f : s["fields"]) {
            sd.fields.push_back({{f["type"]},f["name"],f["description"]});
        }

        auto it = aliasmap.find(name);
        if (it != aliasmap.end()) {
            for (auto& a : it->second) {
                StructDesc& sd = structlist[a.first];
                sd.name = a.first;
                sd.desc = a.second;
                sd.fields = structlist[name].fields;
            }
        }
    }
}
