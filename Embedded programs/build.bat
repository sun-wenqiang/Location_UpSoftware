@ECHO OFF
SETLOCAL
SET UV=D:\Keil\install\UV4\UV4.exe
SET PROJ=".\Project\RVMDK(uv5)\xs_rec_net.uvprojx"
SET LOGFILE=build_app.log
SET EXTRACMD=
ECHO Start building ...
ECHO .>%LOGFILE%
START /WAIT %UV% -j0 -r %PROJ% -o "%cd%\%LOGFILE%" %EXTRACMD%
IF ERRORLEVEL 2 ECHO ERROR.
IF ERRORLEVEL 0 ECHO OK.
TYPE %LOGFILE%
COPY .\Output\*.axf D:\lfxs\XS_STM32_Firmware\xs_rec
ECHO ========Done========