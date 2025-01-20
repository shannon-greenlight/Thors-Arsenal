#include "DAC_MAX5136.h"
#include <Greenface_MAX513X.h>
#include "hardware_defs.h"

DAC_MAX5136::DAC_MAX5136() : output_device(CS_PIN, DOUT_PIN, CLK_PIN, 0)
{
}

void DAC_MAX5136::out(byte output, uint16_t val)
{
    DAC::out(output, val);
    output_device.dac_out(output, dac_value[output]);
}

void DAC_MAX5136::begin(float _idle_max)
{
    scale_corrections_ptr = scale_corrections;
    offset_corrections_ptr = offset_corrections;
    dac_value_ptr = dac_value;
    idle_max = _idle_max;
    output_device.begin();
}