#include <Wire.h>

enum
{
    BOUNCE_OUTPUT,
    BOUNCE_REPEAT,
    BOUNCE_ENABLE_T0,
    BOUNCE_ENABLE_T1,
    BOUNCE_ENABLE_T2,
    BOUNCE_ENABLE_T3,
    BOUNCE_SCALE,
    BOUNCE_OFFSET,
    BOUNCE_SAMPLE_TIME,
    BOUNCE_TRIG_CTRL,
    BOUNCE_TRIG_CTRL_VAL,
    BOUNCE_NUM_PARAMS,
};

#define OUTPUT_CV_TYPES "Off,Scale,Offset,Sample Time,Trigger"

uint16_t _bounce_mins[] = {0, 0, 0, 0, 0, 0, 0, 0, 10, TRIG_HOLDOFF, 0};
uint16_t _bounce_maxs[] = {NUM_OUTPUTS - 1, 1, 1, 1, 1, 1, 200, OUTPUT_OFFSET_MAX_UI, 65535, TRIG_DENSITY, MAX_MS};
uint16_t _bounce_init_vals[] = {0, 0, 0, 0, 0, 0, 200, OUTPUT_OFFSET_INIT, 100, TRIG_HOLDOFF, 5};
String bounce_string_params[] = {"", "No,Yes", TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, "", "", "", "Hold_Off,Skip,Density", ""};
String bounce_labels[] = {"Output: ", "Repeat: ", "T0: ", "T1: ", "T2: ", "T3: ", "Scale: ", "Offset: ", "SampleTime: ", "Trig Control: ", "Trig Holdoff: "};
// int16_t bounce_offsets[] = {0, BOUNCE_OFFSET_CORRECTION, 0}; // allows negative numbers
// bool bounce_param_active[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

uint16_t _bounce0_params[BOUNCE_NUM_PARAMS];
uint16_t *bounce0_stuff[] = {_bounce0_params, _bounce_mins, _bounce_maxs, _bounce_init_vals};
Greenface_gadget bounce0_fxn("Bounce 0", bounce_labels, bounce0_stuff, sizeof(_bounce0_params) / sizeof(_bounce0_params[0]));

uint16_t _bounce1_params[BOUNCE_NUM_PARAMS];
uint16_t *bounce1_stuff[] = {_bounce1_params, _bounce_mins, _bounce_maxs, _bounce_init_vals};
Greenface_gadget bounce1_fxn("Bounce 1", bounce_labels, bounce1_stuff, sizeof(_bounce1_params) / sizeof(_bounce1_params[0]));

int8_t bounce_decimal_places[] = {0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0}; // allows fixed point numbers

int16_t input_offsets0[] = {0, 0, 0, 0, 0, 0, OUTPUT_SCALE_OFFSET, OUTPUT_OFFSET_OFFSET_UI, 0, 0, 0}; // allows negative numbers
int16_t input_offsets1[] = {0, 0, 0, 0, 0, 0, OUTPUT_SCALE_OFFSET, OUTPUT_OFFSET_OFFSET_UI, 0, 0, 0}; // allows negative numbers

int16_t *input_offsets[NUM_INPUTS] =
    {
        input_offsets0,
        input_offsets1,
};

Greenface_gadget *bounce_fxns[NUM_INPUTS] = {
    &bounce0_fxn,
    &bounce1_fxn,
};

String bounce_cmd[BOUNCE_NUM_PARAMS] = {};
bool bounce_is_slider[BOUNCE_NUM_PARAMS] = {};
String bounce_units[BOUNCE_NUM_PARAMS] = {};

String bounce_text[BOUNCE_NUM_PARAMS] = {};

