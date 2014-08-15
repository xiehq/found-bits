rem @echo off
echo Working in %CD%
set ERR=0

IF [%1] EQU [] (
echo Source path not defined
set ERR=-1
goto :done
)

  :: Specify source folders
set SRC_SOURCE=%1\simplelink\source\
set SRC_INCLUDE=%1\simplelink\include\
set SRC_ROOT=%1\simplelink\

  :: Specify libs destination folder
set DEST_SOURCE=%~dp0source\
set DEST_INLUDE=%~dp0include\
set DEST_ROOT=%~dp0

  :: Remove all files
call :DELALL %DEST_SOURCE%
call :DELALL %DEST_INLUDE%
call :DELALL %DEST_ROOT%

xcopy %SRC_SOURCE%*.* %DEST_SOURCE% /q /y /k /r
xcopy %SRC_INCLUDE%*.* %DEST_INLUDE% /q /y /k /r
xcopy %SRC_ROOT%*.* %DEST_ROOT% /q /y /k /r

goto :done

:DELALL %1
del %1*.c
del %1*.h
del %1*.s
)
goto :eof

:done
