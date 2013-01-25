echo xVideoServiceThief VC++ automated compiler script
echo Note: Configure with your envirotment variables before execute this Bat
echo Copyright Xesc and Technolgy 2010

set QtVersion=4.6.2
set QtDir=C:\ProgramacioLibs\Qt\%QtVersion%-VC++
set VCDir=C:\Archivos de programa\Microsoft Visual Studio 9.0
set UPXApp=C:\Documents and Settings\Administrador\Escritorio\upx.exe
set MAKE=nmake

if not exist "%QtDir%" (
  echo ------------------------------------------------------------------
  echo FATAL ERROR: The Qt directory "%QtDir%" is missing...
  echo Please configure your "qt-vc++.bat"
  echo ------------------------------------------------------------------
  pause
  exit
)

if not exist "%VCDir%" (
  echo ------------------------------------------------------------------
  echo FATAL ERROR: The VC++ 2008 directory "%VCDir%" is missing...
  echo Please configure your "qt-vc++.bat"
  echo ------------------------------------------------------------------
  pause
  exit
)

if not exist "%UPXApp%" (
  echo ------------------------------------------------------------------
  echo WARNING: The UPX application is missing ["%UPXApp%"]
  echo Please configure your "qt-vc++.bat"
  echo ------------------------------------------------------------------
  pause
)

call "%VCDir%\VC\vcvarsall.bat" x86
