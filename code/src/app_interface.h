bool sending_to_USB;
bool sending_status = false;
struct SelectedFxnParams
{
    int param_num;
    String name;
    String name_num;
    String param_label;
    String param_label_raw;
    String cv_param_label_raw;
    String param_as_string;
    uint16_t param;
    uint8_t param_index;
};

String extractNumberAsString(const String &input)
{
    String number;
    for (size_t i = 0; i < input.length(); ++i)
    {
        if (isdigit(input[i]))
        {
            number += input[i];
        }
    }
    return number;
}

// Function to initialize the struct
SelectedFxnParams getSelectedFxnParams(Greenface_gadget *fxn = selected_fxn)
{
    SelectedFxnParams params;
    params.name = fxn->name;
    params.name_num = extractNumberAsString(params.name);
    params.param_num = fxn->param_num;
    params.param_label = fxn->get_label(params.param_num);
    params.param_label_raw = params.param_label;
    params.param_label_raw.replace(":", "");
    params.cv_param_label_raw = params.param_label_raw;
    params.cv_param_label_raw.replace("*", "");
    params.param_as_string = fxn->is_available() ? fxn->get_param_as_string() : NOT_AVAILABLE;
    params.param = fxn->get_param_w_offset();
    params.param_index = (fxn->get_param_type() == SPANK_STRING_PARAM_TYPE) ? fxn->get_param() : 0;
    return params;
}

String replace_params(String in_str)
{
    SelectedFxnParams params = getSelectedFxnParams();
    in_str.replace("{param_name}", params.name);
    in_str.replace("{param_name_num}", params.name_num);
    in_str.replace("{param_label}", params.param_label);
    in_str.replace("{param_label_raw}", params.param_label_raw);
    in_str.replace("{cv_param_label_raw}", params.cv_param_label_raw);
    in_str.replace("{param_as_string}", params.param_as_string);
    in_str.replace("{param}", String(params.param));
    return in_str;
}

// "private" fxns
String globals_toJSON()
{
    String out = "";
    out += (list_fxns());
    out += (toJSON("product_name", PRODUCT_NAME));
    out += (",");
    out += (toJSON("num_outputs", String(NUM_OUTPUTS)));
    out += (",");
    out += (toJSON("software_version", settings_get_version()));
    // out += (",");
    // out += (toJSON("dac_fs", String(PRODUCT_NAME == "Bonkulator" ? 3975 : dac.volts_to_dac(10.0, false))));
    return out;
}

String settings_to_string()
{
    SelectedFxnParams params = getSelectedFxnParams();

    String out = "<div id='settings_system_message_div'>";
    out += replace_params(selected_fxn->text[params.param_num]);
    out += "</div>";
    return out;
}

String ask_init_to_string()
{
    SelectedFxnParams params = getSelectedFxnParams();

    String out = "<div id='settings_system_message_div'>";
    out += "<div><strong>Reset " + params.param_as_string + "</strong></div>";
    out += replace_params(selected_fxn->text[params.param_num]);
    out += "</div>";
    return out;
}

String output_params_to_string()
{
    SelectedFxnParams params = getSelectedFxnParams();

    String out = "<div id='output_params_message_div'>";
    out += "<div><h2>" + params.param_label + "<span>" + params.param_as_string + "</span></h2></div>";

    if (!selected_fxn->is_available())
    {
        out += "<div>You cannot set the " + params.param_label_raw + " in this configuration.</div>";
        return out;
    } // else

    if (selected_fxn->text)
    {
        out += replace_params(selected_fxn->text[params.param_num]);
    }
    if (selected_fxn->text_extra)
    {
        out += replace_params(selected_fxn->text_extra[params.param_num][params.param_index]);
    }
    // out += "Param #: " + String(params.param_num) + " Param: " + String(params.param);
    // out += (selected_fxn->text_extra[params.param_num][params.param]);
    out += "</div>";
    return out;
}

String bounce_params_to_string()
{
    SelectedFxnParams params = getSelectedFxnParams();
    String out_temp = replace_params(selected_fxn->text[params.param_num]);
    String out = "<div id='bounce_params_message_div'>";
    out += "<div><h2>" + params.param_label + "<span>" + params.param_as_string + "</span></h2></div>";
    out += out_temp;
    out += "</div>";
    return out;
}

String messages_toJSON()
{
    String waveform_values = "";
    String out = "";
    if (selected_fxn_num != SETTINGS_FXN || in_user_waveforms())
    {
        waveform_values = get_waveform_message();
        out += (toJSON("waveform_values", waveform_values));
        out += (",");
        out += (toJSON("system_message", output_params_to_string()));
        out += (",");
        out += (toJSON("debug", system_message_debug));
        out += (",");
    }
    else
    {
        out += (toJSON("waveform_values", ""));
        out += (",");
    }
    waveform_values = "";

    if (in_wifi())
    {
        out += (wifi_toJSON());
    }
    else if (in_settings())
    {
        out += (toJSON("system_message", settings_to_string()));
        out += (",");
    }
    else if (in_ask_init())
    {
        out += (toJSON("system_message", ask_init_to_string()));
        out += (",");
    }
    else if (in_bounce_fxn())
    {
        out += (toJSON("system_message", bounce_params_to_string()));
        out += (",");
    }
    return out;
}

