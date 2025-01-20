@echo off
echo.
echo.
echo Update Thor's Arsenal software to:
type release.txt
echo.

.\arduino-cli core install arduino:mbed_nano

REM Get a list of connected boards
powershell -NoProfile -ExecutionPolicy Bypass -File ".\list_ports.ps1" %1
rem powershell -Command "$boardList = .\arduino-cli board list; $boardList -split \"`n\" | ForEach-Object { if ($_ -match '^(COM\d+)\s') { $port = $Matches[1]; if ($_ -match 'Greenface RP2040') { Write-Host \"Thor's Arsenal found on $port\" } elseif ($_ -match 'Arduino Nano RP2040 Connect') { Write-Host \"Bonkulator found on $port\" } elseif ($_ -match 'Arduino NANO 33 IoT') { Write-Host \"Spankulator found on $port\" } } }"

echo.
set /p port=What's the COM port? (please enter a number) 'x' to exit 

if %port% == x exit

arduino-cli upload -p COM%port% -b arduino:mbed_nano:nanorp2040connect -i ./src.ino.bin
pause
