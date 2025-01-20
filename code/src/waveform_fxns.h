// waveform fxns
#include <Wire.h>
#include <Arduino.h>

void dump_waveform(int output, bool dump_ref)
{
    byte data_len = outputs[output].data_len;
    String out = "";
    if (dump_ref)
    {
        out += ("Reference ");
    }
    out += ("Wave data for Output: " + String(output));
    out += "\n\r";
    for (int i = 0; i < data_len; i++)
    {
        if (i % 8 == 0)
        {
            out += "\n\r";
        }
        if (dump_ref)
        {
            out += (waveform_reference[output][i]);
        }
        else
        {
            out += (outputs[output].waveform_data[i]);
        }
        out += (" ");
    }
    out += "\n\r";
    out += ("Time_inc: " + String(outputs[output].time_inc));
    ui.terminal_debug(out);
}

// void randomize_waveform(int output, int randomness)
// {
//   uint16_t *waveform_data = outputs[output].waveform_data;
//   byte data_len = outputs[output].data_len;
//   for (int i = 0; i < data_len; i++)
//   {
//   }
// }

void apply_params_to_waveform(int output)
{
    Greenface_gadget *the_output = bonk_outputs[output];
    float scale = the_output->get_param_w_offset(OUTPUT_SCALE);
    int offset = the_output->get_param_w_offset(OUTPUT_OFFSET); // volts*100
    int randomness = the_output->get_param(OUTPUT_RANDOMNESS);
    int wave_type = the_output->get_param(OUTPUT_WAVEFORM);
    output_range_type range_type = (output_range_type)the_output->get_param(OUTPUT_RANGE);
    int offset_correction = output_offset_corrections.get(output);
    int offset_adjust = offset * OUTPUT_OFFSET_FACTOR + DAC_MID + 1;

    // ui.terminal_debug("Apply params -- Output: " + String(output) + " scale: " + String(scale) + " offset: " + String(offset) + " randomness: " + String(randomness) + " wave_type: " + String(wave_type) + " range_type: " + String(range_type) + " scale_corr: " + String(scale_correction) + " offset_corr: " + String(offset_correction) + " offset_adj: " + String(offset_adjust));
    // ui.terminal_debug("Apply params -- Output: " + String(output) + " offset: " + String(offset) + " offset_corr: " + String(offset_correction) + " offset_adj: " + String(offset_adjust));

    uint16_t *waveform_data = outputs[output].waveform_data;
    int16_t *waveform_ref = waveform_reference[output];
    byte data_len = outputs[output].data_len;

    float scale_factor;

    switch (wave_type)
    {
    case WAVEFORM_PULSE:
        outputs[output].active_time = the_output->get_param(OUTPUT_ACTIVE_TIME);
        outputs[output].repeat_count = the_output->get_param(OUTPUT_REPEAT);
        scale_factor = scale / 200;
        // scale_factor += .5;
        // set_randomness_factor(randomness, abs(scale - 100) / 100.0, output);
        break;
    case WAVEFORM_TOGGLE:
        outputs[output].active_time = 65535;
        outputs[output].repeat_count = 1;
        scale_factor = scale / 100;
        // offset_factor = (offset / 100.0) * (DAC_FS + 1);
        // offset_adjust = DAC_FS * (100 - scale) / 200;
        // ui.terminal_debug("Apply params -- Output: " + String(output) + " data_len: " + String(data_len));
        break;
    default:
        outputs[output].active_time = the_output->get_param(OUTPUT_ACTIVE_TIME);
        outputs[output].repeat_count = the_output->get_param(OUTPUT_REPEAT);
        scale_factor = scale / 100; // makes output max 10V or 5V
        // ui.terminal_debug("Apply params -- scale: " + String(scale_factor) + " offset: " + String(offset_adjust));
        break;
    }

    set_randomness_factor(randomness, abs(scale) / 100.0, output);

    // ui.terminal_debug("Apply params -- Offset factor: " + String(offset_factor) + " offset: " + String(offset));
    // ui.terminal_debug("Apply params -- Randomness factor: " + String(randomness_factor) + " int: " + String((int)randomness_factor));
    // ui.terminal_debug("Apply params -- Scale factor: " + String(scale_factor) + " scale: " + String((int)scale));
    // ui.terminal_debug("Apply params -- Output: " + String(output) + " offset corr: " + String(offset_correction));

    int temp;

    if (wave_type == WAVEFORM_TOGGLE)
    {
        offset_adjust = offset * scale_correction / 100 * DAC_MID;
        temp = DAC_MID * scale_factor * scale_correction;
        for (int i = 0; i < 2; i++)
        {
            waveform_data[i] = DAC_MID + (i == 0 ? temp : -temp);
            waveform_data[i] = constrain(waveform_data[i] + offset_adjust + offset_correction, 0, DAC_FS);
        }
    }
    else
    {
        for (int i = 0; i < data_len; i++)
        {
            temp = waveform_ref[i] * scale_factor;
            temp *= scale_correction;
            temp += offset_adjust;
            temp += offset_correction;
            waveform_data[i] = constrain(temp, 0, DAC_FS);
        }
    }
}

