@echo off
set zipper=.\code\7z

if not defined THORS_HARDWARE_RELEASE_NUM goto ask

echo The current release number is: %THORS_HARDWARE_RELEASE_NUM%

:ask
set /p release_num=What's the NEW release number? 

set /p ans=That's %release_num%, eh? (y/n)

if %ans% == n goto end
setx THORS_HARDWARE_RELEASE_NUM %release_num%

set zipfile=.\hardware_releases\release_%release_num%.zip

%zipper% a -r %zipfile% main_board\fab\thors_arsenal_cpl.xlsx main_board\fab\thors_arsenal_gerbers.zip main_board\fab\thors_assembly.docx main_board\bom\*.*
%zipper% a -r %zipfile% main_board\main_board.pdf panel\fab\thors_arsenal_panel_gerbers.zip

:end
echo Goodbye!
REM Make the computer beep using PowerShell
powershell -command "[console]::beep()"
powershell -command "[console]::beep()"
pause

