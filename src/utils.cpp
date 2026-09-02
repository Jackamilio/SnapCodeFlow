#include <string>
#include <set>

std::set<std::string> basicTypes = {
    "...",
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

#ifdef _WIN32

#include <windows.h>
#include <commdlg.h>

// needs -lcomdlg32
std::string OpenFileDialog() {
    char path[MAX_PATH] = {};

    OPENFILENAMEA ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;

    ofn.lpstrFilter =
        "Tous les fichiers\0*.*\0"
        "Fichiers texte\0*.txt\0";

    ofn.nFilterIndex = 1;

    ofn.Flags =
        OFN_PATHMUSTEXIST |
        OFN_FILEMUSTEXIST |
        OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn))
    {
        return path;
    }

    return {};
}

#elif defined(__linux__)

std::string OpenFileDialog() {
    return "";
}

#endif