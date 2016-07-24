#include "console.hpp"

#include "ata.hpp"

typedef uint16_t port_t;

void dasos_demo()
{
	Console::main << "Detecting ATA0 master.\n";
	
	ATADevice ata0(0x1F0, true);

	Console::main << "Check for availability...\n";
	
	if(ata0.isFloating()) {
		Console::main << "ERR: Floating bus detected.\n";
		return;
	}
	
	if(ata0.initialize() != ATAError::Success) {
		Console::main << "ATA0 not available.\n";
		return;
	}
	
	Console::main << "Read sector 0...\n";
		
	uint16_t sectorData[256];
	
	if(ata0.read(sectorData, 0x00, 0x01) != ATAError::Success) {
		Console::main << "Failed to read sector from ATA0.\n";
		return;
	}
	
	if(sectorData[255] == 0xAA55) {
		Console::main << "Sector is MBR.\n";
	}
	
	while(true);

}
