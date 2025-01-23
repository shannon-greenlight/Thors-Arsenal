# Thor's Arsenal

Greenface Labs' **Thor's Arsenal** is a Eurorack module that fits into a 3U x 14HP slot. It features 8 programmable outputs and 6 programmable inputs. The front-panel user controls consist of a 128×64 pixel 2-color OLED display, a rotary encoder for adjusting parameters and a button for each trigger and output.

Page: https://www.greenfacelabs.com/thors-arsenal/

# main_board, panel

These repos contain the materials needed to construct the Thor's Arsenal main board and the front panel.
The files are for KiCAD 8.0. Go to the Greenlight KiCAD repo to get the required libraries.

Assembly instructions are in main_board\fab\thors_assembly.docx.

When using the JLCPCB assembly service, the BOM is split into 4 parts.
Use the KiCAD command "generate legacy bill of materials", then the batch file "generate_boms.bat" to update these files if changes to the BOM occur.

- bom/lcsc/main_board_bom.csv : The parts to be assembled by JLCPCB
- bom/digi-key/main_board_split.csv : The remaining parts to be assembled by user
- bom/digi-key/main_board_full.csv : The parts to be assembled by user if starting from a bare board.
- bom/other/main_board_other.csv : The parts that must be bought from sources other than JLCPCB and Digi-Key

The gerber files work with JLCPC and will most likely work with other board manufacturers. JLCPCB requires these files:

- fab/thors_arsenal_gerbers.zip (needed for bare boards and assembly)
- fab/thors_arsenal_cpl.xlsx (component position file required for assembly)
- bom/main_board_bom.csv (bom required for assembly)

notes: thors_arsenal_cpl.xlsx is auto-generated from the XML BOM file produced by KiCAD using "legacy bill of materials". You will need to refresh the data when changes to the schematic that affect the BOM or changes to the board layout occur.

If changes to the layout occur, use the batch file "/fab/zip_berbers.bat" to package the gerber files for uploading to JLCPCB.

# code

This repository contains the elements needed to build code for Thor's Arsenal.

Use make.bat or make.sh to compile the code. You'll need to install the arduino IDE to get the base code installed on your computer. Install the board arduino:mbed_nano:nanorp2040connect

Thor's Arsenal uses arduino-cli.exe to compile.
You'll need to make sure the arduino-cli config is correct. In particular you need to set the directories.data and directitories.user variables in the configuration.
The data dir points to the AppData folder that was just installed by the Arduino IDE.
The user dir is your project folder.
Here is an example from a config dump:
directories:
data: C:\Users\shann\AppData\Local\Arduino15
user: C:\Users\shann\Documents\Eurorack\Thors_Arsenal\code\src

see https://arduino.github.io/arduino-cli/1.1/ for more info.

The code supports both Thor's Arsenal and the Bonkulator. Also several portions are shared with Fafnir's Fire. You'll see this in #ifdef statements found in the code. Because Thor's Arsenal and the discontinued Bonkulator are so similar, they share the same code base. This is the reason for the two "out" directories.

## Directory Structure

- **bonk_out** contains output of latest Bonkulator compile
- **bonk_out/_release** contains latest Bonkulator release
- **thors_out** contains output of latest Thor's Arsenal compile
- **thors_out/_release** contains latest Thor's Arsenal release
- **releases** contains all of the software releases in zipped form.
- **src** contains the source code.
