@echo off

:init
cd %CD%/..

:qt-config
if "%1" == "vc++" (
  call compile-tools\qt-vc++.bat
  goto :pre-clean
)
if "%1" == "mingw" (
  call compile-tools\qt-mingw.bat
  goto :pre-clean
)

:config-missing
echo ------------------------------------------------------------------
echo FATAL ERROR: No configuration (VC++ or MinGW) has been specified
echo Example: "compile-tools/compile.bat" vc++ clean
echo ------------------------------------------------------------------
goto :END

:pre-clean
if "%2" == "no-clean" goto :do-qmake

:Clean
RMDIR build /s /q
%MAKE% clean 
DEL Makefile
DEL Makefile.*
DEL object_script.xVideoServiceThief.*

:do-qmake
goto :do-qmake-%1%

:do-qmake-vc++
"%QtDir%\bin\qmake" -set build_mode static_build
"%QtDir%\bin\qmake" -spec win32-msvc2008 CONFIG+=release
goto :compile

:do-qmake-mingw
"%QtDir%\bin\qmake" -set build_mode dynamic_build
"%QtDir%\bin\qmake" CONFIG+=release
goto :compile

:compile
"%MAKE%"

:upx
"%UPXApp%" "bin\xVideoServiceThief.exe" -9

:Install languages
%QtDir%\bin\lrelease.exe xVideoServiceThief.pro
call ".\Installer\windows-install-languages.bat" "%CD%\Installer\"                                                                               

:Install plugins
call ".\Installer\windows-install-plugins.bat" "%CD%\Installer\"                                                                  

:END
pause