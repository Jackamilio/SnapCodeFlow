#include "bindings.h"
#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>

using namespace nlohmann;
using namespace std;

set<string> basicTypes = {
    "void",
    "char",
    "bool",
    "short",
    "int",
    "long",
    "float",
    "double",

    "size_t",

    "int8_t",
    "uint8_t",
    "int16_t",
    "uint16_t",
    "int32_t",
    "uint32_t",
    "int64_t",
    "uint64_t"
};

#define UNROLLDEF(func) \
    func(INT) \
    func(FLOAT) \
    func(FLOAT_MATH) \
    func(STRING) \
    func(COLOR)
#define ENUMDEF(x) x,
#define PAIRDEF(x) {#x, x},

typedef enum {
    INVALID = 0, UNROLLDEF(ENUMDEF)
} EnumValidDefineTypes;

map<string, int> validDefineTypes = {
    UNROLLDEF(PAIRDEF)
};

string GetBaseType(string type) {
    istringstream iss(type);
    string word;
    string base;

    while (iss >> word) {
        if (word == "const" ||
            word == "unsigned" ||
            word == "signed")
            continue;

        if (word.find('*') != string::npos)
            word.erase(remove(word.begin(), word.end(), '*'), word.end());

        auto pos = word.find('[');
        if (pos != string::npos)
            word = word.substr(0, pos);

        if (!word.empty())
            base = word;
    }

    return base;
}

string RemoveFloatSuffix(string value) {
    for (size_t i = 1; i < value.size(); ++i) {
        if (value[i] == 'f' && 
            (isdigit(value[i - 1]) || value[i - 1] == '.')) {
            value.erase(i, 1);
            --i;
        }
    }

    return value;
}

void PrintParams(ofstream& out, const json& from) {
    out << '(';
    if (from.contains("params")) {
        bool first = true;
        for(const auto& p : from["params"]) {
            if (!first) out << ", ";
            first = false;
            string type = (string)p["type"];
            out << type;
            if (type != "...") out << ' ' << (string)p["name"];
        }
    }
    out << ");";
}

const char* GenerateLuaRaylibBindings(const char* outputfile) {
    // open the json and output files
    ifstream file("raylib_api.json");
    if (!file.is_open()) return "Could not open raylib_api.json";
    json rlapi;
    try { rlapi = json::parse(file); }
    catch (const json::parse_error& e) {
        static char buf[512];
        sprintf(buf, "Error while parsing raylib.json : %s", e.what());
        return buf;
    }
    ofstream out(outputfile);
    if (!out.is_open()) {
        return "Could not create the output file.";
    }

    // first lines of the ouput
    out << "local ffi = require(\"ffi\")" << endl << endl;
    out << "ffi.cdef[[" << endl;

    // acquire the main top fields
#define SHORTCUT(x) if (!rlapi.contains(#x)) {return "Field " #x " is not present in raylib_api.json"; } json x = rlapi[#x]
    SHORTCUT(defines);
    SHORTCUT(structs);
    SHORTCUT(aliases);
    SHORTCUT(enums);
    SHORTCUT(callbacks);
    SHORTCUT(functions);
#undef SHORTCUT

    // first try to detect unknown types
    set<string> knownTypes = basicTypes;
    for(const auto& a: aliases) {
        knownTypes.emplace(GetBaseType(a["name"]));
    }
    for(const auto& s: structs) {
        knownTypes.emplace(s["name"]);
    }
    set<string> unknownTypes;
    for(const auto& s: structs) {
        for(const auto& f: s["fields"]) {
            string type = GetBaseType(f["type"]);
            if (knownTypes.find(type) == knownTypes.end()) {
                unknownTypes.emplace(type);
            }
        }
    }

    // print out unknown types as opaque types
    out << "// Detected unknown types, declared as opaque types :" << endl;
    for(const auto& ut : unknownTypes) {
        out << "typedef struct " << ut << ' ' << ut << ';' << endl;
    }
    out << endl;

    // construct a map of aliases
    map<string, vector<string>> aliasmap;
    for(const auto& a : aliases) {
        aliasmap[a["type"]].push_back(a["name"]);
    }

    // print out structs
    for(const auto& s : structs) {
        string name = (string)s["name"];
        out << "typedef struct " << name << " { ";
        for(const auto& f : s["fields"]) {
            string type = f["type"];
            size_t bracketl = type.find('[');
            size_t bracketr = type.find(']');
            string brackets;
            if (bracketl != string::npos && bracketr != string::npos) {
                brackets = type.substr(bracketl, bracketr-bracketl+1);
                type = type.substr(0, bracketl);
            }
            out << type << ' ' << (string)f["name"] << brackets << "; ";
        }
        out << "} " << (string)s["name"] << ';' << endl;
        // immediatly print aliases of this struct if any
        auto alias = aliasmap.find(name);
        if (alias != aliasmap.end()) {
            for (const auto& a : alias->second) {
                out << "typedef " << name << ' ' << a << ';' << endl;
            }
        }
    }
    out << endl;

    // print out enums
    for(const auto& e : enums) {
        int align = 0;
        for(const auto& v : e["values"]) {
            int len = v["name"].get<string>().size();
            if (len > align) align = len;
        }
        out << "typedef enum { // " << (string)e["description"] << endl;
        bool first = true;
        for(const auto& v : e["values"]) {
            if (!first) out << ',' << endl;
            first = false;
            string name = v["name"];
            out << "    " << name;
            for(int i = name.size(); i<align; ++i) {
                out << ' ';
            }
            out << " = " << v["value"];
        }
        out << endl << "} " << (string)e["name"] << ';' << endl << endl;
    }

    // print out callbacks
    for(const auto& c : callbacks) {
        out << "typedef " << (string)c["returnType"] << " (*" << (string)c["name"] << ')';
        PrintParams(out, c);
        out << endl;
    }
    out << endl;

    // print out functions
    for(const auto& f: functions) {
        out << (string)f["returnType"] << ' ' << (string)f["name"];
        PrintParams(out, f);
        out << endl;
    }

    // end the cdecl
    out << "]]" << endl << endl << "local rl = ffi.load(\"libraylib\")" << endl;

    // put define constants in a metatable
    out << "local coltype = ffi.typeof(\"Color\")" << endl << endl;
    out << "local PI = 3.141592653589793" << endl; // for FLOAT_MATH, by hand, I don't want to write a preprocessor for two values
    out << "local M = setmetatable({" << endl;
    for(const auto& d: defines) {
        string name = d["name"];
        switch(validDefineTypes[d["type"]]) {
            case INT:
            case FLOAT:
            case STRING:
                out << "    " << name << " = " << d["value"] << ',' << endl;
            break;
            case FLOAT_MATH:
                out << "    " << name << " = " << RemoveFloatSuffix(d["value"]) << ',' << endl;
            break;
            case COLOR:
            {
                string value = d["value"];
                size_t l = value.find('{');
                size_t r = value.find('}');
                if (l != string::npos && l != string::npos) {
                    value = value.substr(l+1, r-l-1);
                    out << "    " << name << " = coltype(" << value << ")," << endl;
                }                
            }
            default: break;
        }
    }
    out << "}, {" << endl << "    __index = rl" << endl << "})";

    // we're done
    out << "return M";
    return nullptr;
}