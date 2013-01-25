@echo off

call qt-vc++.bat

:init
cd %CD%/..

:create-project
"%QtDir%\bin\qmake" -set build_mode static_build
"%QtDir%\bin\qmake" -spec win32-msvc2008 -tp vc