void recv_user_waveform(String in_str)
{
    // set index
    int waveform_num = get_user_waveform_num();
    (*user_waveforms[waveform_num]).param_put((csv_elem(in_str, ',', 0)).substring(1).toInt(), USER_WAVEFORMS_INDEX);
    for (int i = 0; i < WAVEFORM_PARTS; i++)
    {
        user_waveforms_put_val(waveform_num, (csv_elem(in_str, ',', i + 1)).toInt(), i);
    }
}

String get_waveform_message()
{
    // OutputData *outptr;
    // uint16_t *waveform_ref = waveform_reference[selected_output.get()];
    String waveform_values = "";
    uint16_t *waveform_data;
    byte data_len;
    OutputData *outptr = &outputs[selected_output.get()];

    if (in_output_fxn())
    {
        waveform_data = outptr->waveform_data;
        // waveform_data = waveform_ref;
        data_len = outptr->data_len;
    }
    else if (in_user_waveform_fxn())
    {
        waveform_data = _user_waveforms_data[get_user_waveform_num()];
        data_len = WAVEFORM_PARTS;
    }
    else
    {
        return "";
    }

    int offset_correction = output_offset_corrections.get(selected_output.get());
    // ui.terminal_debug("offset_correction: " + String(offset_correction));
    int index = 0; // index into source waveform data
    float index_inc = WAVEFORM_PARTS / float(data_len);
    int value;
    // int randomness_factor = (the_output)().get_param(OUTPUT_RANDOMNESS);

    for (int i = 0; i < WAVEFORM_PARTS; i++)
    {
        if (i > 0 && i != WAVEFORM_PARTS)
            waveform_values += ", ";

        // undo offset correction if Bonkulator
        value = waveform_data[index] - offset_correction; // should be zero for Thor
        // value = waveform_data[index];
        if (!in_user_waveforms())
            value = randomize_value(value, outptr->randomness_factor);
        value = quantize_value(value, selected_output.get());
        waveform_values += String(value);

        if (i + 1 >= (index + 1) * index_inc)
        {
            index++;
        }
    }
    return waveform_values;
}

void graph_waveform(int output_num)
{
    OutputData *outptr = &outputs[selected_output.get()];
    uint16_t *input_waveform = outputs[output_num].waveform_data;
    uint16_t waveform_data[WAVEFORM_PARTS];
    for (int i = 0; i < outptr->data_len; i++)
    {
        waveform_data[i] = input_waveform[i];
        waveform_data[i] = randomize_value(waveform_data[i], outptr->randomness_factor);
        waveform_data[i] = quantize_value(waveform_data[i], selected_output.get());
    }
    // ui.terminal_debug("Graph waveform - len: " + String(outputs[output_num].data_len));
    ui.graph_waveform(waveform_data, outputs[output_num].data_len, DAC_FS);
}

uint16_t get_waveform_parts(int output)
{
    uint16_t calculated_time_inc = 0;
    // uint16_t active_time = 268;
    uint16_t active_time = (outputs[output]).active_time;
    // uint16_t active_time = (*bonk_outputs[output]).get_param(OUTPUT_ACTIVE_TIME);
    uint16_t leftover = active_time % WAVEFORM_PARTS;
    float ratio = (float)active_time / WAVEFORM_PARTS;
    if (leftover == 0)
    {
        calculated_time_inc = outputs[output].time_inc = active_time / WAVEFORM_PARTS;
        return WAVEFORM_PARTS;
    }
    else if (ratio < 1.0)
    {
        calculated_time_inc = outputs[output].time_inc = 1;
        return active_time;
    }
    else
    {
        calculated_time_inc = outputs[output].time_inc = (int)ratio * 2;
        return active_time / outputs[output].time_inc;
    }
}

