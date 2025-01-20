/*
  Greenface_MAX513X - DAC library
  Copyright (c) 2022 Shannon Vance.  All right reserved.
  Released under Creative Commons Attribution-ShareAlike 2.5 Generic License
*/

// ensure this library description is only included once
#ifndef Greenface_MAX513X_h
#define Greenface_MAX513X_h

// commands
#define MAX513X_NOP 0x00
#define MAX513X_LDAC 0x01
#define MAX513X_CLR 0x02
#define MAX513X_PWR 0x03
#define MAX513X_LIN 0x03

#define MAX513X_WRT3 0x18
#define MAX513X_WRT2 0x14
#define MAX513X_WRT1 0x12
#define MAX513X_WRT0 0x11

#define MAX513X_WRT_THRU3 0X38
#define MAX513X_WRT_THRU2 0X34
#define MAX513X_WRT_THRU1 0X32
#define MAX513X_WRT_THRU0 0X31

// library interface description
class Greenface_MAX513X
{
	// user-accessible "public" interface
public:
	Greenface_MAX513X();
	Greenface_MAX513X(int _sync_pin, int _din_pin, int _sck_pin, int _ldac_pin);

	// int ldac_mode;  // 0 = pass thru, 1 = blocked, 2 = latched
	void begin();
	void dac_out(unsigned int dac_num, unsigned int val);
	void xfer(uint8_t dac_num); // use ldac to set output registers
	void data_reset();
	void factory_reset();
	void power_down();

	// library-accessible "private" interface
private:
	int sync_pin;
	int din_pin;
	int sck_pin;
	int ldac_pin;
	byte dacs[4] = {MAX513X_WRT0, MAX513X_WRT1, MAX513X_WRT2, MAX513X_WRT3};
	void send_to_device(uint16_t val, uint8_t cmd); // raw data
};

#endif
