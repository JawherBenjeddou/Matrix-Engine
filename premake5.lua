-- Matrix Engine workspace
-- Generate project files with:  vendor\premake\premake5.exe vs2022   (or vs2026)
-- Or simply run build.bat / scripts\setup.py which does it for you.

workspace "Matrix"
   architecture "x86_64"
   startproject "Editor"
   configurations { "Debug", "Release" }
   multiprocessorcompile "On"

   -- All binaries land in one folder so the engine DLL sits next to the executables.
   targetdir ("%{wks.location}/build/bin/%{cfg.buildcfg}")
   objdir    ("%{wks.location}/build/bin-int/%{cfg.buildcfg}/%{prj.name}")

   -- Every executable runs with the repository root as working directory.
   debugdir "%{wks.location}"

   filter "system:windows"
      systemversion "latest"
      staticruntime "Off"
      defines { "_CRT_SECURE_NO_WARNINGS", "NOMINMAX", "WIN32_LEAN_AND_MEAN" }
      buildoptions { "/utf-8" }

   filter "configurations:Debug"
      defines { "MX_DEBUG_MODE" }
      runtime "Debug"
      symbols "On"
      optimize "Off"

   filter "configurations:Release"
      defines { "MX_RELEASE_MODE" }
      runtime "Release"
      symbols "On"
      optimize "Speed"

   filter {}

-- Shared include directories, relative to the workspace root.
IncludeDir = {}
IncludeDir["GLFW"]      = "%{wks.location}/Dependencies/GLFW/include"
IncludeDir["GLEW"]      = "%{wks.location}/Dependencies/GLEW/include"
IncludeDir["glm"]       = "%{wks.location}/Dependencies/glm"
IncludeDir["entt"]      = "%{wks.location}/Dependencies/entt/single_include/entt"
IncludeDir["spdlog"]    = "%{wks.location}/Dependencies/spdlog/include"
IncludeDir["stb"]       = "%{wks.location}/Dependencies/stb"
IncludeDir["imgui"]     = "%{wks.location}/Dependencies/imgui"
IncludeDir["imgui_backends"] = "%{wks.location}/Dependencies/imgui/backends"
IncludeDir["json"]      = "%{wks.location}/Dependencies/json"
IncludeDir["miniaudio"] = "%{wks.location}/Dependencies/miniaudio"
IncludeDir["Matrix"]    = "%{wks.location}/MatrixEngine/src"
IncludeDir["Matrix_mx"] = "%{wks.location}/MatrixEngine/src/mx"

LibraryDir = {}
LibraryDir["GLFW"] = "%{wks.location}/Dependencies/GLFW/lib-vc2022"
LibraryDir["GLEW"] = "%{wks.location}/Dependencies/GLEW/lib"

-- Settings every project that consumes the engine needs.
function UseMatrixEngine()
   includedirs {
      "%{IncludeDir.Matrix}",
      "%{IncludeDir.Matrix_mx}",
      "%{IncludeDir.GLFW}",
      "%{IncludeDir.GLEW}",
      "%{IncludeDir.glm}",
      "%{IncludeDir.entt}",
      "%{IncludeDir.spdlog}",
      "%{IncludeDir.stb}",
      "%{IncludeDir.imgui}",
      "%{IncludeDir.imgui_backends}",
      "%{IncludeDir.json}",
   }
   defines {
      "GLEW_STATIC",
      "GLFW_INCLUDE_NONE",
      "GLM_ENABLE_EXPERIMENTAL",
      "IMGUI_API=__declspec(dllimport)",
   }
   libdirs { "%{LibraryDir.GLFW}", "%{LibraryDir.GLEW}" }
   links { "Matrix", "glfw3.lib", "opengl32.lib", "glew32s.lib" }
   dependson { "Matrix" }
end

group "Engine"
   include "MatrixEngine"
group "Tools"
   include "Editor"
group "Games"
   include "Sandbox"
group "Tests"
   include "Tests"
group ""
