#include "DAC.h"
#include <Greenface_MAX513X.h>
#include "hardware_defs.h"

void DAC::out(byte output, uint16_t val)
{
    val = DAC_FS ^ val;
    *(dac_value_ptr + output) = val = constrain(val, 0, DAC_FS);
}

uint16_t DAC::volts_to_dac(float volts, byte output, bool output_is_bipolar)
{
    float volts_in = volts;
    if (output_is_bipolar)
    {
        volts += idle_max / 2.0;
    }
#if defined(THORS_ARSENAL) || defined(FAFNIRS_FIRE)
    else
    {
        volts += OUTPUT_VOLTS_MAX;
    }
#endif
    float portion = volts / idle_max;
    int dac_val = int((DAC_FS * portion) + .5);
    dac_val += (*(offset_corrections_ptr + output) + (volts_in >= 1.0 ? 1 : 0));
    // ui.terminal_debug("DAC: " + String(dac_val) + " is_bipolar: " + String(output_is_bipolar(output)) + " output correction: " + String(get_raw_output_offset_correction(output)));
    // ui.terminal_debug("DAC: " + String(dac_val) + " is_bipolar: " + String(output_is_bipolar(output)) + " volts: " + String(volts));
    return dac_val;
}

void DAC::volts_out(float volts, int output, bool output_is_bipolar)
{
    out(output, volts_to_dac(volts, output, output_is_bipolar));
}

uint16_t DAC::get_dac_value(byte output)
{
    return DAC_FS - *(dac_value_ptr + output);
}
