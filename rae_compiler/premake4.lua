#!lua

solution "rae_compiler"
   configurations { "Debug", "Release" }
   platforms {"native", "x64", "x32"}

   project "raec"
      kind "ConsoleApp"
      language "C++"
      targetdir "./"
      files { "./src/*.hpp", "./src/*.cpp" }
      includedirs { "src" }

      configuration { "windows" }
         defines { "_CRT_SECURE_NO_DEPRECATE" } -- to use fopen on windows without warnings.
      
      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
         debugdir "./"

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }
         debugdir "./"

