#include "Wire.h"
#include "Greenface_MAX513X.h"

Greenface_MAX513X::Greenface_MAX513X() : sync_pin(0), din_pin(0), sck_pin(0), ldac_pin(0)
{
}

Greenface_MAX513X::Greenface_MAX513X(int _sync_pin, int _din_pin, int _sck_pin, int _ldac_pin)
{
    sync_pin = _sync_pin;
    din_pin = _din_pin;
    sck_pin = _sck_pin;
    ldac_pin = _ldac_pin;
}

void Greenface_MAX513X::begin()
{
    data_reset();
}

void Greenface_MAX513X::data_reset()
{
    send_to_device(0x00, MAX513X_CLR);
}

void Greenface_MAX513X::factory_reset()
{
    data_reset();
}

void Greenface_MAX513X::xfer(uint8_t dac_num)
{
    send_to_device(dacs[dac_num], MAX513X_LDAC);
}

void Greenface_MAX513X::dac_out(unsigned int dac_num, unsigned int val)
{
    send_to_device(val, dacs[dac_num]);
    xfer(dac_num);
}

// private stuff. Don't look!
void Greenface_MAX513X::send_to_device(uint16_t val, uint8_t cmd)
{
    byte me; // the data to send

    digitalWrite(sync_pin, LOW);
    shiftOut(din_pin, sck_pin, MSBFIRST, cmd);
    me = val >> 8;
    shiftOut(din_pin, sck_pin, MSBFIRST, me);
    me = val & 0xff;
    shiftOut(din_pin, sck_pin, MSBFIRST, me);
    digitalWrite(sync_pin, HIGH);
}
