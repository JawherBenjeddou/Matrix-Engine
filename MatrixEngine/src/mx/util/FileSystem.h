#pragma once
#include "Common.h"
///@file FileSystem.h  Locates the project root and resolves asset paths relative to it.

namespace Matrix::util
{
    class MATRIX_DLL FileSystem
    {
    public:
        /// Finds the project root (the folder that contains "resources" and "assets") by walking up from
        /// the executable location and the current working directory. Falls back to the working directory.
        static void Init();

        static const std::filesystem::path& GetRootDirectory();
        static void SetRootDirectory(const std::filesystem::path& root);

        /// Returns root / "assets".
        static std::filesystem::path GetAssetsDirectory();

        /// Returns root / "resources".
        static std::filesystem::path GetResourcesDirectory();

        /// Resolves a path: absolute paths are returned unchanged, relative paths are joined with the root.
        static std::filesystem::path Resolve(const std::filesystem::path& path);

        /// Converts a path to a root-relative, forward-slash string suitable for saving in files.
        /// Paths outside of the root are returned as generic absolute paths.
        static std::string MakeRelative(const std::filesystem::path& path);

        static bool Exists(const std::filesystem::path& path);

        /// Reads an entire text file. Returns std::nullopt if the file could not be read.
        static std::optional<std::string> ReadText(const std::filesystem::path& path);

        /// Writes text to a file, creating parent directories if needed.
        static bool WriteText(const std::filesystem::path& path, const std::string& text);

        /// Directory of the running executable.
        static std::filesystem::path GetExecutableDirectory();

        /// Normalizes separators to forward slashes.
        static std::string Normalize(const std::filesystem::path& path);

    private:
        static bool LooksLikeRoot(const std::filesystem::path& candidate);
        inline static std::filesystem::path s_Root;
        inline static bool s_Initialized = false;
    };
}