String triggers_toJSON()
{
    String out = "";
    out += ("\"triggers\" : [");
    for (int trig_num = 0; trig_num < NUM_TRIGGERS; trig_num++)
    {
        if (trig_num > 0)
        {
            out += (",");
        }
        // ui.terminal_debug((*triggers[trig_num]).params_toJSON());
        out += ((*triggers[trig_num]).params_toJSON());
    }
    out += ("],");
    return out;
}

String fxn_params_toJSON(char cmd)
{
    String out = "";
    String slider_label = "";
    if (in_bounce_fxn())
    {
        slider_label = "Output";
    }
    else if (in_output_fxn())
    {
        slider_label = "Waveform";
    }
    else
    {
        slider_label = selected_fxn->name;
    }

    out += (toJSON("cmd", String(cmd)));
    out += (",");
    out += (toJSON("fxn", selected_fxn->name));
    out += (",");
    out += (toJSON("fxn_num", String(remembered_fxn.get())));
    out += (",");
    out += (toJSON("device_name", settings_get_device_name()));
    out += (",");
    out += (toJSON("group", String(group)));
    out += (",");
    out += (toJSON("group_active", String(group_active)));
    out += (",");
    out += (toJSON("param_num", String(selected_fxn->param_num)));
    out += (",");
    out += (toJSON("param_active", String(selected_fxn->get_param_active())));
    out += (",");
    out += (toJSON("dp", String(selected_fxn->get_decimal_places())));
    out += (",");
    out += (toJSON("rotary_leds_recording_state", String(rotary_leds_recording_state)));
    out += (",");
    out += (toJSON("slider_label", slider_label));
    out += (",");
    out += ("\"params\" : [");
    if (!in_wifi())
        out += (selected_fxn->params_toJSON());
    out += ("]");
    return out;
}

// "public" fxns
void send_item_to_USB(String item_name, String item_value)
{
    if (sending_to_USB || !usb_direct_enabled())
        return;
    sending_to_USB = true;
    ui.t.print("{");
    ui.t.print(toJSON("item", " { " + toJSON("item_name", item_name) + ", " + toJSON("item_value", item_value) + " }"));
    ui.t.print("}");
    // This terminates serialport message
    ui.t.println("\r\n\r\n");
    sending_to_USB = false;
}

void send_ok_to_USB(char cmd)
{
    if (sending_to_USB || !usb_direct_enabled())
        return;
    sending_to_USB = true;
    ui.t.print("{");
    ui.t.print(toJSON("reply", String(cmd)));
    ui.t.print("}");
    // This terminates serialport message
    ui.t.println("\r\n\r\n");
    sending_to_USB = false;
}

void send_globals_to_USB()
{
    if (sending_to_USB || !usb_direct_enabled())
        return;
    sending_to_USB = true;
    ui.t.print("{");
    ui.t.print(globals_toJSON());
    ui.t.print("}");
    // This terminates serialport message
    ui.t.println("\r\n\r\n");
    sending_to_USB = false;
}

void send_data_to_USB(char cmd)
{
    if (sending_to_USB || !usb_direct_enabled() || cmd == '[')
        return;
    sending_to_USB = true;
    ui.t.print("{");
    // ui.t.print(globals_toJSON());
    ui.t.print(messages_toJSON());
    ui.t.print(triggers_toJSON());
    ui.t.print(fxn_params_toJSON(cmd));

    ui.t.print("}");

    // This terminates serialport message
    ui.t.println("\r\n\r\n");
    sending_to_USB = false;
}

void send_msg_to_USB(String msg)
{
    if (sending_to_USB || !usb_direct_enabled())
        return;
    sending_to_USB = true;
    ui.t.print("{");
    String out = "";
    out += (toJSON("msg", msg));
    ui.t.print(out);
    ui.t.print("}");

    // This terminates serialport message
    ui.t.println("\r\n\r\n");
    sending_to_USB = false;
}

void send_triggers_to_USB()
{
    if (!sending_to_USB && usb_direct_enabled())
    {
        sending_status = true;
        sending_to_USB = true;
        ui.t.print("{");
        ui.t.print(toJSON("event", "trigger"));
        ui.t.print(",");
        ui.t.print(toJSON("t0", trig0.state == TRIGGER_ACTIVE ? "ON" : "OFF"));
        ui.t.print(",");
        ui.t.print(toJSON("t1", trig1.state == TRIGGER_ACTIVE ? "ON" : "OFF"));
        ui.t.print(",");
        ui.t.print(toJSON("t2", trig2.state == TRIGGER_ACTIVE ? "ON" : "OFF"));
        ui.t.print(",");
        ui.t.print(toJSON("t3", trig3.state == TRIGGER_ACTIVE ? "ON" : "OFF"));
        ui.t.print("}");
        // This terminates serialport message
        ui.t.println("\r\n\r\n");
        sending_to_USB = false;
        sending_status = false;
    }
}

#define TRIGGER_EVENT 1
void send_status_to_USB(int event = TRIGGER_EVENT)
{
    switch (event)
    {
    case TRIGGER_EVENT:
        send_triggers_to_USB();
        break;

    default:
        break;
    }
}
