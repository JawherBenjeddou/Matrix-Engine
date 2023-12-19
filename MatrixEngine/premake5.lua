-- premake5.lua

-- project configurations
project "Engine"
   kind "SharedLib"  -- Shared library type
   language "C++"
   cppdialect "C++20"  -- C++ version Matrix uses
   targetdir ("../build/bin/%{prj.name}/%{cfg.buildcfg}") -- Included project name in the path TODO FIX THE OUTPUT DIR
   objdir ("../build/bin-int/%{prj.name}/%{cfg.buildcfg}") -- Included project name in the path 

   -- Specify source files for the project
   files { 
      "src/**.h", 
      "src/**.cpp",
   }

   configurations { "Debug", "Release" }
   platforms { "Win64" }

   -- Configure Release configuration
   filter "configurations:Release"
      defines {
         "_CRT_SECURE_NO_WARNINGS",
         "MX_RELEASE_MODE",
         "GLEW_STATIC",
         "MX_BUILD_DLL"
      }
      optimize "On"
      
      -- additional include directories 
      includedirs {
         "../Dependencies/entt/single_include/entt",
         "../Dependencies/GLFW/include",
         "../Dependencies/GLEW/include",
         "src/mx",
         "../Dependencies/glm",
         "src",
         "../Dependencies/spdlog/include",
         "../Dependencies/stb",
         "../Dependencies/imgui/backends",
         "../Dependencies/imgui"
      }

      -- Linker settings
      links {
         "glfw3.lib",
         "opengl32.lib",
         "glew32s.lib"
      }

      -- Library directories
      libdirs {
         "../Dependencies/GLFW/lib-vc2022",
         "../Dependencies/GLEW/lib",
      }

      -- precompiled header settings
      pchheader "pch.h"
      pchsource "src/pch.cpp"

   -- Configure Debug configuration
   filter "configurations:Debug"
      defines {
         "_CRT_SECURE_NO_WARNINGS",
         "MX_DEBUG_MODE",
         "GLEW_STATIC",
         "MX_BUILD_DLL"
      }
      symbols "On"

      includedirs {
         "../Dependencies/entt/single_include",
         "../Dependencies/GLFW/include",
         "../Dependencies/GLEW/include",
         "../MatrixEngine/src/mx",
         "../Dependencies/glm",
         "src",
         "../Dependencies/spdlog/include",
         "../Dependencies/stb",
         "../Dependencies/imgui/backends",
         "../Dependencies/imgui",
     }
     
      -- Linker settings
      links {
         "glfw3.lib",
         "opengl32.lib",
         "glew32s.lib"
      }

      -- Library directories
      libdirs {
         "$(SolutionDir)Dependencies/GLFW/lib-vc2022",
         "$(SolutionDir)Dependencies/GLEW/lib",
      }

      -- precompiled header settings for both configurations (outside of filter blocks)
      pchheader "pch.h"
      pchsource "src/pch.cpp"