uint16_t get_time_inc(int output)
{
    return outputs[output].time_inc;
    // return (*bonk_outputs[output]).get_param(OUTPUT_TIME_INC);
}

// sets phase in degrees
void set_output_phase(int output, int phase)
{
    outputs[output].phase = phase / 100.0;
    set_waveform(output, (*bonk_outputs[output]).get_param(OUTPUT_WAVEFORM));
}

int calc_wave_value(int i, int waveform, uint16_t waveform_parts, float phase)
{
    int temp = 0;
    int temp_index;
    switch (waveform)
    {
    case WAVEFORM_SINE:
        // Serial.println("Sine i: " + String(i) + " angle: " + String(6.28319 * (i + waveform_parts / 4) / waveform_parts));
        temp = (sin(6.28319 * (i + waveform_parts * phase) / waveform_parts) * DAC_MID);
        break;
    case WAVEFORM_HAYSTACK:
        // temp = (sin(3.14159 * (i + waveform_parts * phase) / waveform_parts) * DAC_FS) - DAC_MID;
        temp_index = (int)(i + waveform_parts * phase) % waveform_parts;
        temp = (sin(3.14159 * temp_index / waveform_parts) * DAC_FS) - DAC_MID;
        break;
    case WAVEFORM_RAMP:
        // temp = (i * (DAC_FS + 1) / waveform_parts) - DAC_MID;
        temp_index = (int)(i + waveform_parts * phase) % waveform_parts;
        temp = (temp_index * (DAC_FS + 1) / waveform_parts) - DAC_MID;
        break;
    case WAVEFORM_PYRAMID:
        temp_index = (int)(i + waveform_parts * phase) % waveform_parts;
        temp_index = (temp_index < waveform_parts / 2) ? temp_index : waveform_parts - temp_index;
        temp = (2 * temp_index * (DAC_FS + 1) / waveform_parts) - DAC_MID;
        break;
    case WAVEFORM_PULSE:
        temp = DAC_FS;
        break;
    case WAVEFORM_TOGGLE:
        temp = (i < waveform_parts / 2) ? DAC_FS : 0;
        break;
    case WAVEFORM_MAYTAG:
        temp = random(0, DAC_FS) - DAC_MID;
        break;
    }
    return temp;
}

void set_waveform(int output, int waveform)
{
    int16_t *waveform_ref = waveform_reference[output];
    uint16_t *waveform_data = outputs[output].waveform_data;
    uint16_t waveform_parts = get_waveform_parts(output); // also sets time_inc

    if (waveform == WAVEFORM_TOGGLE)
    {
        waveform_parts = 2;
    }

    int user_wavenum = waveform - NUM_STD_WAVEFORMS;
    EEPROM_Arr16 user_waveform = (*user_waveforms_data[user_wavenum]);

    outputs[output].data_len = waveform_parts;
    float user_inc_factor = (WAVEFORM_PARTS / (float)waveform_parts);

    // Serial.println("Set Waveform output: " + String(output) + " waveform: " + String(waveform));
    // Serial.println("Set Waveform user_inc_factor: " + String(user_inc_factor) + " parts: " + String(waveform_parts) + " userlen: " + String(user_waveform.length()) + " active time: " + String(outputs[output].active_time));
    // int pyramid_index;
    for (int i = 0; i < waveform_parts; i++)
    {
        if (waveform > WAVEFORM_TOGGLE) // is this a user wave?
        {
            waveform_data[i] = waveform_ref[i] = user_waveform.get(user_inc_factor * i) - DAC_MID;
        }
        else
        {
            waveform_data[i] = waveform_ref[i] = calc_wave_value(i, waveform, waveform_parts, outputs[output].phase);
        }
    }
    apply_params_to_waveform(output);
    // dump_waveform(selected_output.get(), false);
    // graph_waveform(selected_output.get());
}
