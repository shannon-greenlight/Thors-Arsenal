#include <Arduino.h>
String settings_text[SETTINGS_NUM_PARAMS] = {};
void settings_text_begin()
{
    settings_text[SETTINGS_VER] = "<div><h2>{param_label}<span>{param_as_string}</span></h2></div>";
    settings_text[SETTINGS_VER] += "This is the Software Version. It cannot be changed.";

    settings_text[SETTINGS_NAME] = "<div><span class='device_name'>{param_as_string}</span> is the name of your<div class='product_name'>" + String(PRODUCT_NAME) + "</div></div>";
    settings_text[SETTINGS_NAME] += "You can change the name to your liking. Just double-click to edit";

    settings_text[SETTINGS_ENCODER] = "<div><h2>{param_label_raw}</h2></div>";
    settings_text[SETTINGS_ENCODER] += "<div>Change this setting if the digits go the wrong way when adjusting parameters using the front panel encoder dial.</div>";

#if defined(THORS_ARSENAL) || defined(FAFNIRS_FIRE)
    settings_text[SETTINGS_WIFI] = "<div>WiFi is not available on this device.</div>";
#elif defined(BONKULATOR)
    settings_text[SETTINGS_WIFI] = "<div><h2>{param_label}<span>{param_as_string}</span></h2></div>";
    settings_text[SETTINGS_WIFI] += "<div>";
    settings_text[SETTINGS_WIFI] += "Select RESET to disable WiFi or select Enable to enable WiFi, then click ACTIVATE.";
    settings_text[SETTINGS_WIFI] += " Note: It is not recommended to have WiFi and USB Direct enabled at the same time.";
    settings_text[SETTINGS_WIFI] += "</div>";
#endif

    settings_text[SETTINGS_USB_DIRECT] = "<div><h2>{param_label_raw}</h2></div>";
    settings_text[SETTINGS_USB_DIRECT] += "<div>USB Direct is enabled to communicate with this Control Panel app. Only disable it if you want to use another control method.</div>";

    settings_text[SETTINGS_SCREEN] = "<div><h2>{param_label}<span>{param_as_string}</span></h2></div>";
    settings_text[SETTINGS_SCREEN] += "<div>OLED screens degrade over time when on. Your device will turn off the screen to minimize this effect. Screen timeout is set to {param_as_string} minutes.</div>";

    settings_text[SETTINGS_USER_WAVEFORMS] = "<div><h2>{param_label_raw}</h2></div>";
    settings_text[SETTINGS_USER_WAVEFORMS] += "<div>These are the user waveforms you can use in your outputs. You can change these to your liking. Select which waveform you want, then click ACTIVATE to edit.</div>";

    settings_text[SETTINGS_RESET] = "<div><h2>{param_label_raw}</h2></div>";
    settings_text[SETTINGS_RESET] += "<div>Select the items you want to reset, then Click ACTIVATE to enter the RESET menu.</div>";
    settings_text[SETTINGS_RESET] += "<div>Selecting ALL will clear all settings, calibration corrections, and user waveforms.</div>";
    settings_text[SETTINGS_RESET] += "<div>Selecting PARAMETERS will clear all settings and user waveforms.</div>";
    settings_text[SETTINGS_RESET] += "<div>Selecting INPUTS will clear all input calibrations.</div>";
#ifdef BONKULATOR
    settings_text[SETTINGS_RESET] += "<div>Selecting OUTPUTS will clear all output calibrations.</div>";
#endif

    settings_text[SETTINGS_CAL] += "<div><h2>{param_label_raw}</h2></div>";
#if defined(THORS_ARSENAL) || defined(FAFNIRS_FIRE)
    settings_text[SETTINGS_CAL] += "<div>Click ACTIVATE to calibrate the CV inputs. You'll need a voltage source that's accurate to 10mVDC or better.</div>";
    settings_text[SETTINGS_CAL] += "<div>Or";
#endif
#ifdef BONKULATOR
    settings_text[SETTINGS_CAL] += "<div>Select Inputs or Outputs, then Click ACTIVATE to calibrate. For calibrating the CV inputs, you'll need a voltage source that's accurate to 10mVDC or better. For the outputs, you'll need a DVM with an accuracy of 1mVDC or better.</div>";
    settings_text[SETTINGS_CAL] += "<div>Or";
#endif
    settings_text[SETTINGS_CAL] += " you can use one of the outputs. They are very accurate. Just set the Idle Value and connect the output to the input.</div>";
    settings_text[SETTINGS_CAL] += "<div>For the zero reference, just unplug the patch cord from the input. This grounds the input.</div>";

    settings_text[SETTINGS_BOARD_GENERATION] = "<div><h2>{param_label}<span>{param_as_string}</span></h2></div>";
    settings_text[SETTINGS_BOARD_GENERATION] += "<div>This is the board revision. It allows the software to handle hardware differences from revision to revision. Set this to the board revision found on the bottom of the circuit board.</div>";

    settings_text[SETTINGS_BOUNCE] = "<div><h2>{param_label_raw}</h2></div>";
    settings_text[SETTINGS_BOUNCE] += "<div>The bounce function is the combination of a DVM and a scalable sample and hold. It will \"bounce\" the selected input to any of the 8 outputs.</div>";
    settings_text[SETTINGS_BOUNCE] += "<div>Select which input you want, then click ACTIVATE to enter the Bounce menu.</div>";
    settings_text[SETTINGS_BOUNCE] += "<div>CV {param_as_string} is the currently selected input.</div>";
}

String ask_init_text[1] = {};
String ask_init_options[RESET_MAX + 1] = {};

void ask_init_begin()
{
    ask_init_options[INIT_PARAMETERS] = "<div>This will reset all parameters to their default values.<p><strong>Press Activate to confirm Reset<strong></p></div>";
    ask_init_options[INIT_USER_WAVEFORMS] = "<div>This will reset all user waveforms to flat lines and reset their names to the default values.<p><strong>Press Activate to confirm Reset<strong></p></div>";
    ask_init_options[INIT_INPUTS] = "<div>This will reset all input calibrations.<p><strong>Press Activate to confirm Reset<strong></p></div>";
    ask_init_options[INIT_OUTPUTS] = "<div>This will reset all output calibrations.<p><strong>Press Activate to confirm Reset<strong></p></div>";
    ask_init_options[INIT_ALL] = "<div>This will reset all parameters, user waveforms, and calibrations. <p><strong>Press Activate to confirm Reset<strong></p></div>";
}
