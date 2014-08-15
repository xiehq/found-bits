@echo off
echo Working in %CD%
set ERR=0

IF [%1] EQU [] (
echo Source path not defined
set ERR=-1
goto :done
)

  :: Specify source folders
set SRC_OSLIB=%1\oslib\

  :: Specify libs destination folder
set DEST_OSLIB=%~dp0

  :: Remove all files
call :DELALL %DEST_OSLIB%

xcopy %SRC_OSLIB%*.* %DEST_OSLIB% /q /y /k /r

goto :done

:DELALL %1
del %1*.c
del %1*.h
del %1*.s
)
goto :eof

:done
