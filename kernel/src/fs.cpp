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
	bool isRootDir;
	enum filetype type;
	struct directory entry;
	uint32_t length; // number of entries in directory or file length in bytes
};

FileHandleBackend files[FHB_MAX + 1]; // 0 is invalid.

ATADevice ata(0x1F0, true);

struct directory rootDirectory;

struct fatheader   FATHeader;
struct fatheader16 FATExtendedHeader;

#define SECTOR_SIZE (FATHeader.sectorSize)
#define CLUSTER_SIZE (SECTOR_SIZE * FATHeader.clusterSize)

uint8_t FAT[512 * 16];

uint32_t firstClusterSector;
uint32_t invalidClusterIndex;

#define CACHE_SIZE 64
#define CACHE_INVALID_SECTOR 0 // Boot sector will not be cached.

struct
{
	uint32_t sector;
	uint32_t age;
	uint8_t data[1024];
} cache[CACHE_SIZE];

static inline void memcpy(void *dst, void const * src, uint32_t len)
{
	char *d = (char*)dst;
	char const *s = (char const*)src;
	while(len-- > 0)
	{
		*d++ = *s++;
	}
}

static void cache_age(int exclude)
{
	for(int i = 0; i < CACHE_SIZE; i++)
	{
		if(i == exclude) continue;
		cache[i].age += 1;
	}
	cache[exclude].age = 0;
}

static int cache_oldest()
{
	int idx = 0;
	for(int i = 1; i < CACHE_SIZE; i++)
	{
		if(cache[idx].age < cache[i].age) {
			idx = i;
		}
	}
	return idx;
}

static void read_cluster(void *memory, uint32_t sector)
{
	for(int i = 0; i < CACHE_SIZE; i++)
	{
		if(cache[i].sector == sector) {
			// Console::main << "[CR:" << sector << "]";
			memcpy(memory, cache[i].data, SECTOR_SIZE);
			cache_age(i); // Age all other cache entries
			return;
		}
	}
	// Console::main << "[CW:" << sector << "]";
	int idx = cache_oldest();
	ATA_CHECKED(ata.read(cache[idx].data, sector, FATHeader.clusterSize))
	cache[idx].sector = sector;
	cache_age(idx);
	memcpy(memory, cache[idx].data, SECTOR_SIZE);
}

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
	
	Console::main << "FAT Sector Count: " << (uint32_t)FATHeader.fatSize << "\n";
	
	uint32_t rootLen = sizeof(directory_t) * FATHeader.rootMaxSize;
	{ // Initialize FAT sector offset
		firstClusterSector = 
			FATHeader.reservedSectors + 
			FATHeader.numTables * FATHeader.fatSize + 
			(rootLen / CLUSTER_SIZE) -
			2;
		Console::main << "Start of clusters:" << firstClusterSector << "\n";
	}
	{ // Initialize root directory
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
	
	invalidClusterIndex = 0xFF8; // Change for other FATs
	
	{ // initialize cache
		for(int i = 0; i < CACHE_SIZE; i++)
		{
			cache[i].sector = CACHE_INVALID_SECTOR;
		}
	}
}

static bool strcmpn(char const *a, char const *b, int len) 
{
	for(int i = 0; i < len; i++) {
		if(a[i] != b[i]) return false;
	}
	return true;
}

static uint32_t getCluster(struct directory *dir)
{
	return (dir->firstClusterH << 16) | dir->firstClusterL;
}

static struct directory get_directory_entry(
	struct directory *dir, 
	uint32_t index,
	uint32_t (*getNextCluster)(uint32_t sector),
	uint32_t (*clusterToSector)(uint32_t addr))
{
	const uint32_t clusterLength = (CLUSTER_SIZE / sizeof(struct directory));
	
	uint32_t cluster = getCluster(dir);
	
	uint32_t clusterIndex = index / clusterLength;
	uint32_t clusterOffset = index % clusterLength;
	
	/*
	Console::main
		<< "Start Cluster: " << cluster << "\n"
		<< "Cluster Index: " << clusterIndex << "\n"
		<< "Cluster Offset: " << clusterOffset << "\n";
	//*/
	
	for(uint32_t i = 0; i < clusterIndex; i++) {
		uint32_t next = getNextCluster(cluster);;
		// Console::main << "[" << cluster << "] -> [" << next << "]\n";
		cluster = next;
		// End Marker
		if(cluster >= invalidClusterIndex) {
			return (struct directory){ 0 };
		}
	}
	
	struct directory entries[clusterLength];
	
	uint32_t sector = clusterToSector(cluster);
	read_cluster(entries, sector);
	
	return entries[clusterOffset];
}

