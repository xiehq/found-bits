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

 :: pause

set SRC_CMSIS=%SRC_STLIB%Drivers\CMSIS\
set SRC_DEVICE_INC=%SRC_CMSIS%Device\ST\STM32F4xx\Include\
set SRC_DEVICE_SRC=%SRC_CMSIS%Device\ST\STM32F4xx\Source\Templates\

 :: echo - Refreshing core files:
 :: call :REFRESH %SRC_CMSIS%Include\core_cm4.h
 :: call :REFRESH %SRC_CMSIS%Include\core_cmInstr.h
 :: call :REFRESH %SRC_CMSIS%Include\core_cmFunc.h
 :: call :REFRESH %SRC_CMSIS%Include\core_cm4_simd.h

echo - Refreshing system files:
 :: call :REFRESH %SRC_DEVICE_INC%stm32f4xx.h
 :: call :REFRESH %SRC_DEVICE_INC%stm32f407xx.h
 :: call :REFRESH %SRC_DEVICE_INC%system_stm32f4xx.h
call :REFRESH %SRC_DEVICE_SRC%gcc\startup_stm32f407xx.s
call :REFRESH %SRC_DEVICE_SRC%system_stm32f4xx.c

echo - Refreshing USB library files:
set SRC_USB_DEV_LIB=%SRC_STLIB%Middlewares\ST\STM32_USB_Device_Library\
call :REFRESH %SRC_USB_DEV_LIB%Core\Inc\*.h
call :REFRESH %SRC_USB_DEV_LIB%Core\Src\*.c

 :: echo - Refreshing peripheral library files:

 :: call :REFRESH %SRC_STLIB%Drivers\STM32F4xx_HAL_Driver\Inc\*.*
 :: call :REFRESH %SRC_STLIB%Drivers\STM32F4xx_HAL_Driver\Src\*.*

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
