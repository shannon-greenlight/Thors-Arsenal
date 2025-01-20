#include <Wire.h>

// enum
// {
//     SETTINGS_VER,
//     SETTINGS_NAME,
//     SETTINGS_ENCODER,
//     SETTINGS_WIFI,
//     SETTINGS_USB_DIRECT,
//     SETTINGS_SCREEN,
//     SETTINGS_USER_WAVEFORMS,
//     SETTINGS_RESET,
//     SETTINGS_CAL,
//     SETTINGS_BOARD_GENERATION,
//     SETTINGS_BOUNCE,
//     SETTINGS_NUM_PARAMS
// };

#ifdef BONKULATOR
#define CAL_STRING "Inputs,Outputs"
#define CAL_MAX 1
#define WIFI_MAX 2
#define WIFI_CHOICES "Disable,Enable,Reset"
#define WIFI_ACTIVE 1
#define RESET_MAX 4
#define RESET_CHOICES "All,Parameters,User Waveforms,Inputs,Outputs"
#elif defined THORS_ARSENAL || defined FAFNIRS_FIRE
#define CAL_STRING "Inputs"
#define CAL_MAX 0
#define WIFI_MAX 0
#define WIFI_CHOICES "Not Installed"
#define WIFI_ACTIVE 0
#define RESET_MAX 3
#define RESET_CHOICES "All,Parameters,User Waveforms,Inputs"
#endif

uint16_t _settings_params[SETTINGS_NUM_PARAMS];
uint16_t _settings_mins[] = {0, 0, 0, 0, 0, 1, 0, 0, 0, 10, 0};
uint16_t _settings_maxs[] = {0, 0, 1, WIFI_MAX, 0, 9999, 7, RESET_MAX, CAL_MAX, 43, 1};
uint16_t _settings_init_vals[] = {0, 0, 0, 0, 0, 15, 0, 0, 0, 43, 0};
uint16_t *settings_stuff[] = {_settings_params, _settings_mins, _settings_maxs, _settings_init_vals};
String settings_labels[] = {"Version: ", "Name: ", "Encoder Type: ", "WiFi: ", "USB Direct: ", "Screen Saver: ", "Waveforms: ", "Reset: ", "Calibrate: ", "Board Revision: ", "Bounce: "};
String settings_string_params[] = {VERSION_NUM, "$~", "Normal ,Reverse", WIFI_CHOICES, "Disabled", "", "User 0,User 1,User 2,User 3,User 4,User 5,User 6,User 7", RESET_CHOICES, CAL_STRING, "", ""};
String settings_init_stringvars[] = {"", DEVICE_NAME, "", "", "", "", "", "", "", "", ""};
bool settings_param_active[] = {0, 0, 0, WIFI_ACTIVE, 0, 0, 1, 1, 1, 0, 1};
int8_t settings_decimal_places[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}; // allows fixed point numbers

// void (*func_ptr[3])() = {fun1, fun2, fun3};

EEPROM_String settings_device_name(20);
EEPROM_String settings_string_vars[] = {settings_device_name};
Greenface_gadget settings_fxn("Settings", settings_labels, settings_stuff, sizeof(_settings_params) / sizeof(_settings_params[0]));

void settings_begin();

uint16_t _ask_init_params[1];
uint16_t _ask_init_mins[] = {0};
uint16_t _ask_init_maxs[] = {0};
uint16_t _ask_init_init_vals[] = {0};
uint16_t *ask_init_stuff[] = {_ask_init_params, _ask_init_mins, _ask_init_maxs, _ask_init_init_vals};
String ask_init_labels[] = {"Reset: "};
String ask_init_string_params[] = {"Reset who?"};
bool ask_init_param_active[] = {1};

Greenface_gadget ask_init_fxn("Press Activate to: ", ask_init_labels, ask_init_stuff, sizeof(_ask_init_params) / sizeof(_ask_init_params[0]));

#include "settings_text.h"

// "member" fxns
bool in_ask_init()
{
    return selected_fxn == &ask_init_fxn;
}

bool in_user_waveforms()
{
    return (selected_fxn_num == SETTINGS_FXN && settings_fxn.param_num == SETTINGS_USER_WAVEFORMS);
}

