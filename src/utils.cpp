#include <windows.h>
#include <commdlg.h>
#include <string>

// needs -lcomdlg32
std::string OpenFileDialog()
{
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