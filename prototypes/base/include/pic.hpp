#pragma once

#include <inttypes.h>

class PIC
{
private:
	uint16_t port;
public:
	PIC(uint16_t port);
	
	void initialize(uint16_t irqBase, uint16_t icw3, uint16_t icw4);
	
	void maskInterrupts(uint8_t mask);
};

extern PIC masterPIC, slavePIC;