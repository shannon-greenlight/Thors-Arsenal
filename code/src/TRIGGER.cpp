#include "Arduino.h"
#include <TerminalVT100.h>

#include "TRIGGER.h"
#include <Greenface_gadget.h>
#include "hardware_defs.h"
#include "product_defs.h"

extern String toJSON(String key, String value);
extern void trigger_output(byte trig_num, int output_num);
extern void trigger_bounce(byte trig_num, int input_num);
extern bool in_settings();
extern bool in_main_fxn();
extern bool in_bounce_fxn();
extern void code_red(bool);
extern class Greenface_gadget *bounce_fxns[];
extern int bounce_debug;
extern uint16_t settings_get_bounce();

TRIGGER::TRIGGER(int _trig_num)
{
    trig_num = _trig_num;
    blanking_period = 0;
    for (int channel_num = 0; channel_num < TRIGGER_CTRL_CHANS; channel_num++)
    {
        blanking_periods[channel_num] = 0;
        trig_ctrl_vals[channel_num] = 0;
    }
}

void TRIGGER::start()
{
    start_time = millis();
}

void TRIGGER::trigger()
{
    start();
    // state = (state == TRIGGER_ACTIVE) ? TRIGGER_IDLE : TRIGGER_ACTIVE;
    state = TRIGGER_ACTIVE;
    if (in_main_fxn())
    {
        for (int output_num = 0; output_num < NUM_OUTPUTS; output_num++)
        {
            uint16_t ctrl_val = trig_ctrl_vals[output_num];
            uint8_t trig_ctrl = trig_ctrls[output_num];
            if (get_channel(output_num))
            {
                // trigger_output(trig_num, output_num); // for testing
                switch (trig_ctrl)
                {
                case TRIG_HOLDOFF:
                    if (start_time > blanking_periods[output_num])
                    {
                        trigger_output(trig_num, output_num);
                        blanking_periods[output_num] = start_time + ctrl_val;
                    }
                    break;
                case TRIG_SKIP:
                    if (counts[output_num] == 0)
                        trigger_output(trig_num, output_num);
                    if (counts[output_num]++ >= ctrl_val)
                    {
                        counts[output_num] = 0;
                    }
                    break;
                case TRIG_DENSITY:
                    if (random(0, 100) > 100 - ctrl_val)
                    {
                        trigger_output(trig_num, output_num);
                    }
                    break;
                default:
                    code_red(true);
                    // trigger_output(trig_num, output_num);
                }
            }
        }
    }
    else if (in_bounce_fxn())
    {
        for (int i = 0; i < 2; i++)
        {
            // bounce_debug = (*bounce_fxns[i]).get_param(2 + trig_num);
            uint8_t trig_ctrl = (*bounce_fxns[i]).get_param(11);              // BOUNCE_TRIG_CTRL
            uint8_t ctrl_val = (*bounce_fxns[i]).get_param(12);               // BOUNCE_TRIG_CTRL_VAL
            if (get_channel(i + NUM_OUTPUTS) && (settings_get_bounce() == i)) // 2 + trig_num  2 = BOUNCE_ENABLE_T0
            {
                switch (trig_ctrl)
                {
                case TRIG_HOLDOFF:
                    if (start_time > blanking_periods[i + NUM_OUTPUTS])
                    {
                        trigger_bounce(trig_num, i);
                        blanking_periods[i] = start_time + ctrl_val;
                    }
                    break;
                case TRIG_SKIP:
                    if (counts[i + NUM_OUTPUTS]++ > ctrl_val)
                    {
                        trigger_bounce(trig_num, i);
                        counts[i + NUM_OUTPUTS] = 0;
                    }
                    break;
                case TRIG_DENSITY:
                    if (random(0, 100) > 100 - ctrl_val)
                    {
                        trigger_bounce(trig_num, i);
                    }
                    break;
                default:
                    code_red(true);
                    // trigger_output(trig_num, output_num);
                }
            }
        }
    }
}

// used by user waveform recording
void TRIGGER::clear()
{
    state = TRIGGER_IDLE;
    // ui.terminal_debug("Clearing trigger: " + String(trig_num));
}

void TRIGGER::disable_all()
{
    channels = 0;
    // Serial.println("Disabling triggers for TRIG: " + String(trig_num));
}

void TRIGGER::trigger_report()
{
    Serial.println("Trigger " + String(trig_num) + ": " + String(channels));
}

String TRIGGER::params_toJSON()
{
    String out = "";
    out += "{ ";
    out += toJSON("trig_num", String(trig_num));
    out += ", ";
    out += toJSON("state", String(state));
    out += ", ";
    out += toJSON("outputs", String(channels));
    out += " }";
    return out;
}

void TRIGGER::set_channel(int channel_num, bool val)
{
    if (val)
    {
        bitSet(channels, channel_num);
    }
    else
    {
        bitClear(channels, channel_num);
    }
    if (state == TRIGGER_ACTIVE && !in_settings())
    {
        trigger_output(trig_num, channel_num);
    }
    if (state == TRIGGER_ACTIVE && in_bounce_fxn())
    {
        // Serial.println("TRIGGER: set_channel: trig_num: " + String(trig_num) + " channel_num: " + String(channel_num));
        trigger_bounce(trig_num, channel_num - NUM_OUTPUTS);
    }
    if (channels == 0)
    {
        clear();
    }
}

void TRIGGER::set_trig_ctrls(int channel_num, uint8_t trig_ctrl, uint16_t ctrl_val)
{
    trig_ctrls[channel_num] = trig_ctrl;
    trig_ctrl_vals[channel_num] = ctrl_val;
    start();
    blanking_periods[channel_num] = start_time + ctrl_val;
    // Serial.println(" set_trig_ctrls: output - " + String(channel_num) + " ctrl: " + String(trig_ctrl) + " val: " + String(ctrl_val));
}

bool TRIGGER::get_channel(int channel_num)
{
    return bitRead(channels, channel_num) == 1;
}
