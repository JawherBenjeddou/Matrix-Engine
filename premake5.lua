-- premake5.lua

-- the workspace
workspace "Matrix"
   configurations { "Debug", "Release" }
   platforms { "Win64" }
   
   -- Configure platform-specific settings
   filter { "platforms:Win64" }
      system "Windows"
      architecture "x86_64"

   -- Configure Release configuration
   filter "configurations:Release"
      defines {
         "_CRT_SECURE_NO_WARNINGS",
         "MX_RELEASE_MODE",
         "GLEW_STATIC",
         "MX_BUILD_DLL"
      }
      optimize "On"
      
 -- additional include directories for both configurations (outside of filter blocks)
   includedirs {
      "Dependencies/entt/single_include/entt",
      "Dependencies/GLFW/include",
      "Dependencies/GLEW/include",
      "MatrixEngine/src/mx",
      "Dependencies/glm",
      "MatrixEngine/src",
      "Dependencies/spdlog/include",
      "Dependencies/stb",
      "Dependencies/imgui/backends",
      "Dependencies/imgui"
   }
      pchheader "pch.h"
      pchsource "pch.cpp"

   -- Configure Debug configuration
   filter "configurations:Debug"
      defines {
         "_CRT_SECURE_NO_WARNINGS",
         "MX_DEBUG_MODE",
         "GLEW_STATIC",
         "MX_BUILD_DLL"
      }
      symbols "On"

-- additional include directories for both configurations (outside of filter blocks)
includedirs {
   "Dependencies/entt/single_include/entt",
   "Dependencies/GLFW/include",
   "Dependencies/GLEW/include",
   "MatrixEngine/src/mx",
   "Dependencies/glm",
   "MatrixEngine/src",
   "Dependencies/spdlog/include",
   "Dependencies/stb",
   "Dependencies/imgui/backends",
   "Dependencies/imgui"
}

-- precompiled header settings for both configurations (outside of filter blocks)
pchheader "pch.h"
pchsource "pch.cpp"

-- project configurations
project "Engine"
   kind "SharedLib"  -- Shared library type
   language "C++"
   cppdialect "C++20"  -- C++ version Matrix uses
   targetdir ("build/bin/%{prj.name}/%{cfg.buildcfg}") -- Included project name in the path
   objdir ("build/bin-int/%{prj.name}/%{cfg.buildcfg}") -- Included project name in the path

   -- Specify source files for the project
   files { 
      "MatrixEngine/src/**.h", 
      "MatrixEngine/src/**.cpp",
   }
