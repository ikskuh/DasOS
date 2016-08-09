#include "console.hpp"
#include "ata.hpp"
#include "fat.h"
#include "fs.h"

#define TEST(x) if((x) != ATAError::Success) { Console::main << #x " << failed.\n"; while(true); }

unsigned char FAT_table[512 * 9];

uint32_t get_next_cluster(uint32_t cluster)
{
	uint32_t first_fat_sector = 0;
	uint32_t section_size = 2048;
	unsigned int fat_offset = cluster + (cluster / 2);// multiply by 1.5
	unsigned int fat_sector = first_fat_sector + (fat_offset / section_size);
	unsigned int ent_offset = fat_offset % section_size;
	 
	//at this point you need to read from sector "fat_sector" on the disk into "FAT_table".
	uint16_t *ptr = (uint16_t*)&FAT_table[ent_offset];
	unsigned short table_value = *ptr;
	if(cluster & 0x0001)
		table_value = table_value >> 4;
	else
		table_value = table_value & 0x0FFF;
	return table_value;
}

void load_file(ATADevice & ata, directory_t const & file, uint32_t off)
{
	static uint8_t loadFileBlock[512];
	Console::main << "BEGIN(READ_FILE)\n";
	
	uint32_t currentCluster = (file.firstClusterH << 16) | file.firstClusterL;
	int32_t size = file.size;
	Console::main
		<< "Size:           " << size << "\n"
		<< "First Cluster:  " << currentCluster << "\n"
		<< "Cluster Offset: " << off << "\n"
		;
	
	Console::main << "DATA:\n";
	
	while(size > 0) {
	
		TEST(ata.read(loadFileBlock, off + currentCluster, 0x01))
		
		Console::main.write(
			loadFileBlock,
			size > 512 ? 512 : size);
		
		currentCluster = get_next_cluster(currentCluster);
		
		if(currentCluster >= 0xFF8) {
			break;
		}
	
		size -= 512;
	}
	
	Console::main << "\nRemaining size: " << size << "\n";
	
	Console::main << "END(READ_FILE)\n";
}

bool strcmpn(char const *a, char const *b, int len) 
{
	for(int i = 0; i < len; i++) {
		if(a[i] != b[i]) return false;
	}
	return true;
}

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
	
	
	uint32_t rootLen = sizeof(directory_t) * header->rootMaxSize;
	uint8_t directory[rootLen];
	
	uint32_t fileStart = 
		header->reservedSectors + 
		header->numTables * header->fatSize + 
		(rootLen >> 9) -
		2;
	
	Console::main << "Read total FAT...\n";
	TEST(ata.read(FAT_table, header->reservedSectors, header->fatSize))
	
	TEST(ata.read(directory, header->reservedSectors + header->numTables * header->fatSize, rootLen >> 9))
	
	Console::main << rootLen << ", " << (rootLen >> 9) << "\n";
	
	directory_t *dir = (directory_t*)directory;
	for(uint32_t i = 0; i < header->rootMaxSize; i++, dir++) {
		
		// stop on empty 
		if(dir->name[0] == 0x00) {
			break;
		}
		// skip removed
		if(dir->name[0] == 0xE5) {
			continue;
		}
		
		Console::main << "FILE[" << i << "] ";
		Console::main.write(&dir->name[0], 8);
		Console::main.put('.');
		Console::main.write(&dir->name[8], 3);
		Console::main.put(' ');
		
		const char *flagNames = "RHSVDA67";
		
		if(dir->flags == 0x0F) {
			Console::main << "long names not supported.\n";
			continue;
		}
		
		for(int i = 0; i < 8; i++)  {
			if(dir->flags & (1<<i)) {
				Console::main.put(flagNames[i]);
			} else {
				Console::main.put(' ');
			}
		}
		
		Console::main 
			<< " " << bin((uint32_t)dir->flags) 
			<< " " << (uint32_t)dir->firstClusterL 
			<< " " << (uint32_t)dir->size << "\n";
		
		if(strcmpn((char*)dir->name, "DEMO    CPP", 11)) {
			load_file(ata, *dir, fileStart);
		}
	}
}
	


void dasos_demo()
{
	/*
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
	*/
	
	fs_init();
	
	int fd = fs_open("C:/resource");
	
	Console::main << "Opened file " << (int32_t)fd << " of type ";
	
	enum filetype type = fs_type(fd);
	switch(type)
	{
		case ftFile: Console::main << "file"; break;
		case ftDirectory: Console::main << "directory"; break;
		case ftInvalid: Console::main << "invalid"; break;
		case ftUnknown: Console::main << "unknown"; break;
		default: Console::main << "???"; break;
	}
	Console::main << "\n";
	
	struct node info;
	if(fs_info(fd, &info)) {
		Console::main << "File Name:" << info.name << "\n";
		if(info.type != type)  {
			Console::main << "File type mismatch!\n";
		}
	} else {
		Console::main << "Failed to get file info.\n";
	}
	
	if(type == ftFile)
	{
		// dump the file to the console
		
	}
	else if(type == ftDirectory)
	{
		// Do a directory listing
		uint32_t count = dir_length(fd);
		for(uint32_t i = 0; i < count; i++) {
			Console::main << "[" << i << "] ";
			
			struct node entry;
			if(dir_get(fd, i, &entry) == false) {
				Console::main << "INVALID\n";
				continue;
			}
			switch(entry.type) {
				case ftInvalid: Console::main << "INV "; break;
				case ftFile: Console::main << "FILE"; break;
				case ftDirectory: Console::main << "DIR "; break;
				case ftUnknown: Console::main << "UNKN"; break;
				default: Console::main << "??? "; break;
			}
			Console::main << " '" << entry.name << "'\n";
		}
	}
	
	while(true);

}