// Function to initialize the bounce_text array
void initializeBounceText()
{
    bounce_text[BOUNCE_OUTPUT] = "When triggered, the input signal from CV {param_name_num} will be 'bounced' to Output {param} after being scaled and offset by the Scale and Offset parameters.";
    bounce_text[BOUNCE_REPEAT] = "If 'Yes', the trigger will repeat, taking measurements every number of milliseconds set by the Sample Time parameter. If 'No', one measurement will be taken for each incoming trigger.";
    bounce_text[BOUNCE_ENABLE_T0] = "This Enables or Disables Trigger 0 to trigger {param_name}.";
    bounce_text[BOUNCE_ENABLE_T1] = "This Enables or Disables Trigger 1 to trigger {param_name}.";
    bounce_text[BOUNCE_ENABLE_T2] = "This Enables or Disables Trigger 2 to trigger {param_name}.";
    bounce_text[BOUNCE_ENABLE_T3] = "This Enables or Disables Trigger 3 to trigger {param_name}.";
    bounce_text[BOUNCE_SCALE] = "This is the scale factor for the output signal. A negative scale inverts the signal.<div>Example: For an input signal of 1.0V, a scale of -50% will output -0.5V.</div>";
    bounce_text[BOUNCE_OFFSET] = "This is the offset for the output signal. It is added to the scaled signal.";
    bounce_text[BOUNCE_SAMPLE_TIME] = "This is the number of milliseconds between samples when Repeat is set to 'Yes'.";
    bounce_text[BOUNCE_TRIG_CTRL] = "This determines how trigger signals will be handled. Regardless of this setting, when Repeat is set to 'Yes', each qualified trigger will toggle the repeat cycle.";
    bounce_text[BOUNCE_TRIG_CTRL_VAL] = "This is the value of the selected Trigger Control.";
    bounce_text[BOUNCE_TRIG_CTRL_VAL] += "<div>When set to Hold_Off, the measurement can not be triggered again until the Holdoff time (in milliseconds) has expired.</div>";
    bounce_text[BOUNCE_TRIG_CTRL_VAL] += "<div>When set to Skip, the measurement will be skipped for the selected number of incoming trigger signals before before it can be triggered again.</div>";
    bounce_text[BOUNCE_TRIG_CTRL_VAL] += "<div>When set to Density, each incoming trigger signal will have the selected percentage chance of triggering the measurement.</div>";
}

int bounce_input()
{
    return selected_fxn == &bounce0_fxn ? 0 : 1;
}

void bounce_init()
{
    for (int i = 0; i < NUM_INPUTS; i++)
    {
        selected_fxn = bounce_fxns[i]; // because update fxns use selected_fxn
        (*bounce_fxns[i]).init();
    }
}

bool get_bounce_triggered(int input)
{
    return (*bounce_fxns[input]).triggered;
}

void set_bounce_triggered(int input, bool triggered)
{
    (*bounce_fxns[input]).triggered = triggered;
}

bool get_bounce_repeat(int input)
{
    return (*bounce_fxns[input]).get_param(BOUNCE_REPEAT);
}

bool in_bounce_fxn()
{
    return selected_fxn == &bounce0_fxn || selected_fxn == &bounce1_fxn;
}

void bounce_set_trig_ctrls(TRIGGER *trig)
{
    // Serial.println("bounce_set_trig_ctrls: " + String(bounce_input()));
    trig->set_trig_ctrls(bounce_input() + 8, selected_fxn->get_param(BOUNCE_TRIG_CTRL), selected_fxn->get_param(BOUNCE_TRIG_CTRL_VAL));
}

void bounce_update_trig_ctrl_vals()
{
    for (int i = 0; i < NUM_TRIGGERS; i++)
    {
        bounce_set_trig_ctrls(triggers[i]);
    }
}

void bounce_update_trig_ctrl()
{
    switch (selected_fxn->get_param(BOUNCE_TRIG_CTRL))
    {
    case TRIG_HOLDOFF:
        selected_fxn->labels[BOUNCE_TRIG_CTRL_VAL] = F("Trig Holdoff: ");
        selected_fxn->maxs[BOUNCE_TRIG_CTRL_VAL] = MAX_MS;
        break;
    case TRIG_SKIP:
        selected_fxn->labels[BOUNCE_TRIG_CTRL_VAL] = F("Trig Skip: ");
        selected_fxn->maxs[BOUNCE_TRIG_CTRL_VAL] = MAX_MS;
        break;
    case TRIG_DENSITY:
        selected_fxn->labels[BOUNCE_TRIG_CTRL_VAL] = F("Trig Density: ");
        selected_fxn->maxs[BOUNCE_TRIG_CTRL_VAL] = 100; // 100%
        break;
    }
    // now set value in case the max was changed
    selected_fxn->param_put(selected_fxn->get_param(BOUNCE_TRIG_CTRL_VAL), BOUNCE_TRIG_CTRL_VAL);
    bounce_update_trig_ctrl_vals();
}

