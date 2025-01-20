// output range code starts here
void output_ranges_begin()
{
    // ui.terminal_debug("Thor's Arsenal output_ranges_begin.");
}

void output_ranges_init()
{
}

void update_output_ranges(Greenface_gadget *output)
{
    // ui.terminal_debug("Thor's Arsenal output_update_ranges.");
}

void set_output_range(uint8_t output, output_range_type output_range)
{
    uint16_t new_scale = output_range == OUTPUT_RANGE_MINUS_TEN_TO_TEN ? 200 : 150;
    uint16_t new_offset = (output_range == OUTPUT_RANGE_ZERO_TO_TEN ? 500 : 0) - OUTPUT_OFFSET_OFFSET_UI;
    // ui.terminal_debug("Thor's Arsenal set_output_range. Output: " + String(output) + " Range: " + String(output_range) + " Scale: " + String(new_scale) + " Offset: " + String(new_offset));
    (*bonk_outputs[output]).param_put(new_scale, OUTPUT_SCALE);
    (*bonk_outputs[output]).param_put(new_offset, OUTPUT_OFFSET);

    // ui.terminal_debug("Thor's Arsenal set_output_range. Range cmd: " + String(output_range));
    // Serial.println("Output Scale: " + String((*bonk_outputs[output]).get_param(OUTPUT_SCALE)) + " Offset: " + String((*bonk_outputs[output]).get_param(OUTPUT_OFFSET)));
}

void output_update_range()
{
    int output = selected_output.get();
    uint16_t range_type = (the_output)().get_param(OUTPUT_RANGE);
    // ui.terminal_debug("Thor's Arsenal output_update_range. Output: " + String(output) + " Range: " + String(range_type));
    set_output_range(output, (output_range_type)range_type);
    update_waveform();
}
