@echo off
REM Script to automatically select upload method for stm32-based multi-protocol module.

set driverLetter=%~dp0
set driverLetter=%driverLetter:~0,2%
%driverLetter%
cd %~dp0

set comport=%1

set fwpath=%2
set fwpath=%fwpath:/=\%

set blpath=%3
set blpath=%blpath:/=\%

:MAPLE_CHECK
REM Look for a Maple USB device
ECHO.
ECHO Looking for Maple device ...
maple_find.exe
ECHO.

if %errorlevel% equ 0 (
    GOTO USB_FLASH
) else (
    GOTO FTDI_CHECK
)

:USB_FLASH
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

GOTO :EOF

:FTDI_CHECK
REM Attempt to read from the STM module via the specified serial port
ECHO Probing serial port %comport% for STM32 in BOOT0 mode ...
stm32flash.exe -b 115200 %comport%

if %errorlevel% equ 0 (
	ECHO Found module on %comport%
	GOTO FTDI_FLASH
) ELSE (
  ECHO Module in BOOT0 mode not found on %comport%
  GOTO :EOF
)

:FTDI_FLASH
ECHO.
ECHO Flashing module via FTDI adapter on %comport%
ECHO.
ECHO Erasing ...
ECHO stm32flash.exe -o -S 0x8000000:129024 -b 115200 %comport%
stm32flash.exe -o -S 0x8000000:129024 -b 115200 %comport%

ECHO Writing bootloader ...
ECHO stm32flash.exe -v -e 0 -g 0x8000000 -b 115200 -w %blpath% %comport%
stm32flash.exe -v -e 0 -g 0x8000000 -b 115200 -w %blpath% %comport%

ECHO Writing Multi firmware ...
ECHO stm32flash.exe -v -s 8 -e 0 -g 0x8002000 -b 115200 -w %fwpath% %comport%
stm32flash.exe -v -s 8 -e 0 -g 0x8002000 -b 115200 -w %fwpath% %comport%
ECHO.
ECHO Done.
GOTO :EOF
