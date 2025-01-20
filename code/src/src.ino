
/*
  Greenface Labs 2024
  Source code for The Bonkulator and Thor's Arsenal
*/

#include <Wire.h>
#include <Arduino.h>

// generated files
#include "config.h"
// end of generated files

#include "./secrets/arduino_secrets.h"
#include "product_defs.h"
#include "version_num.h"
#include "is_release.h"
#include "face1.h" // greenface logo art

#include "super_globals.h" // global variables that below classes need
#include <Greenface_gadget.h>
#include "EEPROM_fxns.h"
#include "EEPROM_ArrInt.h"
#include "TRIGGER.h"
#include "ADC.h"
#include "hardware_defs.h"
#include "globals.h" // global variables & fxn declarations

// interface to modules
#include "greenface_display.h"
#include "lolevel_fxns.h"
#include "thors_hardware_fxns.h"
#include "hardware_fxns.h"
#include "Rotary_Encoder_fxns.h"
#include "terminal_util.h"
#include "gen_macros.h"

#ifdef WIFI_INSTALLED
#include "WIFI_Util.h"
#include <Greenface_wifi_fxns.h>
#endif

#include "waveform_data.h"
#include "output_fxns.h"
#include "app_interface.h"
#include "user_waveforms_fxns.h"
#include "input_cal_fxns.h"
#include "output_cal_fxns.h"
#include "bounce_fxns.h"
#include "settings_fxns.h"
#include "hilevel_fxns.h"
#include "Keyboard_fxns.h"

#ifdef WIFI_INSTALLED
#include "wifi_ui.h"
#endif

// guts of DAC servicing
#include "cv_fxns.h"
#include "timer.h"
#include "waveform_fxns.h"
#include "trigger_fxns.h"

// EEPROM_Int init_test = EEPROM_Int(0, 7); // for testing sensing changes to fram

// EEO must come after last EEPROM var is declared
#define EEO Greenface_EEPROM::eeprom_offset

#include "Main_fxns.h"
Main_fxns &main_fxn = Main_fxns::getInstance();

void trigger_output(byte trig_num, int output_num) { main_fxn.trigger_output(trig_num, output_num); }
int get_active_time_index() { return main_fxn.get_active_time_index(); }
String list_fxns() { return main_fxn.list_fxns(); }
void _select_fxn(int fxn_index) { main_fxn._select_fxn(fxn_index); }
void select_fxn(int fxn_index) { main_fxn.select_fxn(fxn_index); }
bool in_output_fxn() { return main_fxn.in_output_fxn(); }
void init_all() { main_fxn.init_all(); }
void init_parameters() { main_fxn.init_parameters(); }
void init_input_cal() { main_fxn.init_input_cal(); }
void init_output_cal() { main_fxn.init_output_cal(); }
void set_group_vals(bool init) { main_fxn.set_group_vals(init); }

// the setup function runs once when you press reset or power the board
void setup()
{

  main_fxn.setup();
}

// the loop function runs over and over again forever
void loop()
{
  main_fxn.loop();
}
