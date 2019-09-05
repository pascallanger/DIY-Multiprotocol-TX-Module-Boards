@echo off
rem Note %~dp0 get path of this batch file
rem Need to change drive if My Documents is on a drive other than C:
set driverLetter=%~dp0
set driverLetter=%driverLetter:~0,2%
%driverLetter%
cd %~dp0

rem The lines below are needed to fix path issues with incorrect slashes before the bin file name
set fwpath=%3
set fwpath=%fwpath:/=\%

rem Erase the flash up to the EEPROM page
echo stm32flash -o -S 0x8000000:129024 -b 115200 %1
stm32flash -o -S 0x8000000:129024 -b 115200 %1
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

rem Write the firmware without erasing any blocks
echo stm32flash -v -g %2 -e 0 -b 115200 -w %fwpath% %1 
echo.
stm32flash -v -g %2 -e 0 -b 115200 -w %fwpath% %1 
