#include <Arduino.h>
String output_text[NUM_OUTPUT_PARAMS] = {};
String output_text_extra[NUM_OUTPUT_PARAMS][NUM_WAVEFORMS] = {};
void output_text_begin()
{
    const String is_user_wave = "<div>{param_as_string} is a User Waveform. User waveforms are created, imported and edited in Settings.</div>";
    output_text[OUTPUT_WAVEFORM] = "<div>The {param_as_string} wave will be sent when {param_name} is triggered.</div>";
    output_text_extra[OUTPUT_WAVEFORM][WAVEFORM_PULSE] += "<div>The Pulse waveform will send a pulse whose height is determined by the SCALE setting and the baseline is set using IDLE VALUE.</div>";
    output_text_extra[OUTPUT_WAVEFORM][WAVEFORM_PULSE] += "<div>Make sure that IDLE TIME is non-zero.</div>";
    output_text_extra[OUTPUT_WAVEFORM][WAVEFORM_MAYTAG] += "<div>Maytag generates a random waveform pattern that repeats. You can change the pattern by re-selecting Maytag.</div>";
    output_text_extra[OUTPUT_WAVEFORM][WAVEFORM_TOGGLE] += "<div>The Toggle waveform simply toggles between two values when triggered. The SCALE setting determines the amplitude of the waveform and the OFFSET determines where the baseline appears.</div>";
    output_text_extra[OUTPUT_WAVEFORM][NUM_STD_WAVEFORMS] = is_user_wave;
    output_text_extra[OUTPUT_WAVEFORM][NUM_STD_WAVEFORMS + 1] = is_user_wave;
    output_text_extra[OUTPUT_WAVEFORM][NUM_STD_WAVEFORMS + 2] = is_user_wave;
    output_text_extra[OUTPUT_WAVEFORM][NUM_STD_WAVEFORMS + 3] = is_user_wave;
    output_text_extra[OUTPUT_WAVEFORM][NUM_STD_WAVEFORMS + 4] = is_user_wave;
    output_text_extra[OUTPUT_WAVEFORM][NUM_STD_WAVEFORMS + 5] = is_user_wave;
    output_text_extra[OUTPUT_WAVEFORM][NUM_STD_WAVEFORMS + 6] = is_user_wave;
    output_text_extra[OUTPUT_WAVEFORM][NUM_STD_WAVEFORMS + 7] = is_user_wave;

    output_text[OUTPUT_INIT_DELAY] = "<div>This is the time in milliseconds that the waveform will be delayed after a trigger. The signal will remain at the Idle Value during this period.</div>";

    output_text[OUTPUT_ACTIVE_TIME] = "<div>This is the time in milliseconds that the selected waveform will last. The fidelity of the wave will change depending on this setting. The ideal fidelity is when this setting is multiples of 128.</div>";

    output_text[OUTPUT_IDLE_TIME] = "<div>This is the time in milliseconds that will appear between repeats of the waveform. The signal will assume the Idle Value during this period.</div>";

    output_text[OUTPUT_REPEAT] = "<div>This is the number of times the waveform will repeat when triggered. Set this parameter to 0 if you want the signal to repeat indefinitely.</div>";

    output_text[OUTPUT_ENABLE_T0] = "<div>This Enables or Disables Trigger 0 to trigger {param_name}.</div>";
    output_text[OUTPUT_ENABLE_T0] += "<div>If Trigger 0 is enabled while the trigger is active, {param_name} will immediately be triggered.</div>";
    output_text[OUTPUT_ENABLE_T0] += "<div>If Trigger 0 is disabled while the trigger is active, {param_name} will immediately stop.</div>";

    output_text[OUTPUT_ENABLE_T1] = "<div>This Enables or Disables Trigger 1 to trigger {param_name}.</div>";
    output_text[OUTPUT_ENABLE_T1] += "<div>If Trigger 1 is enabled while the trigger is active, {param_name} will immediately be triggered.</div>";
    output_text[OUTPUT_ENABLE_T1] += "<div>If Trigger 1 is disabled while the trigger is active, {param_name} will immediately stop.</div>";

    output_text[OUTPUT_ENABLE_T2] = "<div>This Enables or Disables Trigger 2 to trigger {param_name}.</div>";
    output_text[OUTPUT_ENABLE_T2] += "<div>If Trigger 2 is enabled while the trigger is active, {param_name} will immediately be triggered.</div>";
    output_text[OUTPUT_ENABLE_T2] += "<div>If Trigger 2 is disabled while the trigger is active, {param_name} will immediately stop.</div>";

    output_text[OUTPUT_ENABLE_T3] = "<div>This Enables or Disables Trigger 3 to trigger {param_name}.</div>";
    output_text[OUTPUT_ENABLE_T3] += "<div>If Trigger 3 is enabled while the trigger is active, {param_name} will immediately be triggered.</div>";
    output_text[OUTPUT_ENABLE_T3] += "<div>If Trigger 3 is disabled while the trigger is active, {param_name} will immediately stop.</div>";

    output_text[OUTPUT_CV0] = "<div class='warn'>A * on Input {cv_param_label_raw} means that it is uncalibrated.<br>Calibrate it in Settings.</div>";
    output_text[OUTPUT_CV0] += "<div>Input {cv_param_label_raw}";
    const String output_text_xtra = "<div class='info'>The CV functions are only enabled when the output has been triggered.</div>";
    output_text_extra[OUTPUT_CV0][CV_OFF] = "is disabled.</div>" + output_text_xtra;
    output_text_extra[OUTPUT_CV0][CV_SCALE] = "is set to scale the signal. Signal gain is = 10<sup>0.2Vi</sup>, where Vi is the input voltage.<br>This results in a gain range of " + CV_GAIN_RANGE + " for an input range of " + CV_GAIN_INPUT_RANGE + ".</div> " + output_text_xtra;
    output_text_extra[OUTPUT_CV0][CV_OFFSET] = "is set to offset the waveform. The output signal will shifted by the value of the input voltage.</div>" + output_text_xtra;
    output_text_extra[OUTPUT_CV0][CV_ACTIVE_TIME] = "is set to control the Active Time of the signal. It does this by modifying the time increment for each step of the output wave. The input range is:<br/>0-" + CV_IN_MAX + " for increments of 1 ms - 512ms.</div>" + output_text_xtra;
    output_text_extra[OUTPUT_CV0][CV_IDLE_VALUE] = "is set to control the Idle Value of the signal. The output signal level follows the level of the input.<br/>This parameter only applies when the trigger is active and Idle Time is greater than 0.</div>" + output_text_xtra;
    output_text_extra[OUTPUT_CV0][CV_RANDOMNESS] = "is set to control the randomness of the signal. An input range of 0-" + CV_IN_MAX + " results in a randomness of 0-100 per cent of the signal's amplitude.</div>" + output_text_xtra;

    // output_text[OUTPUT_CV1] = "<div class='warn'>A * on Input {cv_param_label_raw} means that it is uncalibrated.<br>Calibrate it in Settings.</div>";
    // output_text[OUTPUT_CV1] += "<div>The CV functions are only enabled when the output has been triggered.</div>";
    // output_text[OUTPUT_CV1] += "<div>Input {cv_param_label_raw}";
    output_text[OUTPUT_CV1] = output_text[OUTPUT_CV0];
    output_text_extra[OUTPUT_CV1][CV_OFF] = output_text_extra[OUTPUT_CV0][CV_OFF];
    output_text_extra[OUTPUT_CV1][CV_SCALE] = output_text_extra[OUTPUT_CV0][CV_SCALE];
    output_text_extra[OUTPUT_CV1][CV_OFFSET] = output_text_extra[OUTPUT_CV0][CV_OFFSET];
    output_text_extra[OUTPUT_CV1][CV_ACTIVE_TIME] = output_text_extra[OUTPUT_CV0][CV_ACTIVE_TIME];
    output_text_extra[OUTPUT_CV1][CV_IDLE_VALUE] = output_text_extra[OUTPUT_CV0][CV_IDLE_VALUE];
    output_text_extra[OUTPUT_CV1][CV_RANDOMNESS] = output_text_extra[OUTPUT_CV0][CV_RANDOMNESS];

    output_text[OUTPUT_SCALE] = "<div>This is how you set the amplitude of the waveform. The units are in per cent. A negative scale inverts the waveform.</div>";
    output_text[OUTPUT_SCALE] += "<div>The Scale has no effect on the Idle Value. It only affects the waveform.</div>";

    output_text[OUTPUT_OFFSET] = "<div>This is how you set the waveform's offset. The units are in Volts.</div>";
    output_text[OUTPUT_OFFSET] += "<div>The Offset has no effect on the Idle Value. It only affects the waveform.</div>";

    output_text[OUTPUT_PHASE] = "<div>This is how you set the waveform's phase. The value is in per cent. 100% = 360&deg;. For example, setting the PHASE to 25% changes a sine wave into a cosine wave.</div>";

    output_text[OUTPUT_RANDOMNESS] = "<div>This is how you set the waveform's randomness. The value indicates a level of randomness that is the percent of the waveform's amplitude.</div>";

    output_text[OUTPUT_QUANTIZE] = "<div>When set to 'Yes', the steps of the waveform will be adjusted to fit into the 12-step 1V/octave scale factor of most VCOs.</div>";
    output_text[OUTPUT_QUANTIZE] += "<div>When set to 'No', the steps of the waveform will be exactly as found in the selected wave table.</div>";

    output_text[OUTPUT_IDLE_VALUE] = "<div>This is the voltage that the signal will assume when the trigger is inactive or when the trigger is active and the Idle Time is non-zero. The units are in Volts.</div>";

    output_text[OUTPUT_CLOCK] = "<div>When set to Internal, the frequency of the clock is determined by the Active Time setting and the number of steps in the waveform. It will always be an integer multiple of 1ms.</div>";
    output_text[OUTPUT_CLOCK] += "<div>When set to External, the clock is determined by the trigger input.</div>";

    output_text[OUTPUT_RANGE] = "<div>The Output Range setting merely presets the SCALE and OFFSET.</div>";
    output_text[OUTPUT_RANGE] += "<div>When set to +/-5V, the SCALE is set to 50% and the OFFSET is set to 0%.</div>";
    output_text[OUTPUT_RANGE] += "<div>When set to 0-10V, the SCALE is set to 50% and the OFFSET is set to 50%.</div>";
    output_text[OUTPUT_RANGE] += "<div>When set to +/-10V, the SCALE is set to 100% and the OFFSET is set to 0%.</div>";

    output_text[OUTPUT_TRIG_CTRL] = "<div>The Trigger Control determines how trigger signals to this output will be handled.</div>";
    output_text[OUTPUT_TRIG_CTRL] += "<div>When set to Hold_Off, the output can not be triggered again until the Holdoff time (in milliseconds) has expired.</div>";
    output_text[OUTPUT_TRIG_CTRL] += "<div>When set to Skip, the output will skip the selected number of incoming trigger signals before before it can be triggered again.</div>";
    output_text[OUTPUT_TRIG_CTRL] += "<div>When set to Density, each incoming trigger signal will have the selected percentage chance of triggering the output.</div>";

    output_text[OUTPUT_TRIG_CTRL_VAL] = "<div>Hold_Off: After the output is triggered, it will wait {param_as_string} milliseconds before it can be triggered again.</div>";
    output_text[OUTPUT_TRIG_CTRL_VAL] += "<div>Skip: After the output is triggered, {param_as_string} trigger signals will be ignored before it can be triggered again.</div>";
    output_text[OUTPUT_TRIG_CTRL_VAL] += "<div>Density: Each incoming trigger signal will have a {param_as_string}% chance of triggering the output.</div>";
}
