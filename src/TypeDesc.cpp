#include "typedesc.h"
#include <sstream>
#include <raylib.h>

static std::map<std::string, TypeDesc::Memory> typedescmem;

TypeDesc::TypeDesc(const std::string &parsetype) {
    std::istringstream stream(parsetype);
    std::string word;

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
                isArray = true;

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
            if (!baseType.empty())
                baseType += ' ';

            baseType += word;
        }
    }

    std::map<std::string, Memory>::iterator it = typedescmem.find(baseType);
    if (it != typedescmem.end()) {
        memory = it->second;
    }
}

void TypeDesc::Init() {
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
}
