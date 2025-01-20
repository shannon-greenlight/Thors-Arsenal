// DAC_MAX5136.h
#ifndef DAC_MAX5136_H
#define DAC_MAX5136_H

#include "DAC.h"
#include "Greenface_MAX513X.h" // Include the specific header for this DAC

class DAC_MAX5136 : public DAC
{
public:
    DAC_MAX5136();

    static const int NUM_DACS = 2;
    void begin(float _idle_max) override;
    void out(byte output, uint16_t val) override;
    int16_t scale_corrections[NUM_DACS];
    int16_t offset_corrections[NUM_DACS];

private:
    uint16_t dac_value[NUM_DACS];
    Greenface_MAX513X output_device;
};

#endif