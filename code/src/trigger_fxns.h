// Trigger functions
#include "Arduino.h"

int get_selected_trig_enable()
{
    return (!in_bounce_fxn() ? TRIGGER_FXN_ENABLE_T0 : BOUNCE_ENABLE_T0) + selected_trigger->trig_num;
}

int get_selected_trigger_toggled(int dig2, int selected_trig_enable)
{
    Greenface_gadget *temp_fxn;
    temp_fxn = !in_bounce_fxn() ? triggered_fxn[dig2] : bounce_fxns[dig2];
    return temp_fxn->get_param(selected_trig_enable) == 1 ? 0 : 1;
}

void clear_trigger(int output_num, int trig_num)
{
    OutputData *outptr = &outputs[output_num];

    (*triggered_fxn[output_num]).clear_trigger(trig_num);

    outptr->repeat_count = (*triggered_fxn[output_num]).get_param(TRIGGER_FXN_REPEAT);
    outptr->next_time = (*triggered_fxn[output_num]).get_param(TRIGGER_FXN_INIT_DELAY) + millis();
}

void clear_outputs(int trig_num)
{
    for (int output_num = 0; output_num < NUM_OUTPUTS; output_num++)
    {
        clear_trigger(output_num, trig_num);
    }
}

void select_trigger(int int_param)
{
    int digit_num = selected_fxn->digit_num;
    selected_trigger = triggers[int_param];
    if (!in_bounce_fxn())
    {
        selected_fxn->put_param_num(selected_trigger->trig_num + TRIGGER_FXN_ENABLE_T0);
    }
    else
    {
        selected_fxn->put_param_num(selected_trigger->trig_num + BOUNCE_ENABLE_T0);
    }
    selected_fxn->digit_num = digit_num; // restore digit_num
}

void clear_all_triggers()
{
    int trigger_save = selected_trigger->trig_num;
    for (int i = 0; i < NUM_TRIGGERS; i++)
    {
        clear_outputs(i);
        (*triggers[i]).clear();
    }
    select_trigger(trigger_save);
}

void disable_all_triggers()
{
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        for (int j = 0; j < NUM_TRIGGERS; j++)
        {
            (*triggered_fxn[i]).put_param(0, TRIGGER_FXN_ENABLE_T0 + j);
            if (i == 0) // we only need to do this once
            {
                (*triggers[j]).disable_all();
            }
        }
    }
}

bool check_any_triggers()
{
    bool triggered = false;
    for (int i = 0; i < NUM_TRIGGERS; i++)
    {
        if ((*triggers[i]).state == TRIGGER_ACTIVE)
        {
            triggered = true;
            // (*triggers[i]).state = TRIGGER_IDLE;
            (*triggers[i]).clear();
        }
    }
    return triggered;
}

String check_output_triggers()
{
    String out = "";
    String t = "     ";
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        if ((*triggered_fxn[i]).get_param(TRIGGER_FXN_CLOCK) == 1)
        {
            t = " X" + String((*triggered_fxn[i]).triggered_by) + "  ";
        }
        else
        {
            t = "     ";
            if ((*triggered_fxn[i]).triggered)
            {
                t = " T" + String((*triggered_fxn[i]).triggered_by) + "  ";
            }
        }
        out += String(i) + ":" + t;
    }
    return out;
}

bool trigger_inactive(int trig_num)
{
    bool trigger_active = false;
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        if ((*triggered_fxn[i]).get_param(TRIGGER_FXN_ENABLE_T0 + trig_num) && (*triggered_fxn[i]).triggered)
        {
            trigger_active = true;
        }
    }
    return !trigger_active;
}

void send_trig_status_to_USB()
{
    static trigger_state t0_memory;
    static trigger_state t1_memory;
    static trigger_state t2_memory;
    static trigger_state t3_memory;
    bool send_to_USB = false;
    if (usb_direct_enabled())
    {
        if (trig0.state != t0_memory)
        {
            send_to_USB = true;
            t0_memory = trig0.state;
        }

        if (trig1.state != t1_memory)
        {
            send_to_USB = true;
            t1_memory = trig1.state;
        }

        if (trig2.state != t2_memory)
        {
            send_to_USB = true;
            t2_memory = trig2.state;
        }

        if (trig3.state != t3_memory)
        {
            send_to_USB = true;
            t3_memory = trig3.state;
        }
        if (send_to_USB)
        {
            // send_triggers_to_USB();
            send_status_to_USB(TRIGGER_EVENT);
        }
    }
}

void check_triggers()
{
    send_trig_status_to_USB();
    for (int trig_num = 0; trig_num < 4; trig_num++)
    {
        if (trigger_inactive(trig_num))
        {
            led_write(trig_num, LOW);
            (*triggers[trig_num]).state = TRIGGER_IDLE;
        }
    }
}

void trigger0()
{
    trig0.trigger();
}

void trigger1()
{
    trig1.trigger();
}

void trigger2()
{
    trig2.trigger();
}

void trigger3()
{
    trig3.trigger();
}

void trigger_report()
{
    ui.t.clrDown("15");
    ui.terminal_printRow("Trigger Report", "16");
    Serial.println(F("Output\tT0\tT1\tT2\tT3\tTriggered"));
    ui.t.printChars(50, "-");
    Serial.print("\r\n");
    // Serial.println(F("----------------------------------"));
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        Serial.print(String(i));
        Serial.print("\t");
        Serial.print(String((*triggered_fxn[i]).get_param(TRIGGER_FXN_ENABLE_T0)));
        Serial.print("\t");
        Serial.print(String((*triggered_fxn[i]).get_param(TRIGGER_FXN_ENABLE_T1)));
        Serial.print("\t");
        Serial.print(String((*triggered_fxn[i]).get_param(TRIGGER_FXN_ENABLE_T2)));
        Serial.print("\t");
        Serial.print(String((*triggered_fxn[i]).get_param(TRIGGER_FXN_ENABLE_T3)));
        Serial.print("\t");
        Serial.println(String((*triggered_fxn[i]).triggered));
    }
    Serial.println();
    Serial.println(F("Input\tT0\tT1\tT2\tT3"));
    ui.t.printChars(50, "-");
    Serial.print("\r\n");
    // Serial.println(F("----------------------------------"));
    for (int i = 0; i < NUM_INPUTS; i++)
    {
        Serial.print(String(i));
        Serial.print("\t");
        Serial.print(String((*bounce_fxns[i]).get_param(BOUNCE_ENABLE_T0)));
        Serial.print("\t");
        Serial.print(String((*bounce_fxns[i]).get_param(BOUNCE_ENABLE_T1)));
        Serial.print("\t");
        Serial.print(String((*bounce_fxns[i]).get_param(BOUNCE_ENABLE_T2)));
        Serial.print("\t");
        Serial.print(String((*bounce_fxns[i]).get_param(BOUNCE_ENABLE_T3)));
        Serial.println();
        // Serial.print("\t");
        // Serial.println(String((*triggered_fxn[i]).triggered));
    }
    Serial.println();
    Serial.println("Trigger Channels");
    ui.t.printChars(16, "-");
    Serial.println();
    for (int i = 0; i < NUM_TRIGGERS; i++)
    {
        (*triggers[i]).trigger_report();
    }
}

void triggers_begin()
{
    attachInterrupt(digitalPinToInterrupt(T0_PIN), trigger0, RISING);
    attachInterrupt(digitalPinToInterrupt(T1_PIN), trigger1, RISING);
    attachInterrupt(digitalPinToInterrupt(T2_PIN), trigger2, RISING);
    attachInterrupt(digitalPinToInterrupt(T3_PIN), trigger3, RISING);
}
