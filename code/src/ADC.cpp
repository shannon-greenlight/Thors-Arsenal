#include "ADC.h"
// #include <Adafruit_ADS1X15.h>
// #include "config.h"
// #include "hardware_defs.h"

ADC::ADC()
{
}

int ADC::read(int adc_num)
{
    int adc = read_raw(adc_num) * -1;
    adc += offset_correction[adc_num];
    adc *= scale_correction[adc_num] / 1000.0;
    return adc;
}

float ADC::adc_to_volts(int adc)
{
    return IN_FS_VOLTS * adc / 1650;
}

void ADC::begin()
{
    if (!ads.begin())
    {
        Serial.println("Failed to initialize ADS.");
        while (1)
            ;
    }
    ads.setGain(GAIN_TWO);
}

int ADC::read_raw(int adc_num)
{
    int adc = 0;
    switch (adc_num)
    {
    case 0:
        adc = ads.readADC_Differential_0_3();
        break;

    case 1:
        adc = ads.readADC_Differential_1_3();
        break;

    default:
        break;
    }
    return adc;
}