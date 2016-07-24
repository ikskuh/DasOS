#include "console.hpp"

#include "ata.hpp"

typedef struct {
	char oemName[8];
	uint16_t sectorSize;
	uint8_t clusterSize;
	uint16_t reservedSectors;
	uint8_t numTables;
	uint16_t rootMaxSize;
	uint16_t numSectors;
	uint8_t mediaType;
	uint16_t fatSize;
	uint16_t trackSize;
	uint16_t numHeads;
	uint32_t sectorOffset;
	uint32_t numSectorsLarge;
} __attribute__ ((packed)) fatheader_t;

typedef struct {
	uint8_t biosDriveNum;
	uint8_t reserved;
	uint8_t extendedBootSignature; // should be 0x29
	uint32_t filesystemID;
	char filesystemName[11];
	char fatName[8];
} __attribute__ ((packed)) fat16header_t;

#define TEST(x) if((x) != ATAError::Success) { Console::main << #x " << failed.\n"; while(true); }

void fat_test(ATADevice & ata)
{
	uint8_t bootsector[512];

	TEST(ata.read(bootsector, 0x00, 0x01))

	fatheader_t *header = (fatheader_t*)&bootsector[0x03];
	
	uint16_t *magic = (uint16_t*)&bootsector[0x1FE];
	if(*magic != 0xAA55) {
		Console::main << "Invalid boot magic.\n";
		return;
	}
	
	Console::main << "OEM Name: ";
	Console::main.write(header->oemName, 8);
	Console::main << "\n";
	
	using namespace console_tools;
	Console::main
		<< "Sector Size:      " << (uint32_t)header->sectorSize << "\n"
		<< "Cluster Size:     " << (uint32_t)header->clusterSize << "\n"
		<< "Reserved Sectors: " << (uint32_t)header->reservedSectors << "\n"
		<< "Num FATs:         " << (uint32_t)header->numTables << "\n"
		<< "Max(ROOT):        " << (uint32_t)header->rootMaxSize << "\n"
		<< "Num Sectors:      " << (uint32_t)header->numSectors << "\n"
		<< "Media Type:       " << hex((uint32_t)header->mediaType) << "\n"
		<< "FAT Size:         " << (uint32_t)header->fatSize << "\n"
		<< "Track Size:       " << (uint32_t)header->trackSize << "\n"
		<< "Num Heads:        " << (uint32_t)header->numHeads << "\n"
		<< "Sector offset:    " << (uint32_t)header->sectorOffset << "\n"
		<< "Num Sectors(L):   " << (uint32_t)header->numSectorsLarge << "\n"
		;
	if(header->fatSize == 0) {
		Console::main << "FAT32 is not supported.\n";
		return;
	}
	
	fat16header_t *header16 = (fat16header_t*)&bootsector[0x24];
	
	Console::main
		<< "BIOS Drive:       " << (uint32_t)header16->biosDriveNum << "\n"
		<< "Boot Signature:   " << hex((uint32_t)header16->extendedBootSignature) << "\n"
		<< "Filesystem ID:    " << hex((uint32_t)header16->filesystemID) << "\n"
		<< "Filesystem Name:  ";
	Console::main.write(header16->filesystemName, 11);
	Console::main << "\nFAT Variant:      ";
	Console::main.write(header16->fatName, 8);
	Console::main << "\n";
	
	if(header16->extendedBootSignature != 0x29) {
		Console::main << "Invalid extended boot signature.\n";
		return;
	}
}

























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
		
	fat_test(ata0);
	
	while(true);

}
