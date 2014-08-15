@echo off
echo Working in %CD%
set ERR=0

IF [%1] EQU [] (
echo Source path not defined
set ERR=-1
goto :done
)

  :: Specify source folders
set SRC_DRIVERLIB=%1\driverlib\
set SRC_INC=%1\inc\

  :: Specify libs destination folder
set DEST_DRIVERLIB=%~dp0
set DEST_INC=%~dp0\..\inc\

  :: Remove all files
call :DELALL %DEST_DRIVERLIB%
call :DELALL %DEST_INC%

xcopy %SRC_DRIVERLIB%*.* %DEST_DRIVERLIB% /q /y /k /r
xcopy %SRC_INC%*.* %DEST_INC% /q /y /k /r

goto :done

:DELALL %1
del %1*.c
del %1*.h
del %1*.s
)
goto :eof

:done
