#include "SnapBlockDesc.h"
#include "utils.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

using namespace nlohmann;
using namespace std;

map<string, SnapBlocDesc::List> blocsPerTags;

const char* tagall = "_all";
const char* tagnone = "_notag";

const char* SnapBlocDesc::Load() {
    LOAD_RAYLIB_API_JSON

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

    int order=0;
    for(const auto& f : functions) {
        string fname = f["name"];
        SnapBlocDesc* bloc = new SnapBlocDesc{order++,fname,f["description"],f["returnType"],vector<Parameter>(),set<string>()};
        if (f.contains("params")) {
            for(const auto& p: f["params"]) {
                bloc->params.push_back({p["type"],p["name"]});
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

void SnapBlocDesc::Unload() {
    for(auto& sbd : blocsPerTags[tagall]) {
        delete sbd;
    }
    blocsPerTags.clear();
}

const SnapBlocDesc::List* SnapBlocDesc::GetListFromTag(const std::string& tag) {
    auto it = blocsPerTags.find(tag);
    return it == blocsPerTags.end() ? nullptr : &it->second;
}
const SnapBlocDesc::List& SnapBlocDesc::GetCompleteList() {
    return blocsPerTags[tagall];
}

void SnapBlocDesc::List::add(SnapBlocDesc *d) {
    assert(d && "SnapBlockDesc::List::add item must not be null");
    assert(map.find(d->name) == map.end() && "SnapBlockDesc::List::add can't add duplicate");
    map[d->name] = d;
    set.emplace(d);
}

const SnapBlocDesc* SnapBlocDesc::List::operator[](const std::string &str) const
{
    auto it = map.find(str);
    return it == map.end() ? nullptr : it->second;
}

const std::set<std::string> SnapBlocDesc::GetTags()
{
    set<string> tags;
    for(const auto& t : blocsPerTags) {
        tags.emplace(t.first);
    }
    return tags;
}