#include "pic.hpp"
#include "io.hpp"

PIC masterPIC(0x20);
PIC slavePIC(0xA0);

PIC::PIC(uint16_t port) : 
	port(port)
{

}

void PIC::initialize(uint16_t irqBase, uint16_t icw3, uint16_t icw4)
{
	outb(this->port + 0x00, 0x11);
	outb(this->port + 0x01, irqBase);
	outb(this->port + 0x01, icw3);
	outb(this->port + 0x01, icw4);
}

void PIC::maskInterrupts(uint8_t mask)
{
	outb(this->port + 0x01, mask);
}

void PIC::sendEndOfInterrupt()
{
	outb(this->port + 0x00, 0x20);
}