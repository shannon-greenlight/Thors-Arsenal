#include <Arduino.h>
#if defined(THORS_ARSENAL) || defined(FAFNIRS_FIRE)
#define USER_WAVE_OUTPUT_VOLTS_MAX 10.67
#elif defined BONKULATOR
#define USER_WAVE_OUTPUT_VOLTS_MAX 5.33
#endif
String user_waveforms_text[NUM_USER_WAVEFORMS_PARAMS] = {};
String user_waveforms_text_extra[NUM_OUTPUT_PARAMS][NUM_WAVEFORMS] = {}; // Must match declaration in .h file. That's why NUM_OUTPUT_PARAMS & NUM_WAVEFORMS is used.
void user_waveforms_text_begin()
{
    user_waveforms_text[USER_WAVEFORMS_NAME] = "<div><strong>{param_as_string}</strong> is the name of this waveform.</div>";
    user_waveforms_text[USER_WAVEFORMS_NAME] += "You can change the name to your liking. Just double-click to edit.";
    user_waveforms_text[USER_WAVEFORMS_NAME] += "<div>There are several methods for dealing with User waveforms.</div>";
    user_waveforms_text[USER_WAVEFORMS_NAME] += "<li>User waveforms can be edited directly via the Index and Value parameters.</li>";
    user_waveforms_text[USER_WAVEFORMS_NAME] += "<li>User waveforms can be recorded from the CV0 input. Select the method that will start the recording, set the Sample Time, then click on Record and press Activate to start the recording.</li>";
    user_waveforms_text[USER_WAVEFORMS_NAME] += "<li>User waveforms can also be drawn directly on the screen. Just click Draw Waveform. You can save or cancel the drawing to return to the main User waveforms menu.</li>";
    user_waveforms_text[USER_WAVEFORMS_NAME] += "<li>User waveforms can also be imported from .wav files using the WaveImporter app. This will import a wave into the clipboard. You then paste it into the User Waveform text area and click Import.</li>";
    user_waveforms_text[USER_WAVEFORMS_NAME] += "<li>Finally, User waveforms can also be initialized to a standard waveform by selecting the Init Wave parameter.</li>";

    user_waveforms_text[USER_WAVEFORMS_INDEX] = "<div>Set this parameter to position the edit cursor. The index is denoted by an orange dot on the graph. You may also click on the graph in the horizontal area you want.</div>";

    user_waveforms_text[USER_WAVEFORMS_VALUE] = "<div>Set this parameter to the value you want at the index position.</div>";
    user_waveforms_text[USER_WAVEFORMS_VALUE] += "<div>4095 is full scale, or approx. " + String(USER_WAVE_OUTPUT_VOLTS_MAX) + "V.</div>";
    user_waveforms_text[USER_WAVEFORMS_VALUE] += "<div>0 is negative full scale, or approx. -" + String(USER_WAVE_OUTPUT_VOLTS_MAX) + "V.</div>";
    user_waveforms_text[USER_WAVEFORMS_VALUE] += "<div>2048 is 0.0V.</div>";
    user_waveforms_text[USER_WAVEFORMS_VALUE] += "<div><strong>Press Activate to advance to the next Index.</strong></div>";

    user_waveforms_text[USER_WAVEFORMS_SAMPLE_TIME] = "<div>This is the time in milliseconds between samples when recording. Recording is done from the signal at CV0. Recordings consist of 128 parts. The length of the segment you want to record will determine your choice for Sample Time. </div>";

    // user_waveforms_text[USER_WAVEFORMS_START] = "<div>Set this parameter to determine when to start recording.</div>";
    user_waveforms_text_extra[USER_WAVEFORMS_START][USER_WAVEFORMS_START_IMMEDIATE] += "<div>The recording will begin immediately when the Activate button is pressed. You must first select the <strong>Record</strong> parameter to activate a recording.</div>";
    user_waveforms_text_extra[USER_WAVEFORMS_START][USER_WAVEFORMS_START_TRIGGER] += "<div>The recording will begin when a trigger is received from any of the trigger inputs. You must first select the <strong>Record</strong> parameter to activate a recording.</div>";
    user_waveforms_text_extra[USER_WAVEFORMS_START][USER_WAVEFORMS_START_PLUS_CHANGE] += "<div>The recording will begin when the signal at CV0 changes by a positive amount greater than approx. 125mV. You must first select the <strong>Record</strong> parameter to activate a recording.</div>";
    user_waveforms_text_extra[USER_WAVEFORMS_START][USER_WAVEFORMS_START_MINUS_CHANGE] += "<div>The recording will begin when the signal at CV0 changes by a negative amount greater than approx. 125mV. You must first select the <strong>Record</strong> parameter to activate a recording.</div>";

    user_waveforms_text[USER_WAVEFORMS_RECORD] = "<div>Pressing Activate will cause the module to wait for the selected Start condition. Recording will begin when this condition is met.</div>";

    user_waveforms_text[USER_WAVEFORMS_INIT_WAVE] = "<div>Set this parameter to preset the waveform values to the selected wave. The default wave is a flat line.</div>";
}
