@echo off

echo Working in %CD%
  :: cmd /c .\..\..\PostBuildGCC.bat ${CompilerToolsPath} ${CWD} ${BuildArtifactFileBaseName} ${BuildArtifactFileExt}
  :: set CMP_DIR=%1
  :: set OUT_DIR=%2
  :: set OUT_NAME=%3
  :: set OUT_EXT=%4

  :: set OUT_PATH=%OUT_DIR%\%OUT_NAME%.%OUT_EXT%

  :: echo %CMP_DIR%
  :: echo %OUT_DIR%
  :: echo %OUT_NAME%
  :: echo %OUT_EXT%
  :: echo %OUT_PATH%

  :: exit

set SCRIPT_DIR=%~dp0
set CMP_DIR=\Tools\Compilers\arm-4_8-2014-q1\bin\
set OUT_DIR=%SCRIPT_DIR%eclipse\Debug\
set OUT_NAME=CC3200Burn
set OUT_EXT=out

set OUT_PATH=%OUT_DIR%%OUT_NAME%.%OUT_EXT%

set OUT_BIN=%OUT_DIR%%OUT_NAME%.bin

set OBJCOPY_PATH=%CMP_DIR%arm-none-eabi-objcopy.exe

echo %OBJCOPY_PATH% -O binary %OUT_PATH% %OUT_BIN%
call %OBJCOPY_PATH% -O binary %OUT_PATH% %OUT_BIN%

rem pause