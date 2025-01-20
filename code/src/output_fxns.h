#include <Greenface_gadget.h>

// enum
// {
//     OUTPUT_WAVEFORM,
//     OUTPUT_INIT_DELAY,
//     OUTPUT_ACTIVE_TIME,
//     OUTPUT_IDLE_TIME,
//     OUTPUT_REPEAT,
//     OUTPUT_ENABLE_T0,
//     OUTPUT_ENABLE_T1,
//     OUTPUT_ENABLE_T2,
//     OUTPUT_ENABLE_T3,
//     OUTPUT_CV0,
//     OUTPUT_CV1,
//     OUTPUT_RANGE,
//     OUTPUT_SCALE,
//     OUTPUT_OFFSET,
//     OUTPUT_PHASE,
//     OUTPUT_RANDOMNESS,
//     OUTPUT_QUANTIZE,
//     OUTPUT_IDLE_VALUE,
//     OUTPUT_CLOCK,
//     OUTPUT_TRIG_CTRL,
//     OUTPUT_TRIG_CTRL_VAL,
//     NUM_OUTPUT_PARAMS
// };

typedef enum _output_range_type
{
    OUTPUT_RANGE_PLUS_MINUS_FIVE = 0,
    OUTPUT_RANGE_ZERO_TO_TEN = 1,
    OUTPUT_RANGE_MINUS_TEN_TO_TEN = 2,
} output_range_type;

// pre-installed waveforms
#define WAVEFORM_SINE 0
#define WAVEFORM_HAYSTACK 1
#define WAVEFORM_RAMP 2
#define WAVEFORM_PYRAMID 3
#define WAVEFORM_PULSE 4
#define WAVEFORM_MAYTAG 5
#define WAVEFORM_TOGGLE 6
#define NUM_STD_WAVEFORMS 7

#define INIT_STD_WAVEFORMS "Sine,Haystack,Ramp,Pyramid,Pulse,Maytag,Toggle"
#define INIT_WAVEFORMS "Sine,Haystack,Ramp,Pyramid,Pulse,Maytag,Toggle,User 0,User 1,User 2,User 3,User 4,User 5,User 6,User 7"

// // used by timer.h
// #define CV_OFF 0
// #define CV_SCALE 1
// #define CV_OFFSET 2
// #define CV_ACTIVE_TIME 3
// #define CV_IDLE_VALUE 4
// #define CV_RANDOMNESS 5

// #define CV_TYPES "Off,Scale,Offset,Active Time,Idle Value,Randomness"

#define TIME_INC_MIN 1
#define TIME_INC_MAX 32000

#define ACTIVE_TIME_MIN 10
#define MAX_MS 65535
#define DELAY_MAX 32767

#define OUTPUT_SCALE_OFFSET -100
#define OUTPUT_OFFSET_OFFSET -100

#define OUTPUT_OFFSET_FACTOR 1.9194 // 2048/1067 need to adjust if offset dp or OUTPUT_VOLTS_MAX changes
#if defined(THORS_ARSENAL)
#define OUTPUT_RANGE_MAX OUTPUT_RANGE_MINUS_TEN_TO_TEN
#define OUTPUT_OFFSET_MAX_UI 2000
#define OUTPUT_OFFSET_INIT 1000
#define OUTPUT_OFFSET_OFFSET_UI -1000
#define OUTPUT_IDLE_VALUE_MAX_UI 2000
#define OUTPUT_IDLE_VALUE_INIT 1000
#define OUTPUT_IDLE_VALUE_OFFSET_UI -1000
#define OUTPUT_RANGE_VALUES "+/-5V,0-10V,+/-10V"
#define QUANTIZE_FACTOR 0xFFF0
#elif defined(BONKULATOR)
#define OUTPUT_RANGE_MAX OUTPUT_RANGE_ZERO_TO_TEN
#define OUTPUT_OFFSET_MAX_UI 1000
#define OUTPUT_OFFSET_INIT 500
#define OUTPUT_OFFSET_OFFSET_UI -500
#define OUTPUT_IDLE_VALUE_MAX_UI 1000
#define OUTPUT_IDLE_VALUE_INIT 500
#define OUTPUT_IDLE_VALUE_OFFSET_UI -500
#define OUTPUT_RANGE_VALUES "+/-5V,0-10V"
#define QUANTIZE_FACTOR 0xFFE0
#else
#error "Unknown PRODUCT_NAME"
#endif

