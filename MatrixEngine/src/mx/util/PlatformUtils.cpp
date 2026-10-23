#include "pch.h"
#include "PlatformUtils.h"

#include <Windows.h>
#include <commdlg.h>
#include <shellapi.h>

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

namespace Matrix::util
{
    namespace
    {
        HWND GetOwnerWindow()
        {
            GLFWwindow* window = glfwGetCurrentContext();
            return window ? glfwGetWin32Window(window) : nullptr;
        }
    }

    std::string FileDialogs::OpenFile(const char* filter, const std::filesystem::path& initialDirectory)
    {
        OPENFILENAMEA ofn{};
        CHAR fileBuffer[MAX_PATH] = { 0 };
        std::string initial = initialDirectory.string();

        ofn.lStructSize = sizeof(OPENFILENAMEA);
        ofn.hwndOwner = GetOwnerWindow();
        ofn.lpstrFile = fileBuffer;
        ofn.nMaxFile = sizeof(fileBuffer);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.lpstrInitialDir = initial.empty() ? nullptr : initial.c_str();
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn) == TRUE)
            return std::string(ofn.lpstrFile);
        return {};
    }

    std::string FileDialogs::SaveFile(const char* filter, const std::filesystem::path& initialDirectory, const char* defaultExtension)
    {
        OPENFILENAMEA ofn{};
        CHAR fileBuffer[MAX_PATH] = { 0 };
        std::string initial = initialDirectory.string();

        ofn.lStructSize = sizeof(OPENFILENAMEA);
        ofn.hwndOwner = GetOwnerWindow();
        ofn.lpstrFile = fileBuffer;
        ofn.nMaxFile = sizeof(fileBuffer);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.lpstrInitialDir = initial.empty() ? nullptr : initial.c_str();
        ofn.lpstrDefExt = defaultExtension;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        if (GetSaveFileNameA(&ofn) == TRUE)
            return std::string(ofn.lpstrFile);
        return {};
    }

    void Platform::OpenInShell(const std::filesystem::path& path)
    {
        ShellExecuteW(nullptr, L"open", path.wstring().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}
