#include "io.hpp"
#include "idt.hpp"
#include "console.hpp"

typedef uint16_t port_t;

const port_t ATA_DATA      = 0x1F0; //	0x170	0x1E8	0x168	R/W
const port_t ATA_ERR       = 0x1F1; //	0x171	0x1E9	0x169	R
const port_t ATA_SECTORS   = 0x1F2; //	0x172	0x1EA	0x16A	R/W
const port_t ATA_LBA_LOW   = 0x1F3; //	0x173	0x1EB	0x16B	R/W
const port_t ATA_LBA_MID   = 0x1F4; //	0x174	0x1EC	0x16C	R/W
const port_t ATA_LBA_HIGH  = 0x1F5; //	0x175	0x1ED	0x16D	R/W
const port_t ATA_DEVSELECT = 0x1F6; //	0x176	0x1EE	0x16E	R/W
const port_t ATA_STATUS    = 0x1F7; //	0x177	0x1EF	0x16F	R(Status) / W(Kommando)
const port_t ATA_CMD       = 0x1F7; //	0x177	0x1EF	0x16F	R(Status) / W(Kommando)
const port_t ATA_STATUS1   = 0x3F6; //	0x376	0x3EE	0x36E	R(Status2) / W(Steuer)
const port_t ATA_CONTROL   = 0x3F6; //	0x376	0x3EE	0x36E	R(Status2) / W(Steuer)

void ata_irq(CpuState * & cpu)
{
	// Console::main << "[ATA IRQ: ]" << (uint32_t)inw(ATA_DATA) << "\n";
}

static inline void wait400NS(uint16_t p) 
{
	inb(p); 
	inb(p); 
	inb(p); 
	inb(p); 
}

static uint8_t ataStatus()
{
	wait400NS(ATA_STATUS);
	return inb(ATA_STATUS);
}

static void read_sectors(void *dst, uint32_t lba, uint32_t numSectors)
{
	if(numSectors < 0) {
		return;
	}
	uint16_t * ptr = (uint16_t*)dst;
	
	outb(ATA_DEVSELECT, 0xE0);
	outb(ATA_SECTORS, numSectors);
	outb(ATA_LBA_LOW, lba);
	outb(ATA_LBA_MID, lba >> 8);
	outb(ATA_LBA_HIGH, lba >> 16);
	outb(ATA_CMD, 0x20);
	
	uint32_t off = 0;
	while(numSectors-- > 0) {
		uint8_t status;
		while(((status = ataStatus()) & 0x9) == 0);
		if(status & 0x01) {
			Console::main << "Failed to read from ATA0.\n";
			return;
		}
		
		for(uint32_t i = 0; i < 256; i++) {
			ptr[off++] = inw(ATA_DATA);
		}
		Console::main << "ATA0 sector ready.\n";
	}
}

void dasos_demo()
{
	IDT::interrupt(0x2E) = Interrupt(ata_irq);

	Console::main << "Detecting ATA0 master.\n";

	outb(ATA_CONTROL, 0x00);
	
	if(ataStatus() == 0xFF) {
		Console::main << "ERR: Floating bus detected.\n";
		return;
	}
	
	// TODO: Are timings required here?
	
	outb(ATA_DEVSELECT, 0xE7);
	outb(ATA_LBA_LOW, 0x12);
	outb(ATA_LBA_MID, 0x34);
	outb(ATA_LBA_HIGH, 0x56);
	
	if(inb(ATA_LBA_LOW) != 0x12 ||
		 inb(ATA_LBA_MID) != 0x34 ||
		 inb(ATA_LBA_HIGH) != 0x56) {
		Console::main << "ERR: Non-responsive device.\n";
		return;
	}
	
	// IDENTIFY
	
	// To use the IDENTIFY command, select a target drive by sending
	// 0xA0 for the master drive, or
	// 0xB0 for the slave, to the "drive select" IO port.
	outb(ATA_DEVSELECT, 0xA0); // Select Master
	
	// Then set the Sectorcount, LBAlo, LBAmid, and LBAhi IO ports to 0
	outb(ATA_SECTORS, 0);
	outb(ATA_LBA_LOW, 0);
	outb(ATA_LBA_MID, 0);
	outb(ATA_LBA_HIGH, 0);
	
	// Then send the IDENTIFY command (0xEC) to the Command IO port.
	outb(ATA_CMD, 0xEC);
	
	// Then read the Status port again. If the value read is 0, the drive does not 
	// exist.
	if(ataStatus() == 0x00) {
		Console::main << "IDENTIFY failed with STATUS = 0.\n";
		return;
	}
	
	// For any other value: poll the Status port (0x1F7) until bit 7 (BSY, value = 0x80)
	// clears. Because of some ATAPI drives that do not follow spec, at this point you
	// need to check the LBAmid and LBAhi ports (0x1F4 and 0x1F5) to see if they are
	// non-zero. If so, the drive is not ATA, and you should stop polling. Otherwise,
	// continue polling one of the Status ports until bit 3 (DRQ, value = 8) sets,
	// or until bit 0 (ERR, value = 1) sets.
	while(ataStatus() & 0x80);
	
	if((inb(ATA_LBA_MID) != 0) || (inb(ATA_LBA_HIGH) != 0)) {
		Console::main << (uint32_t)inb(ATA_LBA_MID) << ", " << (uint32_t)inb(ATA_LBA_HIGH) << "\n";
		Console::main << "IDENTIFY failed with INVALID ATA DEVICE.\n";
		return;
	}
	
	uint8_t status;
	while(((status = ataStatus()) & 0x9) == 0);
	if(status & 0x01) {
		Console::main << "IDENTIFY failed with ATA ERR.\n";
		return;
	}

	// At that point, if ERR is clear, the data is ready to read from the Data port 
	// (0x1F0).
	// Read 256 16-bit values, and store them.
	
	uint16_t ataData[256];
	for(uint32_t i = 0; i < 256; i++) {
		ataData[i] = inw(ATA_DATA);
	}
	
	uint32_t size = ((uint32_t)ataData[61] << 16 | ataData[60]);
	
	Console::main << "ATA0 Master available:\n";
	Console::main
		<< "LBA24 Size: " << size << " sectors, " << (size>>1) << " kB" << "\n";

	Console::main << "Read sector 0...\n";
		
	uint16_t sectorData[256];
	
	read_sectors(sectorData, 0x00, 0x01);
	
	if(sectorData[255] == 0xAA55) {
		Console::main << "Sector is MBR.\n";
	}
	
	while(true);

}
