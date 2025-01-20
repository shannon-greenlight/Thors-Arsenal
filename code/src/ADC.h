// Analog to Digital Converter class definitions
#ifndef ADC_h
#define ADC_h

#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include "config.h"
#include "hardware_defs.h"

class ADC
{
public:
    ADC();
    int16_t scale_correction[2];
    int16_t offset_correction[2];
    int read(int adc_num);
    float adc_to_volts(int adc);
    void begin();

private:
    // Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
    Adafruit_ADS1015 ads; /* Use this for the 12-bit version */
    int read_raw(int adc_num);
};

#endif
