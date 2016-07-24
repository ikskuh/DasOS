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

typedef struct {
	char name[11];
	uint8_t flags;
	uint8_t ntReserved;
	uint8_t creationTimeFine;
	uint16_t creationTime;
	uint16_t creationDate;
	uint16_t lastAccessDate;
	uint16_t firstClusterH;
	uint16_t writeTime;
	uint16_t writeDate;
	uint16_t firstClusterL;
	uint32_t size;
} __attribute__ ((packed))  directory_t;

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
	
	
	uint8_t directory[512];
	
	TEST(ata.read(directory, header->reservedSectors + header->numTables * header->fatSize, 0x01))
	
	Console::main << sizeof(directory_t) << "\n";
	
	directory_t *dir =  (directory_t*)directory;
	for(uint32_t i = 0; i < 10; i++, dir++) {
		
		// skip            empty                   removed
		if(dir->name[0] == 0x00 || dir->name[0] == 0xE5) {
			continue;
		}
		
		Console::main << "FILE[" << i << "] ";
		Console::main.write(&dir->name[0], 8);
		Console::main.put('.');
		Console::main.write(&dir->name[8], 3);
		
		Console::main 
			<< " " << bin((uint32_t)dir->flags) 
			<< " " << (uint32_t)dir->firstClusterL 
			<< " " << (uint32_t)dir->size << "\n";
	
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
