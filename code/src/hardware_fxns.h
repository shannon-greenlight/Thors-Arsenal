ADC adc;

void hardware_begin()
{
    pinMode(DOUT_PIN, OUTPUT);
    pinMode(CLK_PIN, OUTPUT);
    pinMode(CS_PIN, OUTPUT);

    pinMode(T0_LED, OUTPUT);
    pinMode(T1_LED, OUTPUT);
    pinMode(T2_LED, OUTPUT);
    pinMode(T3_LED, OUTPUT);

    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);
#if defined(THORS_ARSENAL) || defined(FAFNIRS_FIRE)
    pinMode(LEDAR, OUTPUT);
    pinMode(LEDAG, OUTPUT);
#endif

    // TODO: Input pullup is broken!!!
    // https://forum.arduino.cc/t/interrupt-on-digital-pin-does-not-work-on-nano-rp2040-connect/960900/2
    pinMode(T0_PIN, INPUT_PULLDOWN);
    pinMode(T1_PIN, INPUT_PULLDOWN);
    pinMode(T2_PIN, INPUT_PULLDOWN);
    pinMode(T3_PIN, INPUT_PULLDOWN);

    analogWriteResolution(10);

    adc.begin();

    float idle_max = settings_get_fs_fixed() ? OLD_OUTPUT_VOLTS_MAX : OUTPUT_VOLTS_MAX;
#if defined(THORS_ARSENAL) || defined(FAFNIRS_FIRE)
    idle_max *= 2;
#endif
    dac.begin(idle_max);
}

void led_write(uint8_t led_index, bool state)
{
    digitalWrite(leds[led_index], state);
}
