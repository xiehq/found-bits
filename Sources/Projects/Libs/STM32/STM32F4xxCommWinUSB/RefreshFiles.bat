@echo off
echo Working in %CD%
set ERR=0

IF [%1] EQU [] (
echo Source path not defined
set ERR=-1
goto :done
)

  :: Specify source folders
set SRC_STLIB=%1\

  :: Specify libs destination folder
set DEST_EXTLIB=%~dp0ExtLib\

echo Destination path %DEST_EXTLIB%

echo Refreshing external files from %1

  :: Remove all files
del /S /F /Q %DEST_EXTLIB%*.*

echo - Refreshing USB library files:
set SRC_USB_DEV_LIB=%SRC_STLIB%Middlewares\ST\STM32_USB_Device_Library\
call :REFRESH %SRC_USB_DEV_LIB%Core\Inc\*.h
call :REFRESH %SRC_USB_DEV_LIB%Core\Src\*.c


del /S /F /Q %DEST_EXTLIB%*template*.*

goto :done

:REFRESH %1
xcopy %1 %DEST_EXTLIB% /q /y /k /r
IF NOT %ERRORLEVEL% EQU 0 (
echo ** ERROR **
set ERR=1
)
goto :eof

:done
pause

exit %ERR%
