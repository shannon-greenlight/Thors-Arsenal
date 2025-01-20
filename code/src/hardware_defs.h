#ifndef Hardware_defs_h
#define Hardware_defs_h

#define ROTARY_P_PIN 4
#define ROTARY_A_PIN 5
#define ROTARY_B_PIN 6

#define CS_PIN 7
#define DOUT_PIN 8
#define CLK_PIN 9

// Triggers
#define T0_PIN 10
#define T1_PIN 11
#define T2_PIN 12
#define T3_PIN 13

// Trigger LEDs
#define T0_LED 0
#define T1_LED 1
#define T2_LED 2
#define T3_LED 3

#define DAC_FS 4095
#define DAC_MID 2048
#define ADC_FS 4095
#define ADC_FS_OVER_2 2048
#define ADC_FS_OVER_4 1024
// #define IDEAL_SCALE_CORRECTION 976 // for correcting DAC outputs. .9736 is 5.478 / 5.333 which is desired FS / board FS
#define IDEAL_SCALE_CORRECTION 1000 // DAC outputs are corrected via adjusting Vref now.

#define OLD_OUTPUT_VOLTS_MAX 10.956
#define OUTPUT_VOLTS_MAX 10.667

#if defined(THORS_ARSENAL) || defined(FAFNIRS_FIRE)
#define IN_FS_VOLTS 10.082
#define CV_MOD_SCALE 1.2412
const String CV_GAIN_INPUT_RANGE = "-10V - +10V";
const String CV_GAIN_RANGE = ".01 - 100";
const String CV_IN_MAX = "10V";
#elif defined BONKULATOR
#define IN_FS_VOLTS 4.9665
#define CV_MOD_SCALE .6206
const String CV_GAIN_INPUT_RANGE = "-5V - +5V";
const String CV_GAIN_RANGE = ".1 - 10";
const String CV_IN_MAX = "5V";
#endif

#endif
