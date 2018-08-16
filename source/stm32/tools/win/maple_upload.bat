@echo off
rem: Note %~dp0 get path of this batch file
rem: Need to change drive if My Documents is on a drive other than C:
set driverLetter=%~dp0
set driverLetter=%driverLetter:~0,2%
%driverLetter%
cd %~dp0
java -jar maple_loader.jar %1 %2 %3 %4 %5 %6 %7 %8 %9

for /l %%x in (1, 1, 40) do (
  ping -w 50 -n 1 192.0.2.1 > nul
  mode %1 > nul
  if ERRORLEVEL 0 goto comPortFound
)

echo timeout waiting for %1 serial

:comPortFound
