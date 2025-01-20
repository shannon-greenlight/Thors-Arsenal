// DAC.h
#ifndef DAC_H
#define DAC_H

#include <Arduino.h>

class DAC
{
public:
    virtual ~DAC() {}

    virtual void begin(float _idle_max) = 0;
    virtual void out(byte output, uint16_t val) = 0;
    uint16_t volts_to_dac(float volts, byte output, bool output_is_bipolar);
    void volts_out(float volts, int output, bool output_is_bipolar);
    uint16_t get_dac_value(byte output);

protected:
    int16_t *scale_corrections_ptr; // not used at this time
    int16_t *offset_corrections_ptr;
    uint16_t *dac_value_ptr;
    float idle_max;
};

#endif