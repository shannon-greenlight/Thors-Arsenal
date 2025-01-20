#include "Arduino.h"

void scale_int(int *val, float scale)
{
    *val *= scale;
    *val = min(DAC_FS, *val);
    // *val = max(0, *val);
}

String toJSON(String key, String value)
{
    String s1 = "\\";
    String s2 = "\\\\";
    value.replace(s1, s2);

    s1 = "\"";
    s2 = "\\\"";
    value.replace(s1, s2);

    // Serial.println(value);
    return "\"" + key + "\" :\"" + value + "\"";
}

String params_toJSON()
{
    return selected_fxn->params_toJSON();
}

String urlDecode(String str)
{
    String ret;
    char ch;
    int i, ii, len = str.length();

    for (i = 0; i < len; i++)
    {
        if (str[i] != '%')
        {
            if (false && str[i] == '+') // spank client must not use + for space
                ret += ' ';
            else
                ret += str[i];
        }
        else
        {
            sscanf(str.substring(i + 1, i + 3).c_str(), "%x", &ii);
            // Serial.println("UrlDecode char: " + str + ": " + String(ii) + ".");
            // ii = 32;
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        }
    }
    return ret;
}

bool in_settings()
{
    return selected_fxn == &settings_fxn;
}

bool in_main_fxn()
{
    return selected_fxn_num < 8;
}

boolean in_wifi()
{
#ifdef WIFI_INSTALLED
    return selected_fxn == &wifi_fxn;
#else
    return false;
#endif
}

void adjust_param(int encoder_val)
{
    if (in_settings())
    {
        settings_adjust_param(encoder_val, 0);
    }
    else
    {
        selected_fxn->adjust_param(encoder_val, 0);
    }
}

bool ok_to_set_group()
{
    bool in_group = group & (1 << selected_fxn_num);
    return (group_active && group > 0 && in_group && !in_settings());
}

void adjust_group(int encoder_val)
{
    adjust_param(encoder_val); // adjust this parameter
    if (ok_to_set_group())
    {
        set_group_vals(); // now set the rest of the group
    }
}

// todo add param_num as optional parameter
void init_group()
{
    selected_fxn->init_param();
    selected_fxn->exe_update_fxn();
    selected_fxn->printParam();
    if (ok_to_set_group())
    {
        set_group_vals(true);
    }
}

void put_group(int int_param, int index)
{
    if (index != NULL_PARAM)
    {
        selected_fxn->put_param_w_offset(int_param, index);
        if (ok_to_set_group())
        {
            set_group_vals();
        }
    }
}

void check_rotary_encoder()
{
    if (encoder_is_adjusting())
    {
        ui.reset_inactivity_timer();
        adjust_param(getEncoderValue());
        resetEncoder();
        delay(25);
    }
}

void activate()
{
    if (in_settings())
    {
        settings_activate();
    }
    else if (in_main_fxn() || in_bounce_fxn())
    {
        selected_trigger->trigger();
    }
    else if (in_user_waveform_fxn())
    {
        user_waveforms_activate();
    }
#ifdef WIFI_INSTALLED
    else if (in_wifi())
    {
        wifi_activate();
    }
#endif
    else if (in_ask_init())
    {
        do_init();
    }
}

void inc_param_num()
{
    selected_fxn->inc_param_num_by(1);
}

void dec_param_num()
{
    selected_fxn->inc_param_num_by(-1);
}

void inc_dig_num()
{
    if (selected_fxn->get_param_type(selected_fxn->param_num) == SPANK_STRING_VAR_TYPE && key_held_down)
    {
        // insert new char
        selected_fxn->insert_char(' ');
    }
    else
    {
        selected_fxn->inc_dig_num_by(1);

        if (!in_settings() && (the_output)().param_num == OUTPUT_WAVEFORM)
        {
            graph_waveform(selected_output.get());
        }

        if (in_user_waveform_fxn() && selected_fxn->param_num == USER_WAVEFORMS_NAME)
        {
            user_waveforms_update_waveform();
        }
    }
}

void dec_dig_num()
{
    if (selected_fxn->get_param_type(selected_fxn->param_num) == SPANK_STRING_VAR_TYPE && key_held_down)
    {
        // remove char
        selected_fxn->remove_char();
    }
    else
    {
        selected_fxn->inc_dig_num_by(-1);
        if ((!in_settings() && (the_output)().param_num == OUTPUT_WAVEFORM) || (in_user_waveform_fxn() && selected_fxn->param_num == USER_WAVEFORMS_NAME))
        {
            selected_fxn->printParams();
        }
    }
}

void new_output(Greenface_gadget *output)
{
    selected_fxn = output;
    apply_params_to_waveform(selected_output.get());
    selected_fxn->display();
}
