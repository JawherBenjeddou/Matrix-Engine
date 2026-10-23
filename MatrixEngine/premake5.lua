-- Matrix engine runtime library (Matrix.dll)
project "Matrix"
   kind "SharedLib"
   language "C++"
   cppdialect "C++20"

   pchheader "pch.h"
   pchsource "src/mx/pch.cpp"

   files {
      "src/**.h",
      "src/**.cpp",
      -- Dear ImGui is compiled straight from the dependency checkout.
      "%{wks.location}/Dependencies/imgui/imgui.cpp",
      "%{wks.location}/Dependencies/imgui/imgui_draw.cpp",
      "%{wks.location}/Dependencies/imgui/imgui_tables.cpp",
      "%{wks.location}/Dependencies/imgui/imgui_widgets.cpp",
      "%{wks.location}/Dependencies/imgui/imgui_demo.cpp",
      "%{wks.location}/Dependencies/imgui/backends/imgui_impl_glfw.cpp",
      "%{wks.location}/Dependencies/imgui/backends/imgui_impl_opengl3.cpp",
   }

   includedirs {
      "src",
      "src/mx",
      "%{IncludeDir.GLFW}",
      "%{IncludeDir.GLEW}",
      "%{IncludeDir.glm}",
      "%{IncludeDir.entt}",
      "%{IncludeDir.spdlog}",
      "%{IncludeDir.stb}",
      "%{IncludeDir.imgui}",
      "%{IncludeDir.imgui_backends}",
      "%{IncludeDir.json}",
      "%{IncludeDir.miniaudio}",
   }

   defines {
      "MX_BUILD_DLL",
      "GLEW_STATIC",
      "GLFW_INCLUDE_NONE",
      "GLM_ENABLE_EXPERIMENTAL",
      "IMGUI_API=__declspec(dllexport)",
   }

   libdirs { "%{LibraryDir.GLFW}", "%{LibraryDir.GLEW}" }
   links { "glfw3.lib", "opengl32.lib", "glew32s.lib", "comdlg32.lib" }

   -- The prebuilt GLFW/GLEW libraries were built against the release CRT; ignore their default-lib hints.
   filter "configurations:Debug"
      linkoptions { "/NODEFAULTLIB:LIBCMT", "/NODEFAULTLIB:MSVCRT", "/ignore:4099" }
   filter "configurations:Release"
      linkoptions { "/NODEFAULTLIB:LIBCMT", "/ignore:4099" }
   filter {}

   -- Third-party translation units and single-header implementations do not use the engine PCH.
   filter "files:**/Dependencies/**.cpp or files:**/thirdparty/**.cpp"
      enablepch "Off"
   filter {}

   -- MSVC: the engine exports classes containing STL members (C4251) which is intentional.
   filter "action:vs*"
      disablewarnings { "4251" }
   filter {}
