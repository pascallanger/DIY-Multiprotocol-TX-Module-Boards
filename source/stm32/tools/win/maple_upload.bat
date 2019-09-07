@echo off
rem: Note %~dp0 get path of this batch file
rem: Need to change drive if My Documents is on a drive other than C:
set driverLetter=%~dp0
set driverLetter=%driverLetter:~0,2%
%driverLetter%
cd %~dp0

set comport=%1
set fwpath=%4
set fwpath=%fwpath:/=\%

ECHO Attempting to flash module via Maple USB ...
ECHO maple-reset.exe %comport% 2000
maple-reset.exe %comport% 2000
ECHO.
ECHO dfu-util.exe -a 2 -d 1eaf:0003 -D "%fwpath%" -R
dfu-util.exe -a 2 -d 1eaf:0003 -D "%fwpath%" -R
