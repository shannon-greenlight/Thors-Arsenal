#ifndef GLOBALS_H
#define GLOBALS_H

typedef void (*FunctionPointer)();
typedef void (*update_fxn)(void);

#define OUTPUT_0_FXN 0
#define OUTPUT_1_FXN 1
#define OUTPUT_2_FXN 2
#define OUTPUT_3_FXN 3
#define OUTPUT_4_FXN 4
#define OUTPUT_5_FXN 5
#define OUTPUT_6_FXN 6
#define OUTPUT_7_FXN 7
#define SETTINGS_FXN 8
#define NUM_FXNS 9

#define NULL_PARAM -1 // Only used by put_group()

#define ACTIVATE_STRING "Push Activate"

// for compatibility with Spankulator in gadget code
int user_entering_param = 1;

// interrupt variable
volatile uint32_t Timer0Count = 0;

// variables
String system_message_debug = "Howdy!";
uint16_t group = 0; // group outputs together for setting params en masse.
bool group_active = true;
String version_num = VERSION_NUM;
boolean key_held_down = false;
int16_t adc0;
int16_t adc1;
// int dac_value[NUM_OUTPUTS];
boolean in_wifi();

// Main_fxns
String list_fxns();
void _select_fxn(int fxn_index);
void select_fxn(int fxn_index);
bool in_output_fxn();
void init_all();
void init_parameters();
void init_input_cal();
void init_output_cal();
void set_group_vals(bool init = false);
uint8_t calc_output(uint8_t output_num)
{
    return output_num;
}

// settings
bool in_settings();
bool in_output_cal_fxn();
bool in_user_waveforms();
bool in_ask_init();
String settings_get_device_name();
uint16_t settings_get_encoder_type();
int settings_get_inactivity_timeout();
bool wifi_enabled(void);
bool usb_direct_enabled();
void settings_put_usb_direct(int val);
void timer_service_settings();
bool settings_get_fs_fixed();
void send_data_to_USB(char cmd);
void send_ok_to_USB(char cmd);
void send_item_to_USB(String item_name, String item_value);
uint16_t settings_get_bounce();
String settings_get_version();
void set_user_waveforms(String names);
bool in_bounce_fxn();
int get_active_time_index();

// cv fxns
void cv_set(int cv_num, int output, int16_t cv_val);
void update_cv(int cv_num);
void force_cv();

// timer
FunctionPointer timer0_fxn;
void timer_debug();
uint16_t quantize_value(uint16_t value, uint16_t output_num);

// waveforms
void apply_params_to_waveform(int output);
uint16_t get_waveform_parts(int output);
void dump_waveform(int output_num, bool dump_ref);
void dump_waveform_or_pattern(int dig1, int dig2) { dump_waveform(dig1, dig2 > 0); }
void graph_waveform(int output_num);
void set_waveform(int, int);
void set_output_phase(int output, int phase);
String get_waveform_message();
int calc_wave_value(int i, int waveform, uint16_t waveform_parts, float phase = 0.0);
void recv_user_waveform(String in_str);
bool output_is_bipolar(uint8_t output);

// Output Calibration / Corrections
const float scale_correction = .945; // sets max at 10V instead of 10.666V
void output_corrections_init();
bool get_output_calibrated(int);
int get_raw_output_offset_correction(int output);
int get_output_scale_correction(int output);

// Input Calibration / Corrections
void input_corrections_init();
bool get_input_calibrated(int input);
int get_raw_input_offset_correction(int input);
int get_input_scale_correction(int input);

// triggers
bool check_any_triggers();
void clear_all_triggers();
void disable_all_triggers();
void trigger_output(byte trig_num, int output_num);
void trigger_report();
void clear_trigger(int output_num, int trig_num);
void send_trig_status_to_USB();
void trigger_output(byte trig_num, int output_num);
int get_selected_trig_enable();
int get_selected_trigger_toggled(int dig2, int selected_trig_enable);

// WiFi, USB
String params_toJSON();
String list_fxns();
String output_get_fs_offset();
String wifi_toJSON();
bool usb_paused = false;

EEPROM_Int selected_output = EEPROM_Int(0, 7); // the output we are working on
EEPROM_Int remembered_fxn = EEPROM_Int(0, 8);  // 0-7=OutputX, 8=Settings
Greenface_gadget *selected_fxn;
uint8_t selected_fxn_num = 0;

