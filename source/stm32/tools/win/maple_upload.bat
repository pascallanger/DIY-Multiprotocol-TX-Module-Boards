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

REM Delay to wait for the board to reset
REM Needed to prevent the Arduino IDE Serial Monitor from reconnecting before board has reset
for /l %%x in (1, 1, 40) do (
  ping -w 50 -n 1 192.0.2.1 > nul
  mode %1 > nul
  if ERRORLEVEL 0 (
    ECHO.
    ECHO Done.
    ECHO.
    GOTO :EOF
  )
)
ECHO.
ECHO Timeout waiting for %1
ECHO.
