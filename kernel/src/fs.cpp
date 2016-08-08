#include "console.hpp"

#include "ata.hpp"
#include "fat.h"
#include "fs.h"

#include <stddef.h>

#define FHB_MAX 16

#define ATA_CHECKED(x) if((x) != ATAError::Success) { Console::main << #x " << failed.\n"; while(true); }

struct FileHandleBackend
{
	bool isOpen;
	enum filetype type;
	struct directory entry;
};

FileHandleBackend files[FHB_MAX + 1]; // 0 is invalid.

ATADevice ata(0x1F0, true);

struct directory rootDirectory;

struct fatheader   FATHeader;
struct fatheader16 FATExtendedHeader;

uint8_t FAT[512 * 9];

void fs_init()
{
	if(ata.isFloating()) {
		Console::main << "ERR: Floating bus detected.\n";
		return;
	}
	
	if(ata.initialize() != ATAError::Success) {
		Console::main << "ATA0 not available.\n";
		return;
	}
	
	static uint8_t bootsector[512];

	ATA_CHECKED(ata.read(bootsector, 0x00, 0x01))

	uint16_t *magic = (uint16_t*)&bootsector[0x1FE];
	if(*magic != 0xAA55) {
		Console::main << "Invalid boot magic.\n";
		return;
	}
	
	{
		fatheader_t *header = (fatheader_t*)&bootsector[0x03];
		FATHeader = *header;
	}
	
	if(FATHeader.fatSize == 0) {
		Console::main << "FAT32 is not supported.\n";
		return;
	}
	
	{
		fat16header_t *header16 = (fat16header_t*)&bootsector[0x24];	
		FATExtendedHeader = *header16;
	}
	if(FATExtendedHeader.extendedBootSignature != 0x29) {
		Console::main << "Invalid extended boot signature.\n";
		return;
	}
	
	
	// uint32_t rootLen = sizeof(directory_t) * FATHeader.rootMaxSize;
	// uint8_t directory[rootLen];
	
	/*
	uint32_t fileStart = 
		FATHeader.reservedSectors + 
		FATHeader.numTables * FATHeader.fatSize + 
		(rootLen >> 9) -
		2;
	*/
	
	Console::main << "Read total FAT...\n";
	ATA_CHECKED(ata.read(FAT, FATHeader.reservedSectors, FATHeader.fatSize))
	
	{
		uint32_t rootStart = FATHeader.reservedSectors + FATHeader.numTables * FATHeader.fatSize;
		rootDirectory = (struct directory) {
			"ROOTDIRDIR", // uint8_t name[11];
			ffSystem | ffDirectory, // uint8_t flags;
			0, // uint8_t ntReserved;
			0, // uint8_t creationTimeFine;
			0, // uint16_t creationTime;
			0, // uint16_t creationDate;
			0, // uint16_t lastAccessDate;
			(uint16_t)(rootStart >> 16), // uint16_t firstClusterH;
			0, // uint16_t writeTime;
			0, // uint16_t writeDate;
			(uint16_t)(rootStart & 0xFFFF), // uint16_t firstClusterL;
			0, // uint32_t size;
		};
	}
}

static char toUpper(char c)
{
	if(c >= 'a' && c <= 'z') {
		return (c - 'a') + 'A';
	} else {
		return c;
	}
}

static char const * extract_first_name(char *fileName, char const * path)
{
	if(path[0] != '/')
		return NULL;
	path++;
	int cursor = 0;
	while(true)
	{
		if(cursor >= 12) {
			return NULL;
		}
		if(*path == 0 || *path == '/') {
			break;
		}
		char c = toUpper(*path++);
		if(c == '.') {
			Console::main << "path with .\n";
			// pad with spaces
			for(; cursor < 8; cursor++) {
				fileName[cursor] = ' ';
			}
		} else {
			fileName[cursor++] = c;
		}
	}
	// pad with spaces
	for(; cursor < 11; cursor++) {
		fileName[cursor] = ' ';
	}
	fileName[11] = 0;
	return path;
}

extern "C" int fs_open(char const * path)
{
	if(path[0] != 'C' || path[1] != ':') return 0; /* only one drive supported */
	path += 2; // skip drive specification
	if(path[0] != '/') return 0;
	
	// todo: find the correct directory_t entry
	struct directory entry = rootDirectory;
	
	if(path[1] != 0)
	{	
		char fileName[FILENAME_MAXLEN];
		
		while(path != NULL)
		{
			if((path = extract_first_name(fileName, path)) == NULL)
			{
				return 0;
			}
			Console::main << "FN:  '" << fileName << "'\n";
			Console::main << "PTH: '" << path     << "'\n";
		
		}
	}
	
	// Allocate a file entry.
	for(int i = 1; i <= FHB_MAX; i++)
	{
		if(files[i].isOpen) {
			continue;
		}
		files[i].isOpen = true;
		files[i].entry = entry;
		files[i].type = ftUnknown;
		if(entry.flags & ffDirectory) {
			files[i].type = ftDirectory;
		} else if ((entry.flags & ffVolumeID) == 0) {
			files[i].type = ftDirectory;
		}
		
		return i;
	}
	
	return 0;
}

extern "C" enum filetype fs_type(int file)
{
	if(file > FHB_MAX)
		return ftInvalid;
	if(files[file].isOpen)
		return files[file].type;
	else
		return ftInvalid;
}

extern "C" void fs_close(int file)
{
	if(file <= FHB_MAX)
		files[file].isOpen = false; // Am i doing this right, daddy?
}

extern "C" uint32_t file_read(int file, void *buffer,	uint32_t offset, uint32_t length);

extern "C" uint32_t file_length(int file);

extern "C" uint32_t dir_length(int file);

extern "C" bool dir_get(int file, int index, struct node * node);