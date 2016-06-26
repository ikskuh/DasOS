#pragma once

#include <stdint.h>

#define SERIAL_COM1 0x3F8
#define SERIAL_COM2 0x2F8
#define SERIAL_COM3 0x3E8
#define SERIAL_COM4 0x2E8

enum class Partiy
{
	None = 0x0,
	Even = 0x4,
	Odd  = 0x6,
	High = 0x5,
	Low  = 0x7,
};

class SerialPort
{
private:
	uint16_t mBase;
public:
	SerialPort(
		uint32_t portBase, 
		uint32_t baud = 9600, 
		Partiy parity = Partiy::None,
		uint8_t dataBits = 8);
	
	bool isTransmitEmpty() const;
	
	bool isReceiveEmpty() const;
	
	void write(uint8_t c);
	
	uint8_t read();
};