void bounce_update_trigger()
{
    int input = selected_fxn == &bounce0_fxn ? 0 : 1;
    int channel = input + NUM_OUTPUTS;
    int trig_num = selected_fxn->param_num - BOUNCE_ENABLE_T0;
    int trig_enable = selected_fxn->get_param(); // enabled or disabled?

    // ui.terminal_debug("Selected input: " + String(input) + " trig num: " + String(trig_num) + " trig enabled: " + String(trig_enable));
    if (triggers[trig_num]->state == 'TRIGGER_ACTIVE')
        set_bounce_triggered(input, trig_enable); // stop meas if active
    triggers[trig_num]->set_channel(channel, trig_enable);
}

update_fxn bounce_update_fxns[BOUNCE_NUM_PARAMS] = {
    nullptr,
    nullptr,
    bounce_update_trigger,
    bounce_update_trigger,
    bounce_update_trigger,
    bounce_update_trigger,
    nullptr,
    nullptr,
    nullptr,
    bounce_update_trig_ctrl,
    bounce_update_trig_ctrl_vals,
};

void bounce_begin()
{
    bounce_units[BOUNCE_SCALE] = "%";
    bounce_units[BOUNCE_OFFSET] = "V";
    bounce_units[BOUNCE_SAMPLE_TIME] = "ms";
    bounce_is_slider[BOUNCE_SCALE] = true;
    bounce_is_slider[BOUNCE_OFFSET] = true;
    bounce_cmd[BOUNCE_SCALE] = "S";
    bounce_cmd[BOUNCE_OFFSET] = "O";
    initializeBounceText();
    for (int i = 0; i < 2; i++)
    {
        (*bounce_fxns[i]).begin();
        (*bounce_fxns[i]).string_params = bounce_string_params;
        (*bounce_fxns[i]).update_fxns = bounce_update_fxns;
        (*bounce_fxns[i]).units = bounce_units;
        (*bounce_fxns[i]).is_slider = bounce_is_slider;
        (*bounce_fxns[i]).cmd = bounce_cmd;
        (*bounce_fxns[i]).offsets = input_offsets[i];
        (*bounce_fxns[i]).text = bounce_text;
        (*bounce_fxns[i]).decimal_places = bounce_decimal_places;
        // (*bounce_fxns[i]).active_params = bounce_param_active;
        selected_fxn = bounce_fxns[i];
        for (int j = 0; j < NUM_TRIGGERS; j++)
        {
            selected_fxn->param_num = BOUNCE_ENABLE_T0 + j;
            bounce_update_trigger();
        }
    }

    // bounce0_fxn.begin();
    // bounce0_fxn.string_params = bounce_string_params;
    // bounce0_fxn.update_fxns = bounce_update_fxns;

    // bounce1_fxn.begin();
    // bounce1_fxn.string_params = bounce_string_params;
    // bounce1_fxn.update_fxns = bounce_update_fxns;

    // bounce_fxn.offsets = bounce_offsets;
    // bounce_fxn.update_fxns = bounce_update_fxns;
    // bounce_fxn.display_fxn = &bounce_display;
}

void reset_inputs(int trig_num)
{
    (*triggers[trig_num]).state = TRIGGER_IDLE;
    for (int i = 0; i < NUM_INPUTS; i++)
    {
        if ((*bounce_fxns[i]).get_param(BOUNCE_ENABLE_T0 + trig_num))
        {
            set_bounce_triggered(i, false);
        }
    }
}

// clear leds and triggers if not in use
void check_bounce_triggers()
{
    bool in_use;
    for (int trig_num = 0; trig_num < 4; trig_num++)
    {
        in_use = false;
        for (int input = 0; input < NUM_INPUTS; input++)
        {
            bool this_trig_enabled = (*bounce_fxns[input]).get_param(BOUNCE_ENABLE_T0 + trig_num);
            if (this_trig_enabled && get_bounce_triggered(input))
            {
                in_use = true;
            }
        }
        if (!in_use)
        {
            led_write(trig_num, LOW);
            reset_inputs(trig_num);
            // Serial.println("Turning off LED " + String(trig_num));
        }
    }
}

