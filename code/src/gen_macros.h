void gen_params_macro(Greenface_gadget *item, bool print_header = true, String prefix = "")
{
    if (print_header)
    {
        ui.t.clrScreen();
        ui.terminal_debug("// Macro for: " + item->name);
        Serial.println(prefix);
    }
    uint8_t param_type;
    for (int i = 0; i < item->num_params; i++)
    {
        param_type = item->get_param_type(i);
        // Serial.println(item->labels[i] + String(item->get_param_w_offset(i)) + " type: " + String(param_type));
        switch (param_type)
        {
        case SPANK_INT_PARAM_TYPE:
        case SPANK_STRING_PARAM_TYPE:
            Serial.println("p" + String(i));
            Serial.println(String(item->get_param_w_offset(i)));
            break;
        case SPANK_STRING_VAR_TYPE:
            Serial.println("p" + String(i));
            Serial.println("$" + String(item->get_param_as_string_var(i)));
            break;
        }
    }
}