// bool in_wifi()
// {
//     return (selected_fxn_num == SETTINGS_FXN && settings_fxn.param_num == SETTINGS_WIFI);
// }

void do_init()
{
    int init_type = settings_fxn.get_param(SETTINGS_RESET);
    switch (init_type)
    {
    case INIT_PARAMETERS:
        init_parameters();
        break;
    case INIT_USER_WAVEFORMS:
        user_waveforms_init();
        break;
    case INIT_INPUTS:
        init_input_cal();
        break;
    case INIT_OUTPUTS:
        init_output_cal();
        break;
    case INIT_ALL:
        init_all();
        break;
    }
    // settings_begin();
    // outputs_begin();
    // user_waveforms_begin();
    select_fxn(remembered_fxn.get());
    selected_fxn->display();
}

bool usb_direct_enabled()
{
    return settings_string_params[SETTINGS_USB_DIRECT] == "Enabled";
}

void set_usb_direct()
{
    bool usb_direct = usb_direct_enabled();
    ui.terminal_mirror = usb_direct ? false : true;
    if (usb_direct)
    {
        // ui.terminal_debug("USB Direct Enabled");
        send_globals_to_USB();
    }
}

void settings_put_usb_direct(int val)
{
    settings_string_params[SETTINGS_USB_DIRECT] = val ? "Enabled" : "Disabled";
    set_usb_direct();
}

void settings_put_param(int val)
{
    settings_fxn.put_param(val);
    switch (settings_fxn.param_num)
    {
    case SETTINGS_ENCODER:
        set_encoder();
        break;
    case SETTINGS_SCREEN:
        restore_display();
        break;
    case SETTINGS_USB_DIRECT:
        set_usb_direct();
        break;
    }
}

void settings_adjust_param(int encoder_val, unsigned long delta)
{
    // ui.terminal_debug(settings_fxn.name + " - param num: " + String(settings_fxn.param_num));
    switch (settings_fxn.param_num)
    {
    case SETTINGS_ENCODER:
        settings_fxn.adjust_param(encoder_val, delta);
        set_encoder();
        break;
    case SETTINGS_SCREEN:
        settings_fxn.adjust_param(encoder_val, delta);
        restore_display();
        break;
    // case SETTINGS_USB_DIRECT:
    //     settings_fxn.adjust_param(encoder_val, delta);
    //     set_usb_direct();
    //     break;
    default:
        settings_fxn.adjust_param(encoder_val, delta);
    }
}

// returns software version
String settings_get_version()
{
    return settings_fxn.get_param_as_string(SETTINGS_VER);
}

// returns true for old style boards without output fs adjust pot
bool settings_get_fs_fixed()
{
    return settings_fxn.get_param(SETTINGS_BOARD_GENERATION) < 42 && PRODUCT_NAME == "Bonkulator";
}

String settings_get_device_name()
{
    return settings_fxn.get_param_as_string_var(SETTINGS_NAME);
}

uint16_t settings_get_encoder_type()
{
    return settings_fxn.get_param(SETTINGS_ENCODER);
}

bool wifi_enabled()
{
    return settings_fxn.get_param(SETTINGS_WIFI);
}

int settings_get_inactivity_timeout()
{
    return settings_fxn.get_param(SETTINGS_SCREEN);
    // return max(300, settings_fxn.get_param(SETTINGS_SCREEN) * 300); // 1 minute = 300 heartbeats
}

void adc_cal()
{
    ui.terminal_debug("Calibrate ADC someday...");
}

uint16_t settings_get_bounce()
{
    return settings_fxn.get_param(SETTINGS_BOUNCE);
}

