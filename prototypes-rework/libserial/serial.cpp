#include <driver/serial.hpp>
#include <io.hpp>

// Code adapted from:
// http://www.lowlevel.eu/wiki/Serielle_Schnittstelle

#define IER 1
#define IIR 2
#define FCR 2
#define LCR 3
#define MCR 4
#define LSR 5
#define MSR 6

SerialPort::SerialPort(uint32_t portBase, uint32_t baud, Partiy parity, uint8_t dataBits) : 
	mBase(portBase)
{
	union {
		uint8_t b[2];
		uint16_t w;
	} divisor;
	divisor.w = 115200 / baud;

	// Interrupt ausschalten
	outb(this->mBase + IER, 0x00);

	// DLAB-Bit setzen
	outb(this->mBase + LCR, 0x80);

	// Teiler (low) setzen
	outb(this->mBase + 0, divisor.b[0]);

	// Teiler (high) setzen
	outb(this->mBase + 1, divisor.b[1]);

	int iparity = (int)parity;
	
	// Anzahl Bits, Parität, usw setzen (DLAB zurücksetzen)
	outb(this->mBase + LCR, ((iparity&0x7)<<3)|((dataBits-5)&0x3));

	// Initialisierung abschließen
	outb(this->mBase + FCR, 0xC7);
	outb(this->mBase + MCR, 0x0B);
}

bool SerialPort::isTransmitEmpty() const
{
	return inb(this->mBase + LSR) & 0x20;
}

bool SerialPort::isReceiveEmpty() const
{
	return (inb(this->mBase + LSR) & 1) == 0;
}

void SerialPort::write(uint8_t c)
{
	while (this->isTransmitEmpty() == false);
	outb(this->mBase, c);
}

uint8_t SerialPort::read()
{
	while (this->isReceiveEmpty());
	return inb(this->mBase);
}
