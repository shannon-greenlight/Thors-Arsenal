#include <Arduino.h>
#include "Greenface_HC595.h"

Greenface_HC595::Greenface_HC595(uint8_t data_pin, uint8_t clock_pin, uint8_t latch_pin)
{
    this->data_pin = data_pin;
    this->clock_pin = clock_pin;
    this->latch_pin = latch_pin;
    pinMode(data_pin, OUTPUT);
    pinMode(clock_pin, OUTPUT);
    digitalWrite(clock_pin, LOW);
    if (latch_pin != 0)
    {
        pinMode(latch_pin, OUTPUT);
        digitalWrite(latch_pin, HIGH);
    }
}

void Greenface_HC595::set(uint8_t channel, bool value)
{
    byte mask = 1 << channel;
    byte_value &= ~mask; // clear the bit
    if (value)
    {
        byte_value |= mask; // set the bit
    }
    send_it();
}

void Greenface_HC595::set_byte(byte value)
{
    byte_value = value;
    send_it();
}

void Greenface_HC595::send_it()
{
    if (latch_pin)
        digitalWrite(latch_pin, LOW);
    shiftOut(data_pin, clock_pin, MSBFIRST, byte_value);
    if (latch_pin)
        digitalWrite(latch_pin, HIGH);
}