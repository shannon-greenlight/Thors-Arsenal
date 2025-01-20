#ifndef Greenface_HC595_h
#define Greenface_HC595_h
class Greenface_HC595
{
public:
    Greenface_HC595(uint8_t data_pin, uint8_t clock_pin, uint8_t latch_pin = 0);
    void set(uint8_t channel, bool value);
    void set_byte(byte value);

private:
    byte byte_value;
    uint8_t data_pin;
    uint8_t clock_pin;
    uint8_t latch_pin;

    void send_it();
};
#endif