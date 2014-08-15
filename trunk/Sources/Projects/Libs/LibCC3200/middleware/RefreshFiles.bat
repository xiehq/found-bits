rem @echo off
echo Working in %CD%
set ERR=0

IF [%1] EQU [] (
echo Source path not defined
set ERR=-1
goto :done
)

  :: Specify source folders
set SRC_DRIVER=%1\middleware\driver\
set SRC_FRAMWORK=%1\middleware\framework\
set SRC_SOC=%1\middleware\soc\

  :: Specify libs destination folder
set DEST_DRIVER=%~dp0driver\
set DEST_FRAMWORK=%~dp0framework\
set DEST_SOC=%~dp0soc\

  :: Remove all files
call :DELALL %DEST_DRIVER%
call :DELALL %DEST_FRAMWORK%
call :DELALL %DEST_SOC%
pause
xcopy %SRC_DRIVER%*.* %DEST_DRIVER% /q /y /k /r /s
xcopy %SRC_FRAMWORK%*.* %DEST_FRAMWORK% /q /y /k /r /s
xcopy %SRC_SOC%*.* %DEST_SOC% /q /y /k /r /s

goto :done

:DELALL %1
del /s /p /q %1*.*
rd /s /q %1
)
goto :eof

:done
