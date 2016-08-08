#pragma once

#if defined(__cplusplus)
extern "C"
{
#endif

typedef enum fileflags {
	ffReadOnly  = 0x01,
	ffHidden    = 0x02,
	ffSystem    = 0x04,
	ffVolumeID  = 0x08,
	ffDirectory = 0x10,
	ffArchive   = 0x20,
	ffLongName  = ffReadOnly | ffHidden | ffSystem | ffVolumeID,
} fileflags_t;

typedef struct fatheader {
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

typedef struct fatheader16 {
	uint8_t biosDriveNum;
	uint8_t reserved;
	uint8_t extendedBootSignature; // should be 0x29
	uint32_t filesystemID;
	uint8_t filesystemName[11];
	uint8_t fatName[8];
} __attribute__ ((packed)) fat16header_t;

typedef struct directory {
	uint8_t name[11];
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

#if defined(__cplusplus)
}
#endif