TRIGGER trig0(0);
TRIGGER trig1(1);
TRIGGER trig2(2);
TRIGGER trig3(3);
TRIGGER *selected_trigger;
#define NUM_TRIGGERS 4
TRIGGER *triggers[NUM_TRIGGERS] = {&trig0, &trig1, &trig2, &trig3};
int trigger_leds[NUM_TRIGGERS] = {T0_LED, T1_LED, T2_LED, T3_LED};
void select_trigger(int int_param); // used by Keyboard fxns

// LEDs
enum
{
    LED_T0,
    LED_T1,
    LED_T2,
    LED_T3,
    LED_RED,
    LED_GREEN,
    LED_BLUE,
    LED_ROTARY_RED,
    LED_ROTARY_GREEN,
    NUM_LEDS
}; // for all LEDs

#ifdef THORS_ARSENAL
byte leds[NUM_LEDS] = {T0_LED, T1_LED, T2_LED, T3_LED, LEDR, LEDG, LEDB, LEDAR, LEDAG};
#elif defined BONKULATOR
byte leds[NUM_LEDS] = {T0_LED, T1_LED, T2_LED, T3_LED, T0_LED, T1_LED, T2_LED, T3_LED, T3_LED};
#endif

// used by timer.h
#define CV_OFF 0
#define CV_SCALE 1
#define CV_OFFSET 2
#define CV_ACTIVE_TIME 3
#define CV_IDLE_VALUE 4
#define CV_RANDOMNESS 5

#define CV_TYPES "Off,Scale,Offset,Active Time,Idle Value,Randomness"

// function param defines
enum
{
    OUTPUT_WAVEFORM,
    OUTPUT_INIT_DELAY,
    OUTPUT_ACTIVE_TIME,
    OUTPUT_IDLE_TIME,
    OUTPUT_REPEAT,
    OUTPUT_ENABLE_T0,
    OUTPUT_ENABLE_T1,
    OUTPUT_ENABLE_T2,
    OUTPUT_ENABLE_T3,
    OUTPUT_CV0,
    OUTPUT_CV1,
    OUTPUT_RANGE,
    OUTPUT_SCALE,
    OUTPUT_OFFSET,
    OUTPUT_PHASE,
    OUTPUT_RANDOMNESS,
    OUTPUT_QUANTIZE,
    OUTPUT_IDLE_VALUE,
    OUTPUT_CLOCK,
    OUTPUT_TRIG_CTRL,
    OUTPUT_TRIG_CTRL_VAL,
    NUM_OUTPUT_PARAMS
};

enum
{
    TRIGGER_FXN_WAVEFORM,
    TRIGGER_FXN_INIT_DELAY,
    TRIGGER_FXN_ACTIVE_TIME,
    TRIGGER_FXN_IDLE_TIME,
    TRIGGER_FXN_REPEAT,
    TRIGGER_FXN_ENABLE_T0,
    TRIGGER_FXN_ENABLE_T1,
    TRIGGER_FXN_ENABLE_T2,
    TRIGGER_FXN_ENABLE_T3,
    TRIGGER_FXN_CV0,
    TRIGGER_FXN_CV1,
    TRIGGER_FXN_RANGE,
    TRIGGER_FXN_SCALE,
    TRIGGER_FXN_OFFSET,
    TRIGGER_FXN_PHASE,
    TRIGGER_FXN_RANDOMNESS,
    TRIGGER_FXN_QUANTIZE,
    TRIGGER_FXN_IDLE_VALUE,
    TRIGGER_FXN_CLOCK,
    TRIGGER_FXN_TRIG_CTRL,
    TRIGGER_FXN_TRIG_CTRL_VAL,
    NUM_TRIGGER_FXN_PARAMS
};

enum
{
    SETTINGS_VER,
    SETTINGS_NAME,
    SETTINGS_ENCODER,
    SETTINGS_WIFI,
    SETTINGS_USB_DIRECT,
    SETTINGS_SCREEN,
    SETTINGS_USER_WAVEFORMS,
    SETTINGS_RESET,
    SETTINGS_CAL,
    SETTINGS_BOARD_GENERATION,
    SETTINGS_BOUNCE,
    SETTINGS_NUM_PARAMS
};

enum
{
    INIT_ALL,
    INIT_PARAMETERS,
    INIT_USER_WAVEFORMS,
    INIT_INPUTS,
    INIT_OUTPUTS
};

#endif // GLOBALS_H
