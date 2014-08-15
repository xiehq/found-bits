@echo off
echo Working in %CD%
set ERR=0

IF [%1] EQU [] (
echo Source path not defined
set ERR=-1
goto :done
)

  :: Specify source folders
set SRC_SLSOURCE=%1\simplelink\source\
set SRC_SLINCLUDE=%1\simplelink\include\

  :: Specify libs destination folder
set DEST_SLSOURCE=%~dp0source\
set DEST_SLINLUDE=%~dp0include\

  :: Remove all files
call :DELALL %DEST_SLSOURCE%
call :DELALL %DEST_SLINLUDE%
call :DELALL %~dp0

xcopy %SRC_SLSOURCE%*.* %DEST_SLSOURCE% /q /y /k /r
xcopy %SRC_SLINCLUDE%*.* %DEST_SLINLUDE% /q /y /k /r

goto :done

:DELALL %1
del %1*.c
del %1*.h
del %1*.s
)
goto :eof

:done
