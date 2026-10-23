#pragma once
#include "Common.h"
///@file PlatformUtils.h  Native OS helpers (file dialogs, clipboard-free utilities).

namespace Matrix::util
{
    class MATRIX_DLL FileDialogs
    {
    public:
        /// Shows a native "Open File" dialog. filter uses the Win32 double-null format,
        /// e.g. "Matrix Scene (*.mxscene)\0*.mxscene\0". Returns an empty string if cancelled.
        static std::string OpenFile(const char* filter, const std::filesystem::path& initialDirectory = {});

        /// Shows a native "Save File" dialog. Returns an empty string if cancelled.
        static std::string SaveFile(const char* filter, const std::filesystem::path& initialDirectory = {}, const char* defaultExtension = nullptr);
    };

    class MATRIX_DLL Platform
    {
    public:
        /// Opens a file or folder with the default shell handler (e.g. the OS file explorer).
        static void OpenInShell(const std::filesystem::path& path);
    };
}