// called on interrupt
volatile byte bounce_triggered_by[] = {0, 0};
volatile int saved_bounce_param;
volatile bool bounce_repeating;
void trigger_bounce(byte trig_num, int input_num)
{
    bool in_repeat_mode = get_bounce_repeat(input_num);
    bool toggle_trig = !in_repeat_mode;
    if (bounce_repeating && in_repeat_mode)
    {
        toggle_trig = true;
    }
    (*bounce_fxns[input_num]).trigger(trig_num, toggle_trig);

    bounce_triggered_by[input_num] = trig_num;

    led_write(trig_num, HIGH);
    saved_bounce_param = (*bounce_fxns[input_num]).param_num;
    bounce_repeating = in_repeat_mode;
}

String check_bounce()
{
    int bounce_reading;
    uint16_t the_bounce_input = bounce_input();
    bool bounce_triggered = get_bounce_triggered(the_bounce_input);
    bool terminal_mirror_temp = ui.terminal_mirror;

    static uint bounce_next_time = 0;
    // static bool display_shown = true;
    // display_shown = bounce_triggered && !display_shown;

    send_trig_status_to_USB();
    check_bounce_triggers();

    if (bounce_triggered && millis() > bounce_next_time)
    {
        // display_shown = bounce_triggered;
        bounce_reading = adc.read(the_bounce_input);
        float reading = adc.adc_to_volts(bounce_reading);

        send_msg_to_USB("<div id='bounce_reading' class='big_reading'><div>Reading:</div><div class='reading'>" + String(reading) + "V</div></div>Triggered by: T" + String(bounce_triggered_by[the_bounce_input]));

        char formattedString[10]; // Character array to hold the formatted string
        // Use sprintf to format the string
        sprintf(formattedString, "%6.2fV ", reading);
        // ui.terminal_clrDown("7");
        // ui.fill(0, ui.lines[0]);
        // ui.printLine(formattedString, ui.lines[1], 2);
        ui.terminal_mirror = false;
        ui.printLine(formattedString, 0, 2);
        ui.terminal_mirror = terminal_mirror_temp;

        uint16_t output_num = calc_output((*bounce_fxns[the_bounce_input]).get_param(BOUNCE_OUTPUT));
        float output_scale = float((*bounce_fxns[the_bounce_input]).get_param_w_offset(BOUNCE_SCALE)) / 100.0;
        float output_offset = float((*bounce_fxns[the_bounce_input]).get_param_w_offset(BOUNCE_OFFSET)) * 1.916; // 20.48
        int bounce_offset_correction = get_raw_output_offset_correction(output_num);
        uint16_t dac_val = (bounce_reading * output_scale * 1.16) + DAC_MID + output_offset + bounce_offset_correction;
        dac.out(output_num, dac_val);

        bounce_next_time = millis() + selected_fxn->get_param(BOUNCE_SAMPLE_TIME);

        // ui.terminal_debug(" Reading: " + String(bounce_reading) + " DAC val: " + String(dac_val) + " Offset: " + String(output_offset) + " corr: " + bounce_offset_correction);
        // ui.terminal_debug("Bounce repeat: " + String(selected_fxn->get_param(BOUNCE_REPEAT)) + " saved param: " + String((*bounce_fxns[the_bounce_input]).param_num == saved_bounce_param) + " triggered: " + String((*bounce_fxns[the_bounce_input]).triggered));

        // if (!get_bounce_repeat(the_bounce_input) || (*bounce_fxns[the_bounce_input]).param_num != saved_bounce_param)
        if (!get_bounce_repeat(the_bounce_input))
        {
            set_bounce_triggered(the_bounce_input, false);
            if (get_bounce_repeat(the_bounce_input))
                (*bounce_fxns[the_bounce_input]).default_display();
        }
        return "  CV" + String(the_bounce_input) + " Reading:" + String(formattedString) + " Triggered by: T" + String(bounce_triggered_by[the_bounce_input]) + "  ";
    }
    // else if (!display_shown)
    // {
    //     // display_shown = true;
    //     selected_fxn->display();
    // }
    return "";
}
