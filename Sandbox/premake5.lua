-- Sandbox: a minimal runtime application that loads and plays a scene without the editor.
project "Sandbox"
   language "C++"
   cppdialect "C++20"

   files { "src/**.h", "src/**.cpp" }
   includedirs { "src" }

   UseMatrixEngine()

   filter "configurations:Debug"
      kind "ConsoleApp"
   filter "configurations:Release"
      kind "WindowedApp"
      entrypoint "mainCRTStartup"
   filter "action:vs*"
      disablewarnings { "4251" }
   filter {}
