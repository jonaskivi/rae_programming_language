#!lua

-- A solution contains projects, and defines the available configurations
solution "rae_examples"
   configurations { "Debug", "Release" }
   platforms {"native", "x64", "x32"}

   -- A project defines one build target
   project "hello"
      kind "ConsoleApp"
      language "C++"
      targetdir "../bin/"
      files { "../cpp/examples/hello.hpp", "../cpp/examples/hello.cpp", "../cpp/rae/**.hpp", "../cpp/rae/**.cpp" }
      includedirs { "../cpp", "../cpp/rae" }
      
      configuration { "windows" }
         defines { "_CRT_SECURE_NO_DEPRECATE" } -- to use fopen on windows without warnings.

      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
         debugdir "../bin/"

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }
         debugdir "../bin/"



