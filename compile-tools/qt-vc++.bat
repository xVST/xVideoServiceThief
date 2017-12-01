echo xVideoServiceThief VC++ automated compiler script
echo Note: Configure with your envirotment variables before execute this Bat
echo Copyright Xesc and Technolgy 2010-2017

set QtVersion=5.0.2
set QtDir=C:\ProgramacioLibs\Qt\Qt%QtVersion%-vc2010\qtbase
set VCDir=C:\Archivos de programa\Microsoft Visual Studio 10.0
set UPXApp=C:\Documents and Settings\Administrador\Escritorio\upx.exe
set MAKE=nmake

set LIB=%LIB%;C:\ProgramacioLibs\OpenSSL-Win32\lib\VC\static
set INCLUDE=%INCLUDE%;C:\ProgramacioLibs\OpenSSL-Win32\include
set OPENSSL_LIBS="-lssleay32 -llibeay32"

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