static struct directory search_directory(
	struct directory *dir, 
	char const * name,
	uint32_t (*getNextCluster)(uint32_t sector),
	uint32_t (*clusterToSector)(uint32_t addr))
{
	for(uint32_t i = 0; ; i++)
	{
		struct directory result = get_directory_entry(dir, i, getNextCluster, clusterToSector);
		if(result.name[0] == 0) {
			return result;
		}
		if(result.name[0] == 0xE5) {
			// skip deleted entries
			continue;
		}
		if(result.flags == ffLongName) {
			// todo: implement long name support.
			// store long name here
			continue;
		}
		if(strcmpn((char*)result.name, name, 11) == true) {
			// todo: apply long name here
			return result;
		}
		// todo: reset long name here
	}
}


static uint32_t get_directory_length(
	struct directory *dir, 
	uint32_t (*getNextCluster)(uint32_t sector),
	uint32_t (*clusterToSector)(uint32_t addr))
{
	for(uint32_t i = 0; ; i++)
	{
		struct directory result = get_directory_entry(dir, i, getNextCluster, clusterToSector);
		if(result.name[0] == 0) {
			return i;
		}
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

static uint32_t increment(uint32_t i) { return i + 1; }

static uint32_t identity(uint32_t addr) { return addr; }

static uint32_t getNextClusterFat12(uint32_t cluster)
{
	uint32_t first_fat_sector = 0;
	uint32_t section_size = 2048;
	unsigned int fat_offset = cluster + (cluster / 2);// multiply by 1.5
	unsigned int fat_sector = first_fat_sector + (fat_offset / section_size);
	unsigned int ent_offset = fat_offset % section_size;
	 
	//at this point you need to read from sector "fat_sector" on the disk into "FAT_table".
	uint16_t *ptr = (uint16_t*)&FAT[ent_offset];
	unsigned short table_value = *ptr;
	if(cluster & 0x0001)
		table_value = table_value >> 4;
	else
		table_value = table_value & 0x0FFF;
	return table_value;
}

static uint32_t getSectorForCluster(uint32_t cluster)
{
	return firstClusterSector + FATHeader.clusterSize * cluster;
}

static enum filetype typeFromEntry(struct directory const * entry)
{
	if(entry->flags & ffDirectory) {
		return ftDirectory;
	} else if ((entry->flags & ffVolumeID) == 0) {
		return ftFile;
	} else {
		return ftUnknown;
	}
}

extern "C" int fs_open(char const * path)
{
	if(path[0] != 'C' || path[1] != ':') return 0; /* only one drive supported */
	path += 2; // skip drive specification
	if(path[0] != '/') return 0;
	
	int fd = 0;
	// Allocate a file entry.
	for(int i = 1; i <= FHB_MAX; i++)
	{
		if(files[i].isOpen) {
			continue;
		}
		fd = i;
		break;
	}
	if(fd == 0) {
		return 0;
	}
	
	// todo: find the correct directory_t entry
	struct directory entry = rootDirectory;
	bool isRootDir = true;
	
	if(path[1] != 0) // Check if we have any specific path
	{	
		char fileName[FILENAME_MAXLEN];
		
		// initialize with root directory search
		uint32_t (*getNextCluster)(uint32_t sector) = &increment;
		uint32_t (*clusterToSector)(uint32_t addr) = &identity;
		while(*path != 0)
		{
			if((entry.flags & ffDirectory) == 0) {
				// trying to index a file, won't work
				Console::main << "The file is not a directory.\n";
				return 0;
			}
			if((path = extract_first_name(fileName, path)) == NULL)
			{
				return 0;
			}
			struct directory next = search_directory(
				&entry,
				fileName,
				getNextCluster, 
				clusterToSector);
			if(next.name[0] == 0x00) {
				Console::main << "file not found.\n";
				return 0;
			}
			/*
			Console::main << "FN:        '" << fileName << "'\n";
			Console::main << "PTH:       '" << path     << "'\n";
			Console::main << "File Name: '";
			Console::main.write(next.name, 11);
			Console::main << "'\n";
			//*/
			entry = next;
			
			// then follow up with Fat12 FAT indexing
			getNextCluster = &getNextClusterFat12;
			clusterToSector = &getSectorForCluster;
			isRootDir = false;
		}
		// Console::main << "open now.\n";
	}
	
	files[fd].isOpen = true;
	files[fd].isRootDir = false;
	files[fd].entry = entry;
	files[fd].type = ftUnknown;
	files[fd].length = 0;
	
	files[fd].type = typeFromEntry(&entry);
	
	if(files[fd].type == ftDirectory) {
		files[fd].isRootDir = isRootDir;
		if(isRootDir) {
			files[fd].length = get_directory_length(&entry, &increment, &identity);
		} else {
			files[fd].length = get_directory_length(&entry, &getNextClusterFat12, &getSectorForCluster);
		}
		
	} else if (files[fd].type == ftFile) {
		files[fd].type = ftFile;
		files[fd].length = entry.size;
	}

	return fd;
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

static void beautifyFileName(char *target, const uint8_t *src)
{
	int namelen = 0;
	for(int i = 0; i < 8; i++) {
		if(src[i] != ' ') {
			namelen = i + 1;
		}
	}
	int cursor = 0;
	for(int i = 0; i < namelen; i++) {
		target[cursor++] = src[i];
	}
	if(src[8] != ' ')
	{ // append extension if any
		target[cursor++] = '.';
		for(int i = 0; i < 3; i++) {
			char c = src[8 + i];
			if(c == ' ') break;
			target[cursor++] = c;
		}
	}
	target[cursor] = 0;
}

extern "C" bool fs_info(int file, struct fsnode * node)
{
	if(node == NULL)
		return false;
	if(file > FHB_MAX)
		return false;
	if(files[file].isOpen == false) {
		return true;
	}
	for(int i = 0; i < FILENAME_MAXLEN; i++) {
		node->name[i] = 0;
	}
	struct directory *entry = &files[file].entry;
	beautifyFileName(node->name, entry->name);
	node->type = files[file].type;
	return true;
}

extern "C" void fs_close(int file)
{
	if(file <= FHB_MAX)
		files[file].isOpen = false; // Am i doing this right, daddy?
}

extern "C" uint32_t file_read(int file, void *buffer,	uint32_t offset, uint32_t length)
{
	if(file > FHB_MAX)
		return 0;
	if(files[file].type != ftFile)
		return 0;
	if(buffer == NULL)
		return 0;
	if(length == 0)	
		return 0;
	
	uint32_t cluster = getCluster(&files[file].entry);
	uint32_t clusterIndex = offset / CLUSTER_SIZE;
	uint32_t clusterOffset = offset % CLUSTER_SIZE;
	
	for(uint32_t i = 0; i < clusterIndex; i++) {
		cluster = getNextClusterFat12(cluster);
		if(cluster >= invalidClusterIndex) {
			return 0; // out of file
		}
	}
	
	uint8_t *target = (uint8_t*)buffer;
	uint32_t cursor = 0;
	while(length > 0)
	{
		uint8_t clusterData[CLUSTER_SIZE];
	
		//*
		Console::main
			<< "Read: "
			<< "cluster=" << cluster << ", "
			<< "idx=" << clusterIndex << ", "
			<< "off=" << clusterOffset << ", "
			<< "len=" << length << ", "
			<< "cur=" << cursor
			<< "\n";
		//*/
		
		// Read cluster here
		read_cluster(
			clusterData,
			getSectorForCluster(cluster));
		
		// Copy data to target
		for(uint32_t i = clusterOffset; i < CLUSTER_SIZE; i++) {
			target[cursor++] = clusterData[i];
			length -= 1;
			if(length == 0) {
				Console::main << "Done Reading with " << cursor << " bytes.\n";
				return cursor;
			}
		}
		
		// Move to next cluster
		clusterOffset = 0;
		cluster = getNextClusterFat12(cluster);
		if(cluster >= invalidClusterIndex) {
			return cursor; // out of file
		}
	}
	Console::main << "Done Reading with " << cursor << " bytes.\n";
	return cursor;
}

extern "C" uint32_t file_size(int file)
{
	if(file > FHB_MAX)
		return 0;
	if(files[file].type != ftFile)
		return 0;
	return files[file].length;
}

extern "C" uint32_t dir_length(int file)
{
	if(file > FHB_MAX)
		return 0;
	if(files[file].type != ftDirectory)
		return 0;
	return files[file].length;
}

extern "C" bool dir_get(int file, int index, struct fsnode * node)
{
	if(node == NULL)
		return false;
	if(file > FHB_MAX)
		return false;
	if(files[file].type != ftDirectory)
		return false;
	
	struct directory entry;
	
	if(files[file].isRootDir) {
		entry = get_directory_entry(
			&files[file].entry,
			index,
			&increment,
			&identity);
	} else {
		entry = get_directory_entry(
			&files[file].entry,
			index,
			&getNextClusterFat12,
			&getSectorForCluster);
	}
	beautifyFileName(node->name, entry.name);
	node->type = typeFromEntry(&entry);
	return true;
}