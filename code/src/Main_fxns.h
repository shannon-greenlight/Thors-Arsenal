class Main_fxns
{
public:
    // Static method to get the instance of the singleton
    static Main_fxns &getInstance()
    {
        static Main_fxns instance; // Guaranteed to be destroyed and instantiated on first use
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent copying
    Main_fxns(const Main_fxns &) = delete;
    Main_fxns &operator=(const Main_fxns &) = delete;

    // Public method to demonstrate functionality
    void doSomething()
    {
        // Implementation here
        ui.terminal_debug("Main_fxns: " + String("doSomething"));
    }

    void init_parameters()
    {
        ui.all_off();
        init_all_outputs();
        // user_waveforms_init();
        settings_init();
        // output_cal_init();
        bounce_init();
        ui.all_on(!usb_direct_enabled());
        // Serial.println("Init bounce done!");

        selected_output.put(0);
        remembered_fxn.put(0);

        // do this last!
        // Serial.println("eeprom_offset: " + String(Greenface_EEPROM::eeprom_offset));
        // selected_output.write_int(EEPROM_INIT_FLAG, EEPROM_INIT_PATTERN);
        // selected_output.write_int(EEPROM_OFFSET_FLAG, Greenface_EEPROM::eeprom_offset);
    }

    void init_input_cal()
    {
        input_cal_init();
        input_corrections_init();
    }

    void init_output_cal()
    {
        output_cal_init();
        output_corrections_init();
    }

    void init_all()
    {
        init_input_cal();
        init_output_cal();
        init_parameters();
#ifdef WIFI_INSTALLED
        wifi_init();
#endif
    }

    bool in_output_fxn()
    {
        return selected_fxn_num < SETTINGS_FXN;
    }

    int get_active_time_index()
    {
        return in_bounce_fxn() ? BOUNCE_SAMPLE_TIME : NULL_PARAM;
    }

    String list_fxns()
    {
        String out = "\"fxns\" : [";
        out += list_outputs();
        out += ", " + enquote("Settings");
        out += "],";
        return out;
    }

    void _select_fxn(int fxn_index)
    {
        selected_fxn = bonk_outputs[fxn_index];
        selected_output.put(fxn_index);
    }

    void select_fxn(int fxn_index)
    {
        fxn_index = constrain(fxn_index, OUTPUT_0_FXN, SETTINGS_FXN);
        selected_fxn_num = fxn_index;
        // force_cv();
        if (fxn_index == SETTINGS_FXN)
        {
            int param_num = settings_fxn.param_num;
            selected_fxn = &settings_fxn;
            clear_all_triggers();
            check_triggers();
            settings_fxn.param_num = param_num;
            timer0_fxn = &timer_service_settings;
            // outputs_begin();
        }
        else
        {
            _select_fxn(fxn_index);
            timer0_fxn = &timer_service_outputs;
        }
        remembered_fxn.put(fxn_index);
        selected_fxn->display();
    }

    void trigger_output(byte trig_num, int output_num)
    {
        OutputData *outptr = &outputs[output_num];
        (*bonk_outputs[output_num]).trigger(trig_num, (*bonk_outputs[output_num]).get_param(OUTPUT_REPEAT) == 0);
        if ((*bonk_outputs[output_num]).get_param(OUTPUT_CLOCK) != 1 && (*bonk_outputs[output_num]).get_param(OUTPUT_WAVEFORM) != WAVEFORM_TOGGLE)
        {
            outptr->current_index = 0;
            outptr->repeat_count = (*bonk_outputs[output_num]).get_param(OUTPUT_REPEAT);
            outptr->next_time = (*bonk_outputs[output_num]).get_param(OUTPUT_INIT_DELAY) + millis();
        }
        // set led
        led_write(trig_num, HIGH);
    }

    void set_group_vals(bool init)
    {
        int fxn_memory = selected_fxn_num;
        int my_param = selected_fxn->get_param(selected_fxn->param_num);
        int my_param_num = selected_fxn->param_num;
        for (int i = 0; i < NUM_FXNS - 1; i++)
        {
            if ((group & (1 << i)) != 0)
            {
                _select_fxn(i);
                //(*bonk_outputs[i]).param_num = selected_fxn->param_num;
                // (*bonk_outputs[i]).param_put(my_param, selected_fxn->param_num);
                if (init)
                {
                    selected_fxn->init_param();
                    selected_fxn->exe_update_fxn();
                }
                else
                {
                    selected_fxn->put_param(my_param, my_param_num);
                }
            }
        }
        _select_fxn(fxn_memory);
    }

    void setup()
    {
        Wire.begin();
        SPI.begin();
        Serial.begin(115200);

        // pinMode(KEYBOARD_COLUMN_0, INPUT_PULLUP);
        keyboard_begin();
        while (digitalRead(KEYBOARD_COLUMN_0) && !IS_RELEASE)
        {
        };

        ee_info.begin(false);
        ee_info.xfer();

        output_ranges_begin();

        timer0_fxn = &timer_service_outputs;
        selected_output.begin(true);
        selected_output.xfer();
        remembered_fxn.begin(false);
        remembered_fxn.xfer();

        // init_test.begin(false);
        // init_test.xfer();

        // delay as bit before starting or else CRASH!!!
        delay(500);

        settings_begin();
        hardware_begin();
        ui.begin();
        timer_begin();
        // triggers_begin();
        // keyboard_begin();
        // settings_begin();
        input_cal_begin();
        output_cal_begin();
        rotary_leds_set_color(ROTARY_LEDS_RED);
        bounce_begin();
        rotary_encoder_begin();
        outputs_begin();
        user_waveforms_begin();

#ifdef WIFI_INSTALLED
        wifi_begin();
#endif
        if (!eeprom_is_initialized())
        {
            Serial.println(F("EEPROM is not initialized!"));
            init_all();
            selected_output.write_int(EEPROM_INIT_FLAG, EEPROM_INIT_PATTERN);
            selected_output.write_int(EEPROM_OFFSET_FLAG, Greenface_EEPROM::eeprom_offset);

            // re-begin eeprom stuff
            settings_begin();
            input_cal_begin();
            output_cal_begin();
            outputs_begin();
            user_waveforms_begin();

            // selected_output.test();
        }

        // outputs_begin();
        rotary_leds_set_color(ROTARY_LEDS_ORANGE);
        Serial.println("Howdy!");
        ui.display_on();
        ui.signon(face1);
        delay(2000);
        rotary_leds_set_color(ROTARY_LEDS_GREEN);
        ui.splash();
        delay(3000);
        settings_put_usb_direct(0);
        select_fxn(remembered_fxn.get());
        triggers_begin();
        // hardware_begin();
        rotary_leds_set_color(ROTARY_LEDS_OFF);
    }

    void loop()
    {
        String status_string;
        static unsigned long cnt;
        debug = false;

        if (debug)
            ui.terminal_debug("Loop Start.");

#ifdef THORS_ARSENAL
        check_rotary_leds();
#endif

        check_keyboard();
        if (debug)
            Serial.println("Keyboard checked");

        check_serial();
        if (debug)
            Serial.println("Serial checked");

        if (in_main_fxn() || in_bounce_fxn())
        {
            if (in_main_fxn())
            {
                check_triggers();
                status_string = check_outputs();
                if (cnt % 10 == 0)
                    check_index();
                check_cv_inputs();
            }
            else
            {
                // check_bounce_triggers();
                String temp_status_string = check_bounce();
                if (temp_status_string != "")
                {
                    status_string = temp_status_string;
                }
            }
            if (debug)
                Serial.println("Outputs checked");
        }
        else if (in_user_waveform_fxn())
        {
            status_string = check_user_recording();
        }
        else if (in_input_cal_fxn())
        {
            status_string = check_input_cal();
        }
        else if (in_output_cal_fxn())
        {
            status_string = check_output_cal();
        }

#ifdef WIFI_INSTALLED
        status_string += check_wifi();
#endif

        if (debug)
            Serial.println("Server checked");

        check_rotary_encoder();
        if (debug)
            Serial.println("Encoder checked");

        if (!in_settings())
        {
            // terminal status interferes with entering strings
            // this is only done in settings AFAIK
            terminal_print_status(status_string);
        }

        ui.check_inactivity_timer(settings_get_inactivity_timeout());
        if (debug)
            Serial.println("Inactivity checked");
        if (cnt++ % 55 == 1)
        {
            serviceLEDs();
        }
        if (debug)
            Serial.println("Count checked: " + String(cnt));
    }

private:
    // Private constructor to prevent direct instantiation
    bool debug = false;
    Main_fxns()
    {
        // Initialization code here
        // cnt = 0;
    }

    // Private destructor
    ~Main_fxns()
    {
        // Cleanup code here
    }

    void serviceLEDs()
    {
        static unsigned int led_indx = 0;

#ifdef BONKULATOR
#define OFF_VAL 255
#define ON_VAL 0
        analogWrite(LEDR, OFF_VAL); // Turn off Red LED
        analogWrite(LEDG, OFF_VAL); // Turn off Green LED
        analogWrite(LEDB, OFF_VAL); // Turn off Blue LED
        switch (led_indx)
        {
        case 0:
            analogWrite(LEDR, ON_VAL); // Turn on Red LED
            break;
        case 1:
            analogWrite(LEDG, ON_VAL); // Turn on Green LED
            break;
        case 2:
            analogWrite(LEDB, ON_VAL); // Turn on Blue LED
            break;
        }
#elif defined THORS_ARSENAL
        led_write(LED_RED, led_indx == 0 ? HIGH : LOW);   // Turn on Red LED
        led_write(LED_GREEN, led_indx == 1 ? HIGH : LOW); // Turn on Green LED
        led_write(LED_BLUE, led_indx == 2 ? HIGH : LOW);  // Turn on Blue LED
#endif
        led_indx++;
        if (led_indx > 2)
            led_indx = 0;
        // Serial.println("LEDs checked: " + String(led_indx) + " LEDG: " + String(LEDG));
    }
};

// Usage example
// int main() {
//     Main_fxns& singleton = Main_fxns::getInstance();
//     singleton.doSomething();
//     return 0;
// }