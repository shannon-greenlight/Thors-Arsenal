// output range code starts here
// for Bonkulator
#include <Adafruit_MCP23X08.h>
Adafruit_MCP23X08 mcp;
bool output_ranges_installed = true;

void set_output_range(uint8_t output, output_range_type output_range)
{
    if (output_ranges_installed)
    {
        // output_ranges[output] = output_range;
        mcp.digitalWrite(output, output_range);
    }
}

void output_ranges_begin()
{
    if (!mcp.begin_I2C())
    {
        Serial.println("MCP not installed. ");
        output_ranges_installed = false;
        // adjust output params here?
    }
    else
    {
        for (int i = 0; i < NUM_OUTPUTS; i++)
        {
            mcp.pinMode(i, OUTPUT);
        }
    }
}

void output_ranges_init()
{
}

void output_update_range()
{
    int output = selected_output.get();
    uint16_t range_type = (the_output)().get_param(OUTPUT_RANGE);
    set_output_range(output, (output_range_type)range_type);
    (the_output)().offsets[OUTPUT_IDLE_VALUE] = (the_output)().get_param(OUTPUT_RANGE) == OUTPUT_RANGE_ZERO_TO_TEN ? 0 : OUTPUT_IDLE_VALUE_OFFSET_UI;
}
void update_output_ranges(Greenface_gadget *output)
{
    if (output_ranges_installed)
    {
        output_update_range();
    }
    else
    {
        output->maxs[OUTPUT_RANGE] = (uint16_t)OUTPUT_RANGE_PLUS_MINUS_FIVE;
    }
}
// for the MCP4921
// todo move this unused DAC code to a library
// void output_dac(int dac_num, int output_value)
// {
//   byte me;
//   me = (dac_num + 1) << 4 | (output_value >> 4);
//   digitalWrite(CS_PIN, LOW);
//   shiftOut(DOUT_PIN, CLK_PIN, MSBFIRST, me);
//   me = output_value << 4;
//   shiftOut(DOUT_PIN, CLK_PIN, MSBFIRST, me);
//   digitalWrite(CS_PIN, HIGH);
// }
