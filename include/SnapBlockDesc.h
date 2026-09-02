#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include "TypeDesc.h"

struct SnapBlocDesc {
    static const char* Load();
    static void Unload();

    static const char* prefix;

    int order;
    std::string name;
    std::string desc;
    TypeDesc returntype;

    struct Parameter {
        TypeDesc type;
        std::string name;
    };

    std::vector<Parameter> params;

    std::set<std::string> tags;
    struct CompareOrder {
        bool operator()(const SnapBlocDesc* a, const SnapBlocDesc*b) const {
            return a->order < b->order;
        }
    };
    typedef std::set<SnapBlocDesc*,CompareOrder> OrderedSet;

    struct List {
        friend struct SnapBlocDesc;
        struct CompareOrder {
            bool operator()(const SnapBlocDesc* a, const SnapBlocDesc*b) const {
                return a->order < b->order;
            }
        };
        typedef std::set<SnapBlocDesc*,CompareOrder> OrderedSet;
        private:
        std::map<std::string, SnapBlocDesc*> map;
        OrderedSet set;
        void add(SnapBlocDesc* d);
        public:
        const SnapBlocDesc* operator[](const std::string& str) const;
        inline OrderedSet::const_iterator begin() const { return set.begin(); }
        inline OrderedSet::const_iterator end() const { return set.end(); }
        inline size_t size() const { return set.size(); };
    };

    static const List* GetListFromTag(const std::string& tag);
    static const List& GetCompleteList();
    static const std::set<std::string> GetTags();
};