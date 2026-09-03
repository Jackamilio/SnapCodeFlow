#include "DataDesc.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <raylib.h>
#include "utils.h"

using namespace nlohmann;
using namespace std;

map<string, StructDesc> structlist;
map<string, TypeDesc::Memory> typedescmem;
map<string, FunctionDesc::List> blocsPerTags;
const char* DataDesc::prefix = "rl."; // maybe give a way to the user to change it in the future

const char* tagall = "_all";
const char* tagnone = "_notag";

TypeDesc::TypeDesc(const string &parsetype) {
    istringstream stream(parsetype);
    string word;

    string foundBaseType;

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

        if (arrayStart != string::npos) {
            auto arrayEnd = word.find(']', arrayStart);

            if (arrayEnd != string::npos) {
                arraySize = 0;

                string size =
                    word.substr(
                        arrayStart + 1,
                        arrayEnd - arrayStart - 1
                    );

                if (!size.empty())
                    arraySize = stoi(size);

                word.erase(arrayStart);
            }
        }

        if (!word.empty()) {
            if (!foundBaseType.empty())
                baseType += ' ';

            foundBaseType += word;
        }
    }

    set<string>::iterator itbasetype = basicTypes.find(foundBaseType);
    if (itbasetype != basicTypes.end()) baseType = &(*itbasetype);

    map<string, StructDesc>::iterator itstruct = structlist.find(foundBaseType);
    if (itstruct != structlist.end()) structref = &itstruct->second;

    assert(!(baseType != nullptr && structref != nullptr) && "Can't be a base type and a struct at the same time");

    bool opaque = baseType == nullptr && structref == nullptr;

    assert(!(opaque && pointerDepth == 0) && "Can't be opaque and not be a pointer");

    if (opaque) {
        StructDesc& sd = structlist[foundBaseType];
        sd.name = foundBaseType;
        sd.desc = "Opaque type.";
    }

    map<string, Memory>::iterator it = typedescmem.find(foundBaseType);
    if (it != typedescmem.end()) {
        memory = it->second;
    }
}

const string& TypeDesc::GetTypeName() const {
    return baseType ? *baseType : structref->name;
}

void InitTypeDesc(const nlohmann::json& structs, const nlohmann::json& aliases, const nlohmann::json& callbacks) {
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
        sd.desc = string(c["description"]) + string(". Returns ") + string(c["returnType"]);
    }

    map<string, vector<pair<string,string>>> aliasmap;
    for(const auto& a : aliases) {
        string name = a["name"];
        string type = a["type"];
        if (name.front() == '*') {
            name.erase(0, 1);
            //type.insert(0, "*");
        }
        aliasmap[type].push_back({name,a["description"]});
    }

    for(const auto& s : structs) {
        string name = s["name"];
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

void FunctionDesc::List::add(FunctionDesc *d) {
    assert(d && "SnapBlockDesc::List::add item must not be null");
    assert(map.find(d->name) == map.end() && "SnapBlockDesc::List::add can't add duplicate");
    map[d->name] = d;
    set.emplace(d);
}

const FunctionDesc* FunctionDesc::List::operator[](const std::string &str) const {
    auto it = map.find(str);
    return it == map.end() ? nullptr : it->second;
}

const char* DataDesc::Load() {
    LOAD_RAYLIB_API_JSON

    //  ----------- TAGS ----------- 
    ifstream tagfile("raylib_tags.csv");
    if(!tagfile.is_open()) {
        return "Could not open \"raylib_tags.csv\"";
    }

    map<string,set<string>> tagsperfunc;
    string line;
    while(getline(tagfile,line)) {
        stringstream ss(line);
        string func;
        getline(ss, func, ',');
        if (!func.empty()) {
            string tag;
            while(getline(ss, tag, ',')) {
                if (!tag.empty()) tagsperfunc[func].emplace(tag);
            }
        }
    }

    // ----------- FUNCTIONS ----------- 
    InitTypeDesc(structs, aliases, callbacks);
    int order=0;
    for(const auto& f : functions) {
        string fname = f["name"];
        FunctionDesc* bloc = new FunctionDesc{order++,TypeDesc(f["returnType"]),fname,f["description"],FieldsDesc(),set<string>()};
        if (f.contains("params")) {
            for(const auto& p: f["params"]) {
                bloc->params.push_back({TypeDesc(p["type"]),p["name"],""});
            }            
        }
        blocsPerTags[tagall].add(bloc);
        if (tagsperfunc[fname].empty()) {
            blocsPerTags[tagnone].add(bloc);
        }
        else {
            bloc->tags = tagsperfunc[fname];
            for(const auto& tag : tagsperfunc[fname]) {
                blocsPerTags[tag].add(bloc);
            }
        }
    }

    return nullptr;
}

void DataDesc::Unload() {
    for(auto& sbd : blocsPerTags[tagall]) {
        delete sbd;
    }
    blocsPerTags.clear();
}

const FunctionDesc::List* DataDesc::GetFunctionsListFromTag(const std::string& tag) {
    auto it = blocsPerTags.find(tag);
    return it == blocsPerTags.end() ? nullptr : &it->second;
}

const FunctionDesc::List& DataDesc::GetCompleteFunctionsList() {
    return blocsPerTags[tagall];
}

const std::set<std::string> DataDesc::GetFunctionTags() {
    set<string> tags;
    for(const auto& t : blocsPerTags) {
        tags.emplace(t.first);
    }
    return tags;
}
