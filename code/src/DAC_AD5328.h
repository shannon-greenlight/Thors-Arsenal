// DAC_AD5328.h
#ifndef DAC_AD5328_H
#define DAC_AD5328_H

#include "DAC.h"
#include "Greenface_AD5328.h" // Include the specific header for this DAC

class DAC_AD5328 : public DAC
{
public:
    DAC_AD5328();

    static const int NUM_DACS = 8;
    void begin(float _idle_max) override;
    void out(byte output, uint16_t val) override;
    int16_t scale_corrections[NUM_DACS];
    int16_t offset_corrections[NUM_DACS];

private:
    uint16_t dac_value[NUM_DACS];
    Greenface_AD5328 output_device;
};

#endif