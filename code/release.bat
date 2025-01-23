@echo off
set src_dir=src
set file_path=.\%src_dir%\version_num.h
set file_path2=.\%src_dir%\is_release.h

echo This will set the %src_dir% software release number in %file_path%

if not defined THORS_RELEASE_NUM goto ask

echo The current release number is: %THORS_RELEASE_NUM%

:ask
set /p release_num=What's the NEW software release number?

set /p ans=That's %release_num%, eh? (y/n)

if %ans% == n goto end
setx THORS_RELEASE_NUM %release_num%

echo // *** Warning! *** Do not edit this file. Use release.bat instead. > %file_path%
echo #ifndef VERSION_NUM_h >> %file_path%
echo #define VERSION_NUM_h >> %file_path%
echo. >> %file_path%
echo #define VERSION_NUM "v%release_num%" >> %file_path%
echo. >> %file_path%
echo #endif >> %file_path%

echo // *** Warning! *** Do not edit this file. release.bat sets value. > %file_path2%
echo #ifndef IS_RELEASE_h >> %file_path2%
echo #define IS_RELEASE_h >> %file_path2%
echo. >> %file_path2%
echo #define IS_RELEASE true >> %file_path2%
echo. >> %file_path2%
echo #endif >> %file_path2%

call make.bat Bonkulator
copy .\bonk_out\%src_dir%.ino.bin .\bonk_out\release
copy .\bonk_out\%src_dir%.ino.elf .\bonk_out\release
copy Blink.ino.elf.uf2 .\bonk_out\release
echo %release_num% > .\bonk_out\release\release.txt

call make.bat Thors Arsenal
copy .\thors_out\%src_dir%.ino.bin .\thors_out\release
copy .\thors_out\%src_dir%.ino.elf .\thors_out\release
copy Blink.ino.elf.uf2 .\thors_out\release
echo %release_num% > .\thors_out\release\release.txt

rem unset the define for normal compile
echo // *** Warning! *** Do not edit this file. release.bat sets value. > %file_path2%
echo #ifndef IS_RELEASE_h >> %file_path2%
echo #define IS_RELEASE_h >> %file_path2%
echo. >> %file_path2%
echo #define IS_RELEASE false >> %file_path2%
echo. >> %file_path2%
echo #endif >> %file_path2%

.\7z a -r .\releases\release_%release_num%.zip bonk_out\release -x!updater.sh
.\7z a -r .\releases\release_%release_num%.zip thors_out\release -x!updater.sh

REM Create a tar archive using 7-Zip for the Mac/Linux users
".\7z" a -ttar archive.tar ./bonk_out/release/* -x!update.bat
".\7z" a -ttar archive.tar ./thors_out/release/* -x!update.bat

REM Compress the tar archive into .gz format using 7-Zip
".\7z" a -tgzip .\archive.tar.gz ./archive.tar
".\7z" a .\releases\release_%release_num%.tar.zip ./archive.tar.gz

REM Delete the intermediate .tar file
del archive.tar
del archive.tar.gz
:end
echo Goodbye!
REM Make the computer beep using PowerShell
powershell -command "[console]::beep()"
powershell -command "[console]::beep()"
pause

