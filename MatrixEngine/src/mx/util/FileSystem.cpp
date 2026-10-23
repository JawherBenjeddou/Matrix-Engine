#include "pch.h"
#include "FileSystem.h"

#include <Windows.h>

namespace Matrix::util
{
    namespace fs = std::filesystem;

    bool FileSystem::LooksLikeRoot(const fs::path& candidate)
    {
        std::error_code ec;
        return fs::is_directory(candidate / "resources" / "shaders", ec);
    }

    void FileSystem::Init()
    {
        if (s_Initialized)
            return;

        std::error_code ec;
        std::vector<fs::path> starts = { fs::current_path(ec), GetExecutableDirectory() };
        for (const auto& start : starts)
        {
            fs::path current = start;
            for (int depth = 0; depth < 8 && !current.empty(); ++depth)
            {
                if (LooksLikeRoot(current))
                {
                    s_Root = fs::absolute(current, ec).lexically_normal();
                    s_Initialized = true;
                    return;
                }
                fs::path parent = current.parent_path();
                if (parent == current)
                    break;
                current = parent;
            }
        }

        s_Root = fs::current_path(ec);
        s_Initialized = true;
    }

    const fs::path& FileSystem::GetRootDirectory()
    {
        if (!s_Initialized)
            Init();
        return s_Root;
    }

    void FileSystem::SetRootDirectory(const fs::path& root)
    {
        s_Root = root.lexically_normal();
        s_Initialized = true;
    }

    fs::path FileSystem::GetAssetsDirectory()
    {
        return GetRootDirectory() / "assets";
    }

    fs::path FileSystem::GetResourcesDirectory()
    {
        return GetRootDirectory() / "resources";
    }

    fs::path FileSystem::Resolve(const fs::path& path)
    {
        if (path.empty())
            return path;
        if (path.is_absolute())
            return path.lexically_normal();
        return (GetRootDirectory() / path).lexically_normal();
    }

    std::string FileSystem::MakeRelative(const fs::path& path)
    {
        std::error_code ec;
        fs::path absolute = path.is_absolute() ? path.lexically_normal() : fs::absolute(path, ec).lexically_normal();
        fs::path relative = absolute.lexically_relative(GetRootDirectory());
        if (relative.empty() || relative.string().rfind("..", 0) == 0)
            return Normalize(absolute);
        return Normalize(relative);
    }

    bool FileSystem::Exists(const fs::path& path)
    {
        std::error_code ec;
        return fs::exists(Resolve(path), ec);
    }

    std::optional<std::string> FileSystem::ReadText(const fs::path& path)
    {
        std::ifstream stream(Resolve(path), std::ios::in | std::ios::binary);
        if (!stream)
            return std::nullopt;
        std::stringstream buffer;
        buffer << stream.rdbuf();
        return buffer.str();
    }

    bool FileSystem::WriteText(const fs::path& path, const std::string& text)
    {
        fs::path resolved = Resolve(path);
        std::error_code ec;
        if (resolved.has_parent_path())
            fs::create_directories(resolved.parent_path(), ec);
        std::ofstream stream(resolved, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!stream)
            return false;
        stream << text;
        return static_cast<bool>(stream);
    }

    fs::path FileSystem::GetExecutableDirectory()
    {
        wchar_t buffer[MAX_PATH];
        DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        if (length == 0)
            return fs::current_path();
        return fs::path(buffer).parent_path();
    }

    std::string FileSystem::Normalize(const fs::path& path)
    {
        std::string result = path.generic_string();
        std::replace(result.begin(), result.end(), '\\', '/');
        return result;
    }
}