void settings_activate()
{
    int mode;
    switch (settings_fxn.param_num)
    {
#ifdef WIFI_INSTALLED
    case SETTINGS_WIFI:
        selected_fxn = &wifi_fxn;
        mode = settings_fxn.get_param(SETTINGS_WIFI);
        wifi_display();
        break;
#endif
    case SETTINGS_USER_WAVEFORMS:
        // ui.terminal_debug("User Waveforms!");
        selected_fxn = user_waveforms[settings_fxn.get_param(SETTINGS_USER_WAVEFORMS)];
        user_waveforms_update_index();
        user_waveforms_recording_state = RECORDING_IDLE;
        selected_fxn->display();
        // ui.terminal_debug("User Waveforms: setting param# " + String(settings_fxn.param_num));
        break;

    case SETTINGS_RESET:
        if (in_ask_init())
        {
            // do_init();
        }
        else
        {
            ask_init_text[0] = ask_init_options[settings_fxn.get_param(SETTINGS_RESET)];
            ask_init_string_params[0] = settings_fxn.get_param_as_string(SETTINGS_RESET);
            selected_fxn = &ask_init_fxn;
            selected_fxn->display();
        }
        break;

    case SETTINGS_CAL:
        switch (settings_fxn.get_param(SETTINGS_CAL))
        {
        case 0:
            selected_fxn = &input_cal_fxn;
            selected_fxn->display();
            // adc_cal();
            break;
        case 1:
            selected_fxn = &output_cal_fxn;
            selected_fxn->display();
            break;
        }
        break;

    case SETTINGS_BOUNCE:
        switch (settings_fxn.get_param(SETTINGS_BOUNCE))
        {
        case 0:
            selected_fxn = &bounce0_fxn;
            break;
        case 1:
            selected_fxn = &bounce1_fxn;
            break;
        }
        selected_fxn->display();
        break;

    default:
        // ui.terminal_debug("Initialized: " + String(is_initialized()) + " Pattern: " + String(EEPROM_INIT_PATTERN));
        break;
    }
}

void set_user_waveforms(String names)
{
    settings_string_params[SETTINGS_USER_WAVEFORMS] = names;
}

update_fxn settings_update_fxns[SETTINGS_NUM_PARAMS] = {nullptr, nullptr, set_encoder, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

void settings_init()
{
    uint16_t wifi_save = settings_fxn.get_param(SETTINGS_WIFI);
    wifi_save = constrain(0, 1, wifi_save);
    // settings_fxn.string_vars[0].put(DEVICE_NAME);
    settings_fxn.init();
    ask_init_fxn.init();
    // settings_fxn.param_put(wifi_save, SETTINGS_WIFI);
}

void settings_begin()
{
    settings_fxn.begin();
    // settings_fxn.display_fxn = &settings_display;
    settings_fxn.string_vars = settings_string_vars;
    settings_fxn.string_vars[0].begin(false);
    settings_fxn.string_vars[0].xfer();
    settings_fxn.string_params = settings_string_params;
    settings_fxn.init_stringvars = settings_init_stringvars;
    settings_fxn.update_fxns = settings_update_fxns;
    settings_fxn.active_params = settings_param_active;
    settings_fxn.decimal_places = settings_decimal_places;
    ask_init_fxn.begin();
    ask_init_begin();
    settings_text_begin();
    ask_init_fxn.string_params = ask_init_string_params;
    ask_init_fxn.active_params = ask_init_param_active;
    ask_init_fxn.text = ask_init_text;
    settings_fxn.text = settings_text;
}

void timer_service_settings()
{
    // do nothing for now
}

void user_waveform_params_macro(int waveform_num, bool print_header = true)
{
    Greenface_gadget *fxn = user_waveforms[waveform_num];
    uint16_t *waveform_data = _user_waveforms_data[waveform_num];
    // fxn->param_num = USER_WAVEFORMS_VALUE;
    if (print_header)
    {
        ui.t.clrScreen();
        ui.terminal_debug("// Macro for: " + fxn->name);
    }
    Serial.println("*p" + String(SETTINGS_USER_WAVEFORMS));
    Serial.println(String(waveform_num));
    Serial.println("!");
    gen_params_macro(fxn, false);

    Serial.println("p" + String(USER_WAVEFORMS_INDEX));
    Serial.println("0");
    Serial.println("p" + String(USER_WAVEFORMS_VALUE));

    for (int i = 0; i < WAVEFORM_PARTS; i++)
    {
        Serial.println(String(waveform_data[i]));
        Serial.println("!");
    }
}
