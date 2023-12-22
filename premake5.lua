-- premake5.lua (in the root directory)
-- premake5.lua

workspace "Matrix"
   configurations { "Debug", "Release" }
   startproject "Editor"
   platforms { "Win64" }

      -- Configure platform-specific settings
      filter { "platforms:Win64" }
      system "Windows"
      architecture "x86_64"

      group "Engine"
      include "MatrixEngine/premake5.lua"
      include "Editor/premake5.lua"  -- Including the Editor project in the same group
   
    group "Games"
        files { } -- Empty table to create an empty group

    filter "configurations:Debug"
        postbuildcommands {
            '{COPY} "%{wks.location}/build/bin/Matrix/%{cfg.buildcfg}/Matrix.dll" "%{wks.location}/build/bin/Editor/%{cfg.buildcfg}"'
        }
    
    filter "configurations:Release"
        postbuildcommands {
            '{COPY} "%{wks.location}/build/bin/Matrix/%{cfg.buildcfg}/Matrix.dll" "%{wks.location}/build/bin/Editor/%{cfg.buildcfg}"'
        }
    