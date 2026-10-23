-- Matrix Editor: the visual scene editor built on top of the engine.
project "Editor"
   language "C++"
   cppdialect "C++20"
   targetname "MatrixEditor"

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
