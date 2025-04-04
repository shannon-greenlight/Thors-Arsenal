// ensure this library description is only included once
#ifndef SPANK_fxn_h
#define SPANK_fxn_h

#define LONGEST_PULSE 0
#define SHORTEST_PULSE 1
#define NUM_PULSES 2
#define RANDOMNESS 3
#define INITIAL_DELAY 4

// for use with get_num_digits
#define NUMERIC_DIGITS_ONLY true

// include types & constants of Wiring core API
#include "Arduino.h"
#include <TerminalVT100.h>
#include <Greenface_EEPROM.h>
#include <EEPROM_Int.h>
#include <EEPROM_Arr16.h>
#include <EEPROM_String.h>
#include <Greenface_ui.h>
#include "../../super_globals.h"

#define SPANK_INT_PARAM_TYPE 0
#define SPANK_STRING_PARAM_TYPE 1
#define SPANK_STRING_VAR_TYPE 2
#define SPANK_FIXED_POINT_TYPE 3

#define STRING_TOKEN "$~"

extern Greenface_ui ui;
extern String toJSON(String, String);
extern int user_entering_param;

typedef void (*update_fxn)(void);

// library interface description
class Greenface_gadget
{
public:
    typedef void (Greenface_gadget::*FunctionPointer)();
    Greenface_gadget(String _name, String *_labels, uint16_t **_params, uint16_t _num_params);
    String name;
    String *labels, *text, (*text_extra)[NUM_WAVEFORMS];
    String *string_params, *string_param_init_vals;
    String *alt_values, *units, *cmd;
    EEPROM_String *string_vars;
    int16_t *offsets = nullptr;
    bool *active_params;
    bool *is_slider;
    int8_t *decimal_places = nullptr;
    boolean check_params = false;
    uint16_t *mins;
    uint16_t *maxs;
    uint16_t *init_vals;
    String *init_stringvars;

    uint16_t num_params;
    int param_num;
    uint8_t digit_num;
    void begin(void);
    void init();
    void init_param(int8_t _param_num = -1);

    String get_label(int label_num);
    uint8_t get_param_type(int16_t indx = -1);
    uint16_t get_param(int16_t indx = -1);     // returns raw
    int get_param_w_offset(int16_t indx = -1); // returns offset param as int
    float get_param_as_float(int16_t indx = -1);
    String get_param_as_string(int16_t indx = -1);
    String get_param_as_string_var(int16_t indx = -1);
    bool get_param_active(int16_t indx = -1);
    uint16_t get_min(int16_t indx = -1);
    uint16_t get_max(int16_t indx = -1);
    int get_min_w_offset(int16_t indx = -1);
    int get_max_w_offset(int16_t indx = -1);
    int get_decimal_places(int16_t indx = -1);

    void adjust_param(int e, unsigned long delta);
    void param_put(uint16_t val, int8_t _param_num = -1); // raw -- no update fxn, printing
    void put_param_num(int val);
    void put_dig_num(int val);
    void put_param(uint16_t val, int8_t _param_num = -1);
    void put_param_w_offset(int val, int8_t _param_num = -1);
    void put_string_var(String val, int16_t indx = -1);
    void insert_char(char);
    void remove_char();
    int check_param(int the_param);
    void inc_param_num_by(int val);
    void inc_dig_num_by(int val);
    void hilight_param();
    void printParam(int8_t _param_num = -1);
    void printParams();
    String params_toJSON();         // for wifi/USB
    String selected_param_toJSON(); // for wifi/USB
    void set_cursor_to_param();
    bool param_is_stringvar();
    bool is_available(int16_t indx = -1);

    void trigger(uint8_t, bool toggle = true);
    void clear_trigger(uint8_t);
    void display(void); // calls vectored fxn
    void copy_to(Greenface_gadget &target);
    void debug(void);

    void default_display(void); // prints name and params
    byte display_offset = 0;    // user fxn sets this to 1 to make room for sequence
    boolean enable_hilight = true;

    int offset(void);
    uint8_t triggered_by;

    // vectored fxns
    void (*display_fxn)();
    update_fxn *update_fxns;

    bool triggered;
    void (*trigger_fxn)();
    void exe_update_fxn(int8_t _param_num = -1);

    // library-accessible "private" interface
private:
    EEPROM_Arr16 params;
    uint8_t get_num_digits(int, bool numeric_only = false);
    String calc_format(uint8_t, bool);
    void print_param(uint16_t, uint16_t);
    void print_string_param(uint16_t, uint16_t);
    void print_string_var(uint16_t, uint16_t);
    String param_toJSON(int); // for wifi/USB
    // const uint8_t int_param_type = 0;
    // const uint8_t string_param_type = 1;
    // const uint8_t string_var_type = 2;
};
#endif