#define TRIGGER_STATES F("Disabled,Enabled ")

void update_clock();
void set_output_range(uint8_t output, output_range_type output_range);
void output_update_range();
void update_idle_value();
void update_waveform();

uint16_t _output_mins[] = {0, 0, ACTIVE_TIME_MIN, 0, 0, 0, 0, 0, 0, CV_OFF, CV_OFF, OUTPUT_RANGE_PLUS_MINUS_FIVE, 0, 0, 0, 0, 0, 0, 0, TRIG_HOLDOFF, 0};
uint16_t _output_maxs[] = {NUM_WAVEFORMS - 1, DELAY_MAX, MAX_MS, DELAY_MAX, 32767, 1, 1, 1, 1, CV_RANDOMNESS, CV_RANDOMNESS, OUTPUT_RANGE_MAX, 200, OUTPUT_OFFSET_MAX_UI, 100, 99, 1, OUTPUT_IDLE_VALUE_MAX_UI, 1, TRIG_DENSITY, MAX_MS};
uint16_t _output_init_vals[] = {0, 0, 128, 0, 0, 0, 0, 0, 0, CV_OFF, CV_OFF, OUTPUT_RANGE_PLUS_MINUS_FIVE, 150, OUTPUT_OFFSET_INIT, 0, 0, 0, OUTPUT_IDLE_VALUE_INIT, 0, TRIG_HOLDOFF, 5};

#define OUTPUT_LABELS {"Waveform: ", "Init Delay: ", "Active Time/Parts: ", "Idle Time: ", "Repeat: ", "T0: ", "T1: ", "T2: ", "T3: ", "CV0: ", "CV1: ", "Range: ", "Scale: ", "Offset: ", "Phase: ", "Randomness: ", "Quantize: ", "Idle Value: ", "Clock: ", "Trig Ctrl: ", "Trig Holdoff: "};
String output_labels0[] = OUTPUT_LABELS;
String output_labels1[] = OUTPUT_LABELS;
String output_labels2[] = OUTPUT_LABELS;
String output_labels3[] = OUTPUT_LABELS;
String output_labels4[] = OUTPUT_LABELS;
String output_labels5[] = OUTPUT_LABELS;
String output_labels6[] = OUTPUT_LABELS;
String output_labels7[] = OUTPUT_LABELS;

// user_waveforms alters [0] so we need to have an init array to undo this
String output_string_param_init_vals[] = {INIT_WAVEFORMS, "", "", "", "", TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, CV_TYPES, CV_TYPES, OUTPUT_RANGE_VALUES, "", "", "", "", "No ,Yes", "", "Internal,External", "Hold_Off,Skip,Density", ""};
String output_string_params[] = {INIT_WAVEFORMS, "", "", "", "", TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, CV_TYPES, CV_TYPES, OUTPUT_RANGE_VALUES, "", "", "", "", "No ,Yes", "", "Internal,External", "Hold_Off,Skip,Density", ""};

int8_t decimal_places[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0}; // allows fixed point numbers

