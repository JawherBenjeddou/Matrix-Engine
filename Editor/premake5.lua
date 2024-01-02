-- premake5.lua

-- project configurations
project "Editor"
   kind "ConsoleApp"  -- ConsoleApp type
   language "C++"
   cppdialect "C++20"  -- C++ version Editor uses
   targetdir ("%{wks.location}/build/bin/%{prj.name}/%{cfg.buildcfg}") -- Included project name in the path 
   objdir ("%{wks.location}/build/bin-int/%{prj.name}/%{cfg.buildcfg}") -- Included project name in the path 
   dependson { "Matrix" }
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
      "%{wks.location}/MatrixEngine/src/mx",
      "%{wks.location}/MatrixEngine/src",
      "%{wks.location}/Dependencies/glm",
      "%{wks.location}/Dependencies/spdlog/include",
      "%{wks.location}/Dependencies/stb",
      "%{wks.location}/Dependencies/imgui/backends",
      "%{wks.location}/Dependencies/imgui"
   }
      -- Linker settings
      links {
         "Matrix.lib",
         "glfw3.lib",
         "opengl32.lib",
         "glew32s.lib"
      }

      -- Library directories
      libdirs {
         "%{wks.location}/Dependencies/GLFW/lib-vc2022",
         "%{wks.location}/Dependencies/GLEW/lib",
         "%{wks.location}/build/bin/Matrix/Release"
      }

   -- Debug configuration
   filter "configurations:Debug"
      defines {
         "_CRT_SECURE_NO_WARNINGS",
         "MX_DEBUG_MODE",
         "GLEW_STATIC",
         "MX_BUILD_DLL"
      }
      symbols "On"
      includedirs {
         "%{wks.location}/Dependencies/entt/single_include/entt",
         "%{wks.location}/Dependencies/GLFW/include",
         "%{wks.location}/Dependencies/GLEW/include",
         "%{wks.location}/MatrixEngine/src/mx",
         "%{wks.location}/MatrixEngine/src",
         "%{wks.location}/Dependencies/glm",
         "%{wks.location}/Dependencies/spdlog/include",
         "%{wks.location}/Dependencies/stb",
         "%{wks.location}/Dependencies/imgui/backends",
         "%{wks.location}/Dependencies/imgui"
     }
     
      -- Linker settings
      links {
         "Matrix.lib",
         "glfw3.lib",
         "opengl32.lib",
         "glew32s.lib"
      }

      -- Library directories
      libdirs {
         "%{wks.location}/Dependencies/GLFW/lib-vc2022",
         "%{wks.location}/Dependencies/GLEW/lib",
         "%{wks.location}/build/bin/Matrix/Debug"
      }

