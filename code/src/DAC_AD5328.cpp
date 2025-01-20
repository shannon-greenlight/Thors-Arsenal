#include "DAC_AD5328.h"
#include <Greenface_AD5328.h>
#include "hardware_defs.h"

DAC_AD5328::DAC_AD5328() : output_device(CS_PIN, DOUT_PIN, CLK_PIN, 0)
{
}

void DAC_AD5328::out(byte output, uint16_t val)
{
    DAC::out(output, val);
    output_device.dac_out(output, dac_value[output]);
}

void DAC_AD5328::begin(float _idle_max)
{
    scale_corrections_ptr = scale_corrections;
    offset_corrections_ptr = offset_corrections;
    dac_value_ptr = dac_value;
    idle_max = _idle_max;
    output_device.begin(AD5328_LDAC_PASSTHRU, AD5328_VDD_BOTH, AD5328_BUFFERED_BOTH, AD5328_GAIN_NONE);
}