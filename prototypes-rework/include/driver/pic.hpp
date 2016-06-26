#pragma once

#include <stdint.h>

class PIC
{
private:
	uint16_t port;
public:
	PIC(uint16_t port);
	
	/**
	 * Sends an initialization code to the PIC.s
	 */
	void initialize(uint16_t irqBase, uint16_t icw3, uint16_t icw4);
	
	/**
	 * Disables all interrupts where the corresponding bit in mask is set.
	 */
	void maskInterrupts(uint8_t mask);
	
	/**
	 * Sends an end-of-interrupt to the PIC.
	 */
	void sendEndOfInterrupt();
};

extern PIC masterPIC, slavePIC;