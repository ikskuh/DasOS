#include "ata.hpp"

#include "io.hpp"
#include "idt.hpp"
#include "console.hpp"

static inline void wait400NS(uint16_t p) 
{
	inb(p);
	inb(p);
	inb(p);
	inb(p);
}

static bool isDriverInitialized = false;

static void ata_irq(CpuState * & cpu)
{
	// Console::main << "[ATA IRQ: ]" << (uint32_t)inw(ATA_DATA) << "\n";
}

ATADevice::ATADevice(uint16_t basePort, bool master) : 
	master(master),
	ports({
		/*data     =*/(uint16_t)(basePort + 0), 
		/*error    =*/(uint16_t)(basePort + 1),
		/*sectors  =*/(uint16_t)(basePort + 2),
		/*lbaLow   =*/(uint16_t)(basePort + 3),
		/*lbaMid   =*/(uint16_t)(basePort + 4),
		/*lbaHigh  =*/(uint16_t)(basePort + 5),
		/*devSelect=*/(uint16_t)(basePort + 6),
		/*status   =*/(uint16_t)(basePort + 7),
		/*cmd      =*/(uint16_t)(basePort + 7),
		/*control  =*/(uint16_t)(basePort + 518) }),
	sectorCount(-1)
{
	outb(this->ports.control, 0x00);
}

uint8_t ATADevice::status()
{
	wait400NS(ports.status);
	return inb(ports.status);
}

bool ATADevice::isFloating()
{
	return (status() == 0xFF);
}

ATAError ATADevice::initialize()
{
	if(::isDriverInitialized == false) {
		IDT::interrupt(0x2E) = Interrupt(ata_irq);
		::isDriverInitialized = true;
	}

	if(this->isFloating()) {
		return ATAError::FloatingDevice;
	}
	// To use the IDENTIFY command, select a target drive by sending
	// 0xA0 for the master drive, or
	// 0xB0 for the slave, to the "drive select" IO port.
	if(master)
		outb(ports.devSelect, 0xA0); // Select Master
	else
		outb(ports.devSelect, 0xB0); // Select Master
	
	// Then set the Sectorcount, LBAlo, LBAmid, and LBAhi IO ports to 0
	outb(ports.sectors, 0);
	outb(ports.lbaLow, 0);
	outb(ports.lbaMid, 0);
	outb(ports.lbaHigh, 0);
	
	// Then send the IDENTIFY command (0xEC) to the Command IO port.
	outb(ports.cmd, 0xEC);
	
	// Then read the Status port again. If the value read is 0, the drive does not 
	// exist.
	if(status() == 0x00) {
		Console::main << "IDENTIFY failed with STATUS = 0.\n";
		return ATAError::InitializationFailed;
	}
	
	// For any other value: poll the Status port (0x1F7) until bit 7 (BSY, value = 0x80)
	// clears. Because of some ATAPI drives that do not follow spec, at this point you
	// need to check the LBAmid and LBAhi ports (0x1F4 and 0x1F5) to see if they are
	// non-zero. If so, the drive is not ATA, and you should stop polling. Otherwise,
	// continue polling one of the Status ports until bit 3 (DRQ, value = 8) sets,
	// or until bit 0 (ERR, value = 1) sets.
	while(status() & 0x80) {
		Console::main << "devbusy\n";
	}
	
	if((inb(ports.lbaMid) != 0) || (inb(ports.lbaHigh) != 0)) {
		Console::main << (uint32_t)inb(ports.lbaMid) << ", " << (uint32_t)inb(ports.lbaHigh) << "\n";
		Console::main << "IDENTIFY failed with INVALID ATA DEVICE.\n";
		return ATAError::InitializationFailed;
	}
	
	uint8_t status;
	while(((status = this->status()) & 0x9) == 0) {
		Console::main << "devbusy2\n";
	}
	if(status & 0x01) {
		Console::main << "IDENTIFY failed with ATA ERR.\n";
		return ATAError::InitializationFailed;
	}

	// At that point, if ERR is clear, the data is ready to read from the Data port 
	// (0x1F0).
	// Read 256 16-bit values, and store them.
	
	uint16_t ataData[256];
	for(uint32_t i = 0; i < 256; i++) {
		ataData[i] = inw(ports.data);
	}
	
	this->sectorCount = ((uint32_t)ataData[61] << 16 | ataData[60]);
			
	return ATAError::Success;
}

/**
 * @brief Reads a given number of sectors from the device.
 * @param lba The linear block address.
 * @param num The number of blocks on the device.
 */
ATAError ATADevice::read(void *buffer, uint32_t lba, uint32_t num)
{
	if(this->sectorCount < 0) {
		return ATAError::NotInitialized;
	}
	if(num <= 0) {
		return ATAError::InvalidBlockCount;
	}
	uint16_t * ptr = (uint16_t*)buffer;
	
	if(this->master)
		outb(ports.devSelect, 0xE0);
	else
		outb(ports.devSelect, 0xF0);
	outb(ports.sectors, num);
	outb(ports.lbaLow, lba);
	outb(ports.lbaMid, lba >> 8);
	outb(ports.lbaHigh, lba >> 16);
	outb(ports.cmd, 0x20);
	
	uint32_t off = 0;
	while(num-- > 0) {
		uint8_t status;
		while(((status = this->status()) & 0x9) == 0);
		if(status & 0x01) {
			Console::main << "Failed to read from ATA0.\n";
			return ATAError::GenericError;
		}
		
		for(uint32_t i = 0; i < 256; i++) {
			ptr[off++] = inw(ports.data);
		}
	}
	return ATAError::Success;
}

ATAError ATADevice::getSectorCount(uint32_t *count) { 
	if(this->sectorCount <= 0) {
		return ATAError::NotInitialized;
	}
	if(count != NULL) {
		*count = (uint32_t)this->sectorCount;
	}
	return ATAError::Success;
}