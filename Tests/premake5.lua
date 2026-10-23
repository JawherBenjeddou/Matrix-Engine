-- Automated engine tests (no window or GPU required).
project "Tests"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"

   files { "src/**.h", "src/**.cpp" }
   includedirs { "src" }

   UseMatrixEngine()

   filter "action:vs*"
      disablewarnings { "4251" }
   filter {}
