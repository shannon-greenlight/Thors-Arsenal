@echo off
echo.
echo.
@REM echo Available COM ports
@REM echo -------------------
powershell -NoProfile -ExecutionPolicy Bypass -File ".\list_ports.ps1" %1
rem powershell -Command "$boardList = .\arduino-cli board list; $boardList -split \"`n\" | ForEach-Object { if ($_ -match '^(COM\d+)\s') { $port = $Matches[1]; if ($_ -match 'Greenface RP2040') { Write-Host \"Thor's Arsenal found on $port\" } elseif ($_ -match 'Arduino Nano RP2040 Connect') { Write-Host \"Bonkulator found on $port\" } elseif ($_ -match 'Arduino NANO 33 IoT') { Write-Host \"Spankulator found on $port\" } } }"
rem powershell.exe [System.IO.Ports.SerialPort]::getportnames()
echo.
echo.
SetLocal EnableDelayedExpansion

set out_dir=
for /F "delims=" %%i in (out_dir.txt) do set out_dir=%out_dir%%%i
call :TRIM %out_dir%
set out_dir=%TRIMRESULT%
echo Loading from: %out_dir%

set port=
for /F "delims=" %%i in (spank_port.txt) do set port=%port%%%i
call :TRIM %port%
set port=%TRIMRESULT%

echo Using: COM%port%
set /p port=What's the COM port? (please enter a number or press ENTER to use COM%port%) 'x' to exit 

if %port% == x exit

echo %port% > spank_port.txt

arduino-cli upload -p COM%port% -b arduino:mbed_nano:nanorp2040connect -i %out_dir%/src.ino.bin
pause
EndLocal

:TRIM
  rem echo "%1"
  rem echo "%2"
  rem echo "%3"
  rem echo "%4"
  SET TRIMRESULT=%*
GOTO :EOF
pause