// Output definitions
uint16_t _output0_params[NUM_OUTPUT_PARAMS];
uint16_t *output0_stuff[] = {_output0_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output0("Output 0", output_labels0, output0_stuff, sizeof(_output0_params) / sizeof(_output0_params[0]));

uint16_t _output1_params[NUM_OUTPUT_PARAMS];
uint16_t *output1_stuff[] = {_output1_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output1("Output 1", output_labels1, output1_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

uint16_t _output2_params[NUM_OUTPUT_PARAMS];
uint16_t *output2_stuff[] = {_output2_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output2("Output 2", output_labels2, output2_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

uint16_t _output3_params[NUM_OUTPUT_PARAMS];
uint16_t *output3_stuff[] = {_output3_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output3("Output 3", output_labels3, output3_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

uint16_t _output4_params[NUM_OUTPUT_PARAMS];
uint16_t *output4_stuff[] = {_output4_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output4("Output 4", output_labels4, output4_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

uint16_t _output5_params[NUM_OUTPUT_PARAMS];
uint16_t *output5_stuff[] = {_output5_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output5("Output 5", output_labels5, output5_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

uint16_t _output6_params[NUM_OUTPUT_PARAMS];
uint16_t *output6_stuff[] = {_output6_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output6("Output 6", output_labels6, output6_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

uint16_t _output7_params[NUM_OUTPUT_PARAMS];
uint16_t *output7_stuff[] = {_output7_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output7("Output 7", output_labels7, output7_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

Greenface_gadget *bonk_outputs[NUM_OUTPUTS] = {
    &output0,
    &output1,
    &output2,
    &output3,
    &output4,
    &output5,
    &output6,
    &output7,
};

Greenface_gadget **triggered_fxn = bonk_outputs;

String alt_values0[NUM_OUTPUT_PARAMS] = {};
String alt_values1[NUM_OUTPUT_PARAMS] = {};
String alt_values2[NUM_OUTPUT_PARAMS] = {};
String alt_values3[NUM_OUTPUT_PARAMS] = {};
String alt_values4[NUM_OUTPUT_PARAMS] = {};
String alt_values5[NUM_OUTPUT_PARAMS] = {};
String alt_values6[NUM_OUTPUT_PARAMS] = {};
String alt_values7[NUM_OUTPUT_PARAMS] = {};

String *alt_values[NUM_OUTPUTS] =
    {
        alt_values0,
        alt_values1,
        alt_values2,
        alt_values3,
        alt_values4,
        alt_values5,
        alt_values6,
        alt_values7,
};

#define OUTPUT_OFFSETS {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OUTPUT_SCALE_OFFSET, OUTPUT_OFFSET_OFFSET_UI, 0, 0, 0, OUTPUT_IDLE_VALUE_OFFSET_UI, 0, 0, 0}

// OFFSETS allow negative numbers
int16_t output_offsets0[] = OUTPUT_OFFSETS;
int16_t output_offsets1[] = OUTPUT_OFFSETS;
int16_t output_offsets2[] = OUTPUT_OFFSETS;
int16_t output_offsets3[] = OUTPUT_OFFSETS;
int16_t output_offsets4[] = OUTPUT_OFFSETS;
int16_t output_offsets5[] = OUTPUT_OFFSETS;
int16_t output_offsets6[] = OUTPUT_OFFSETS;
int16_t output_offsets7[] = OUTPUT_OFFSETS;

int16_t *output_offsets[NUM_OUTPUTS] =
    {
        output_offsets0,
        output_offsets1,
        output_offsets2,
        output_offsets3,
        output_offsets4,
        output_offsets5,
        output_offsets6,
        output_offsets7,
};

String output_cmd[NUM_OUTPUT_PARAMS] = {};
bool output_is_slider[NUM_OUTPUT_PARAMS] = {};
String output_units0[NUM_OUTPUT_PARAMS] = {};
String output_units1[NUM_OUTPUT_PARAMS] = {};
String output_units2[NUM_OUTPUT_PARAMS] = {};
String output_units3[NUM_OUTPUT_PARAMS] = {};
String output_units4[NUM_OUTPUT_PARAMS] = {};
String output_units5[NUM_OUTPUT_PARAMS] = {};
String output_units6[NUM_OUTPUT_PARAMS] = {};
String output_units7[NUM_OUTPUT_PARAMS] = {};
String *output_units[NUM_OUTPUTS] = {output_units0, output_units1, output_units2, output_units3, output_units4, output_units5, output_units6, output_units7};

#include "output_text.h"

Greenface_gadget bonk_output(int output_num)
{
    return *bonk_outputs[output_num];
}

Greenface_gadget the_output()
{
    return *bonk_outputs[selected_output.get()];
}

void clear_alt_values()
{
    for (int i = 0; i < NUM_OUTPUT_PARAMS; i++)
    {
        (the_output)().alt_values[i] = "";
    }
}

void set_cv_alt_values(int cv, String label)
{
    // CV_TYPES;
    int param = -1;
    switch (cv)
    {
    case 1:
        param = OUTPUT_SCALE;
        break;
    case 2:
        param = OUTPUT_OFFSET;
        break;
    case 3:
        param = OUTPUT_ACTIVE_TIME;
        break;
    case 4:
        param = OUTPUT_IDLE_VALUE;
        break;
    case 5:
        param = OUTPUT_RANDOMNESS;
        break;
    }
    if (param != -1)
    {
        if ((the_output)().alt_values[param] > "" && (the_output)().alt_values[param] != label)
        {
            (the_output)().alt_values[param] += ("/" + label);
        }
        else
        {
            (the_output)().alt_values[param] = label;
        }
    }
}

String output_get_name(int output_num)
{
    String text = (*bonk_outputs[output_num]).name;
    if (!get_output_calibrated(output_num))
        text += "*";
    return text;
}

bool is_standard_waveform()
{
    int waveform = (the_output)().get_param(OUTPUT_WAVEFORM);
    return waveform < NUM_STD_WAVEFORMS;
}

bool is_toggle_waveform()
{
    int waveform = (the_output)().get_param(OUTPUT_WAVEFORM);
    return waveform == WAVEFORM_TOGGLE;
}

void set_phase_value(int output)
{
    int waveform = (*bonk_outputs[output]).get_param(OUTPUT_WAVEFORM);
    switch (waveform)
    {
    case WAVEFORM_SINE:
    case WAVEFORM_HAYSTACK:
    case WAVEFORM_RAMP:
    case WAVEFORM_PYRAMID:
        (*bonk_outputs[output]).alt_values[OUTPUT_PHASE] = "";
        break;
    default:
        (*bonk_outputs[output]).alt_values[OUTPUT_PHASE] = NOT_AVAILABLE;
        break;
    }
}

void output_display()
{
    update_clock();
    update_waveform();
    (the_output)().labels[OUTPUT_CV0] = get_input_calibrated(0) ? "CV0: " : "CV0* ";
    (the_output)().labels[OUTPUT_CV1] = get_input_calibrated(1) ? "CV1: " : "CV1* ";

    clear_alt_values();
    set_cv_alt_values((the_output)().get_param(OUTPUT_CV0), "CV0");
    set_cv_alt_values((the_output)().get_param(OUTPUT_CV1), "CV1");

    int wave_num = (the_output)().get_param(OUTPUT_WAVEFORM);
    clear_alt_values();
    switch (wave_num)
    {
    case WAVEFORM_TOGGLE:
        (the_output)().alt_values[OUTPUT_ACTIVE_TIME] = NOT_AVAILABLE;
        (the_output)().alt_values[OUTPUT_IDLE_TIME] = NOT_AVAILABLE;
        (the_output)().alt_values[OUTPUT_REPEAT] = NOT_AVAILABLE;
        (the_output)().alt_values[OUTPUT_IDLE_VALUE] = NOT_AVAILABLE;
        (the_output)().alt_values[OUTPUT_CLOCK] = NOT_AVAILABLE;
        break;
    case WAVEFORM_PULSE:
        (the_output)().alt_values[OUTPUT_OFFSET] = NOT_AVAILABLE;
        // (the_output)().labels[OUTPUT_ACTIVE_TIME] = "Active Time: ";
        // (the_output)().labels[OUTPUT_IDLE_TIME] = "Idle Time: ";
        break;
    default:
        // clear_alt_values();
        break;
    }

    set_phase_value(selected_output.get());
    update_idle_value();

    (the_output)().default_display();

    String text = output_get_name(selected_output.get());
    ui.printLine(text, 0, 2);
    // ui.terminal_debug("Waveform: " + String(wave_num));
    apply_params_to_waveform(selected_output.get());
    get_waveform_parts(selected_output.get());
    // ui.terminal_debug("Output: " + String(selected_output.get()));
}

// not used -- saved jic
void display_output()
{
    Greenface_gadget output = the_output();
    output.display();
    // (the_output)().display();
    // (*bonk_outputs[selected_output.get()]).display();
}

void init_all_outputs()
{
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        (*bonk_outputs[i]).init();
    }
}

bool output_is_bipolar(uint8_t output)
{
#ifdef THORS_ARSENAL
    return true;
#elif defined(BONKULATOR)
    return (*bonk_outputs[output]).get_param(OUTPUT_RANGE) != OUTPUT_RANGE_ZERO_TO_TEN;
#endif
}

String output_get_fs_offset()
{
    uint16_t range = (the_output)().get_param(OUTPUT_RANGE);
    return String(range);
}

String list_outputs()
{
    String out = "";
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        if (i > 0)
        {
            out += ", ";
        }
        out += enquote(output_get_name(i));
    }
    return out;
}

bool any_output_triggered()
{
    bool any_triggered = false;
    int i = 0;
    while (!any_triggered && i < NUM_OUTPUTS)
    {
        any_triggered = (*bonk_outputs[i++]).triggered;
    }
    return any_triggered;
}

void add_waveform()
{
    _output_maxs[OUTPUT_WAVEFORM]++;
    output_string_params[OUTPUT_WAVEFORM] += ",User Wave 0";
}

void set_trigger(int output)
{
    int trig_num = bonk_outputs[output]->param_num - OUTPUT_ENABLE_T0;
    int trig_type = bonk_outputs[output]->get_param();
    // int trig_hold_off = bonk_outputs[output]->get_param(OUTPUT_TRIG_CTRL_VAL);
    // ui.terminal_debug("Selected output: " + String(output) + " trig num: " + String(trig_num) + " trig_type: " + String(trig_type));
    triggers[trig_num]->set_channel(output, trig_type);
}

void update_trigger()
{
    set_trigger(selected_output.get());
}

void update_user_waveform_names(String names)
{
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        Greenface_gadget *output = bonk_outputs[i];
        output->string_params[0] = String(INIT_STD_WAVEFORMS + names);
    }
}

void update_waveform()
{
    int wave_num = (the_output)().get_param(OUTPUT_WAVEFORM);
    set_waveform(selected_output.get(), wave_num);
}

void update_waveform_params()
{
    int output = selected_output.get();
    apply_params_to_waveform(output);
}

void update_active_time()
{
    outputs[selected_output.get()].active_time = (the_output)().get_param(OUTPUT_ACTIVE_TIME);
    update_waveform();
}

void update_and_graph_waveform()
{
    // update_waveform();
    output_display();
    graph_waveform(selected_output.get());
}

void update_cv0()
{
    update_cv(OUTPUT_CV0);
}

void update_cv1()
{
    update_cv(OUTPUT_CV1);
}

void set_trig_ctrls(TRIGGER *trig)
{
    trig->set_trig_ctrls(selected_output.get(), (the_output)().get_param(OUTPUT_TRIG_CTRL), (the_output)().get_param(OUTPUT_TRIG_CTRL_VAL));
}

void update_trig_ctrl_vals()
{
    for (int i = 0; i < NUM_TRIGGERS; i++)
    {
        set_trig_ctrls(triggers[i]);
    }
}

void update_trig_ctrl()
{
    switch ((the_output)().get_param(OUTPUT_TRIG_CTRL))
    {
    case TRIG_HOLDOFF:
        (the_output)().labels[OUTPUT_TRIG_CTRL_VAL] = F("Trig Holdoff: ");
        (the_output)().maxs[OUTPUT_TRIG_CTRL_VAL] = MAX_MS;
        break;
    case TRIG_SKIP:
        (the_output)().labels[OUTPUT_TRIG_CTRL_VAL] = F("Trig Skip: ");
        (the_output)().maxs[OUTPUT_TRIG_CTRL_VAL] = MAX_MS;
        break;
    case TRIG_DENSITY:
        (the_output)().labels[OUTPUT_TRIG_CTRL_VAL] = F("Trig Density: ");
        (the_output)().maxs[OUTPUT_TRIG_CTRL_VAL] = 100; // 100%
        break;
    }
    // now set value in case the max was changed
    (the_output)().param_put((the_output)().get_param(OUTPUT_TRIG_CTRL_VAL), OUTPUT_TRIG_CTRL_VAL);
    update_trig_ctrl_vals();
    // ui.terminal_debug("update_trig_ctrl: " + (the_output)().name + " Trig Ctrl val: " + String((the_output)().get_param(OUTPUT_TRIG_CTRL_VAL)));
}

// randomness range 0:100, scale range 0.0:1.0
void set_randomness_factor(uint16_t randomness, float scale_factor, int output)
{
    OutputData *outptr = &outputs[output];
    float randomness_factor = DAC_FS * scale_factor * randomness / 200;
    outptr->randomness_factor = (int)randomness_factor;
}

void update_phase()
{
    int output = selected_output.get();
    int phase = (the_output)().get_param(OUTPUT_PHASE);
    set_output_phase(output, phase);
}

void update_wave()
{
    update_and_graph_waveform();
    set_phase_value(selected_output.get());
}

void update_randomness_factor()
{
    int output = selected_output.get();
    float scale_factor = (the_output)().get_param(OUTPUT_SCALE) / 100.0;
    int randomness = (the_output)().get_param(OUTPUT_RANDOMNESS);
    set_randomness_factor(randomness, scale_factor, output);
}

void set_idle_value(int val, int output)
{
    OutputData *outptr = &outputs[output];
    outptr->idle_value = max(0, val);
}

void send_idle_value(int output_num)
{
    dac.out(output_num, outputs[output_num].idle_value);
}

// uint16_t quantize(uint16_t val)
// {
//     return (*bonk_outputs[selected_output.get()]).get_param(OUTPUT_QUANTIZE) ? val & 0xFFF0 : val;
// }

// output scale factor * num steps in 1 volt / 12 .. for 12 bits this equals 32. Then negate to get FFE0.
uint16_t quantize_value(uint16_t value, uint16_t output_num)
{
    // ui.terminal_debug("Quantize: " + String(value));
    if ((*bonk_outputs[output_num]).get_param(OUTPUT_QUANTIZE))
    {
        value &= QUANTIZE_FACTOR;
    }
    // Serial.println("Value: " + String(value));
    return value;
}

void update_idle_value()
{
    int output = selected_output.get();
    float val = (float)(*bonk_outputs[output]).get_param_w_offset(OUTPUT_IDLE_VALUE) / pow(10, (*bonk_outputs[output]).get_decimal_places(OUTPUT_IDLE_VALUE));
    set_idle_value(quantize_value(dac.volts_to_dac(val, output, output_is_bipolar(output)), output), output);
    if (!(the_output)().triggered)
        send_idle_value(output);
}

void update_clock()
{
    switch ((the_output)().get_param(OUTPUT_CLOCK))
    {
    case 1:
        (the_output)().labels[OUTPUT_ACTIVE_TIME] = "Parts: ";
        (the_output)().maxs[OUTPUT_ACTIVE_TIME] = WAVEFORM_PARTS;
        (the_output)().alt_values[OUTPUT_INIT_DELAY] = NOT_AVAILABLE;
        (the_output)().alt_values[OUTPUT_IDLE_TIME] = NOT_AVAILABLE;
        (the_output)().alt_values[OUTPUT_REPEAT] = NOT_AVAILABLE;
        break;
    case 0:
        (the_output)().labels[OUTPUT_ACTIVE_TIME] = "Active Time: ";
        (the_output)().maxs[OUTPUT_ACTIVE_TIME] = MAX_MS;
        (the_output)().alt_values[OUTPUT_INIT_DELAY] = "";
        (the_output)().alt_values[OUTPUT_IDLE_TIME] = "";
        (the_output)().alt_values[OUTPUT_REPEAT] = "";
        break;
    }
}

#if defined(THORS_ARSENAL)
#include "thors_output_ranger.h"
#elif defined(BONKULATOR)
#include "mcp_output_ranger.h"
#else
#error "Unknown PRODUCT_NAME"
#endif

update_fxn output_update_fxns[NUM_OUTPUT_PARAMS] = {
    update_wave,
    nullptr,
    update_active_time,
    nullptr,
    nullptr,
    update_trigger,
    update_trigger,
    update_trigger,
    update_trigger,
    update_cv0,
    update_cv1,
    output_update_range,
    update_waveform_params,
    update_waveform_params,
    update_phase,
    update_waveform,
    nullptr,
    update_idle_value,
    update_clock,
    update_trig_ctrl,
    update_trig_ctrl_vals};

void outputs_begin()
{
    int output_memory = selected_output.get();
    Greenface_gadget *selected_fxn_memory = selected_fxn;
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        output_units[i][OUTPUT_SCALE] = "%";
        output_units[i][OUTPUT_OFFSET] = "V";
        output_units[i][OUTPUT_PHASE] = "%";
        output_units[i][OUTPUT_RANDOMNESS] = "%";
        output_units[i][OUTPUT_IDLE_VALUE] = "V";
        output_units[i][OUTPUT_ACTIVE_TIME] = "ms";
        output_units[i][OUTPUT_INIT_DELAY] = "ms";
        output_units[i][OUTPUT_IDLE_TIME] = "ms";
    }

    output_is_slider[OUTPUT_SCALE] = true;
    output_is_slider[OUTPUT_OFFSET] = true;
    output_is_slider[OUTPUT_PHASE] = true;
    output_is_slider[OUTPUT_RANDOMNESS] = true;
    output_is_slider[OUTPUT_IDLE_VALUE] = true;

    output_cmd[OUTPUT_SCALE] = "S";
    output_cmd[OUTPUT_OFFSET] = "O";
    output_cmd[OUTPUT_PHASE] = "P";
    output_cmd[OUTPUT_RANDOMNESS] = "R";
    output_cmd[OUTPUT_ACTIVE_TIME] = "A";
    output_cmd[OUTPUT_IDLE_VALUE] = "V";
    output_text_begin();
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        Greenface_gadget *output = bonk_outputs[i];
        output->begin();
        output->string_param_init_vals = output_string_param_init_vals;
        output->string_params = output_string_params;
        output->update_fxns = output_update_fxns;
        output->units = output_units[i];
        output->is_slider = output_is_slider;
        output->cmd = output_cmd;
        // (*bonk_outputs[output]).offsets[OUTPUT_IDLE_VALUE] = idle_value_offsets[2] / 2;
        output->offsets = output_offsets[i];
        output->decimal_places = decimal_places;
        output->display_fxn = &output_display;
        output->alt_values = alt_values[i];
        output->text = output_text;
        output->text_extra = output_text_extra;
        outputs[i].active_time = output->get_param(OUTPUT_ACTIVE_TIME);

        selected_output.put(i); // required by update_ fxns
        selected_fxn = bonk_outputs[i];
        update_idle_value();
        update_trig_ctrl();
        update_cv0();
        update_cv1();
        update_clock();
        update_output_ranges(output);
        update_phase();
        set_phase_value(i);

        // update_repeat_count();
        for (int j = 0; j < 4; j++)
        {
            output->param_num = OUTPUT_ENABLE_T0 + j;
            update_trigger();
        }
        output->param_num = OUTPUT_WAVEFORM;
    }
    selected_fxn = selected_fxn_memory;
    selected_output.put(output_memory);
    // update_clock();
    // update_trig_ctrl();
}

void check_cv_inputs()
{
    for (int output = 0; output < NUM_OUTPUTS; output++)
    {
        if ((*bonk_outputs[output]).get_param(OUTPUT_CV0) > 0 && (*bonk_outputs[output]).triggered)
        {
            cv_set(OUTPUT_CV0, output, adc.read(0));
        }
        if ((*bonk_outputs[output]).get_param(OUTPUT_CV1) > 0 && (*bonk_outputs[output]).triggered)
        {
            cv_set(OUTPUT_CV1, output, adc.read(1));
        }
    }
}

void check_index()
{
    if (selected_fxn->triggered) //  && selected_fxn->get_param(OUTPUT_ACTIVE_TIME) > 511
    {
        OutputData *outptr = &outputs[selected_output.get()];
        send_item_to_USB("current_index", String(outptr->current_index * 128 / outptr->data_len));
        // ui.terminal_debug(selected_fxn->name + " Data len: " + String(outptr->data_len));
    }
}

String check_outputs()
{
    bool output_in_use;
    for (int output = 0; output < NUM_OUTPUTS; output++)
    {
        output_in_use = false;
        for (int trig = 0; trig < 4; trig++)
        {
            if ((*bonk_outputs[output]).get_param(OUTPUT_ENABLE_T0 + trig) > 0 && triggers[trig]->state == TRIGGER_ACTIVE)
            {
                output_in_use = true;
            }
        }
        if (!output_in_use)
        {
            (*bonk_outputs[output]).triggered = false;
        }
    }
    String out = "";
    String t = "     ";
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        if ((*bonk_outputs[i]).get_param(OUTPUT_CLOCK) == 1)
        {
            t = " X" + String(bonk_output(i).triggered_by) + "  ";
        }
        else
        {
            t = "     ";
            if ((*bonk_outputs[i]).triggered)
            {
                t = " T" + String(bonk_output(i).triggered_by) + "  ";
            }
        }
        out += String(i) + ":" + t;
    }
    return out;
}
