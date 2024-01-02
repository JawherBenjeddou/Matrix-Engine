-- premake5.lua

-- project configurations
project "Matrix"
   kind "SharedLib"  -- Shared library type
   language "C++"
   cppdialect "C++20"  -- C++ version Engine uses
   targetdir ("../build/bin/%{prj.name}/%{cfg.buildcfg}") -- Included project name in the path 
   objdir ("../build/bin-int/%{prj.name}/%{cfg.buildcfg}") -- Included project name in the path 

   -- Specify source files for the project
   files { 
      "src/**.h", 
      "src/**.cpp",
   }

   configurations { "Debug", "Release" }
   platforms { "Win64" }

   -- Release configuration
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
         "%{wks.location}/Dependencies/entt/single_include/entt",
         "%{wks.location}/Dependencies/GLFW/include",
         "%{wks.location}/Dependencies/GLEW/include",
         "src/mx",
         "%{wks.location}/Dependencies/glm",
         "src",
         "%{wks.location}/Dependencies/spdlog/include",
         "%{wks.location}/Dependencies/stb",
         "%{wks.location}/Dependencies/imgui/backends",
         "%{wks.location}/Dependencies/imgui"
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

   -- Debug configuration
   filter "configurations:Debug"
      defines {
         "_CRT_SECURE_NO_WARNINGS",
         "MX_DEBUG_MODE",
         "GLEW_STATIC",
         "MX_BUILD_DLL"
      }
      symbols "On"

    -- additional include directories 
    includedirs {
      "%{wks.location}/Dependencies/entt/single_include/entt",
      "%{wks.location}/Dependencies/GLFW/include",
      "%{wks.location}/Dependencies/GLEW/include",
      "src/mx",
      "%{wks.location}/Dependencies/glm",
      "src",
      "%{wks.location}/Dependencies/spdlog/include",
      "%{wks.location}/Dependencies/stb",
      "%{wks.location}/Dependencies/imgui/backends",
      "%{wks.location}/Dependencies/imgui"
   }
   
     
      -- Linker settings
      links {
         "glfw3.lib",
         "opengl32.lib",
         "glew32s.lib"
      }

      -- Library directories
      libdirs {
         "%{wks.location}/Dependencies/GLFW/lib-vc2022",
         "%{wks.location}/Dependencies/GLEW/lib",
      }

      -- precompiled header settings for both configurations (outside of filter blocks)
      pchheader "pch.h"
      pchsource "src/pch.cpp"
