#include <Wire.h>

enum
{
    OUTPUT_CAL_OUTPUT_NUM,
    OUTPUT_CAL_OFFSET,
    OUTPUT_CAL_SCALE,
    OUTPUT_CAL_STATE,
    OUTPUT_CAL_NUM_PARAMS
};

#define OUTPUT_CAL_RANGE OUTPUT_RANGE_PLUS_MINUS_FIVE

uint16_t _output_cal_params[OUTPUT_CAL_NUM_PARAMS];
uint16_t _output_cal_mins[] = {0, 0, 0, 0};
uint16_t _output_cal_maxs[] = {NUM_OUTPUTS - 1, DAC_FS, 0, 1};
uint16_t _output_cal_init_vals[] = {0, DAC_MID, 0, 0};
uint16_t *output_cal_stuff[] = {_output_cal_params, _output_cal_mins, _output_cal_maxs, _output_cal_init_vals};
String output_cal_labels[] = {"Output: ", "Offset: ", "Scale: ", "Calibrated: "};
String output_cal_string_params[] = {"", "", NOT_AVAILABLE, "No ,Yes"};
int16_t output_cal_offsets[] = {0, -DAC_MID, 0, 0}; // allows negative numbers

Greenface_gadget output_cal_fxn("Calibrate", output_cal_labels, output_cal_stuff, sizeof(_output_cal_params) / sizeof(_output_cal_params[0]));

// output corrections
EEPROM_ArrInt output_scale_corrections(dac.scale_corrections, NUM_OUTPUTS);
EEPROM_ArrInt output_offset_corrections(dac.offset_corrections, NUM_OUTPUTS);
EEPROM_String output_calibrated(NUM_OUTPUTS);

bool output_cal_msg_sent = false;

// void (*func_ptr[3])() = {fun1, fun2, fun3};
bool in_output_cal_fxn()
{
    return selected_fxn == &output_cal_fxn;
}

int get_raw_output_offset_correction(int output)
{
    // ui.terminal_debug("Get raw offset correction: for: " + String(output) + " val: " + String(dac.offset_corrections[output]));
    return dac.offset_corrections[output];
}

int get_output_offset_correction(int output)
{
    return output_offset_corrections.get(output) + DAC_MID;
}

int get_output_scale_correction(int output)
{
    // Serial.println("Get scale correction: for: " + String(output));
    return output_scale_corrections.get(output);
}

bool get_output_calibrated(int output)
{
    // ui.terminal_debug("Get output calibrated: output: " + String(output) + " val: " + output_calibrated.get());
#ifdef BONKULATOR
    return output_calibrated.charAt(output) == 'Y';
#elif defined(THORS_ARSENAL) || defined(FAFNIRS_FIRE)
    return true;
#endif
}

// transfer values from EEPROM to parameter fields
void get_output_corrections()
{
    int output = output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM);
    output_cal_fxn.param_put(get_output_offset_correction(output), OUTPUT_CAL_OFFSET);
    output_cal_fxn.param_put(get_output_scale_correction(output), OUTPUT_CAL_SCALE);
    output_cal_fxn.param_put(get_output_calibrated(output) ? 1 : 0, OUTPUT_CAL_STATE);
}

void set_output_offset_correction(int val)
{
    int output = output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM);
    // Serial.println("Set offset correction: " + String(val) + " for: " + String(output));
    output_offset_corrections.put(val, output);
    update_waveform();
}

void set_output_scale_correction(int val)
{
    int output = output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM);
    // ui.terminal_debug("Set scale correction: " + String(val) + " for: " + String(output));
    output_scale_corrections.put(val, output);
    update_waveform();
}

void set_output_calibrated(int val)
{
    int output = output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM);
    output_calibrated.putCharAt(val == 1 ? 'Y' : 'N', output);
    // Serial.println("Set output calibrated: " + String(val) + " for: " + String(output));
}

void output_cal_update_offset()
{
    int offset = output_cal_fxn.get_param_w_offset(OUTPUT_CAL_OFFSET);
    // ui.terminal_debug("Offset correction: " + String(offset));
    set_output_offset_correction(offset);
    dac.out(output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM), DAC_MID + offset);
}

void output_cal_update_scale()
{
    int scale = output_cal_fxn.get_param(OUTPUT_CAL_SCALE);
    int output = output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM);

    dac.volts_out(5.123, output, output_is_bipolar(output)); // adjust for output = 5.00VDC
    // Serial.println("Scale correction: " + String(scale) + " DAC: " + String(5.123 * mult) + " param #: " + String(selected_fxn->param_num));
    set_output_scale_correction(1000);
}

void output_cal_update_state()
{
    int state = output_cal_fxn.get_param(OUTPUT_CAL_STATE);
    // Serial.println("Output calibrated: " + String(state) + " param #: " + String(selected_fxn->param_num));
    set_output_calibrated(state);
    send_globals_to_USB();
}

void output_cal_update_params()
{
    // ui.terminal_debug("Update params: ");
    set_output_range(output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM), OUTPUT_CAL_RANGE);
    get_output_corrections(); // sets params for selected output from EEPROM
    output_cal_fxn.default_display();
}

update_fxn output_cal_update_fxns[OUTPUT_CAL_NUM_PARAMS] = {output_cal_update_params, output_cal_update_offset, output_cal_update_scale, output_cal_update_state};

void output_cal_display()
{
    // ui.terminal_debug("output_cal_display param_num: " + String(output_cal_fxn.param_num));
    // force bipolar output
    set_output_range(output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM), OUTPUT_CAL_RANGE);
    output_cal_msg_sent = false;
    output_cal_fxn.default_display();
    switch (output_cal_fxn.param_num)
    {
    case OUTPUT_CAL_OFFSET:
        output_cal_update_offset();
        break;
    case OUTPUT_CAL_SCALE:
        output_cal_update_scale();
        break;
    }
    output_cal_fxn.default_display();
}

void output_corrections_init()
{
    output_scale_corrections.fill(IDEAL_SCALE_CORRECTION);
    output_scale_corrections.init();
    output_offset_corrections.fill(0);
    output_offset_corrections.init();
    output_calibrated.init();
    output_calibrated.put("NNNNNNNN");
    get_output_corrections();
}

void output_cal_init()
{
    output_cal_fxn.init();
}

void output_cal_begin()
{
    output_scale_corrections.begin(false);
    output_scale_corrections.xfer();
    output_offset_corrections.begin(false);
    output_offset_corrections.xfer();
    output_calibrated.begin(false);
    output_calibrated.xfer();

    output_cal_fxn.begin();
    output_cal_fxn.string_params = output_cal_string_params;
    output_cal_fxn.offsets = output_cal_offsets;
    output_cal_fxn.update_fxns = output_cal_update_fxns;
    output_cal_fxn.display_fxn = &output_cal_display;
}

void output_cal_params_macro()
{
    // print out code to get to cal fxn
    String prefix = "*\r\np8\r\n1\r\n!";

    Greenface_gadget *fxn = &output_cal_fxn;
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        output_cal_fxn.param_put(i, OUTPUT_CAL_OUTPUT_NUM);
        output_cal_update_params();
        output_cal_update_offset();
        output_cal_update_scale();
        output_cal_update_state();
        gen_params_macro(fxn, i == 0, prefix);
    }
    outputs_begin();
    Serial.println("*");
}

String check_output_cal()
{
    String out = "Adjust offset to measure 0.0VDC at Output: " + String(output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM));
    if (!output_cal_msg_sent)
    {
        send_msg_to_USB(out);
        output_cal_msg_sent = true;
    }
    return out;
}