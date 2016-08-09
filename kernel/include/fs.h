#pragma once

#define FILENAME_MAXLEN 13 // 8 + . + 3 + \0

#if defined(__cplusplus)
extern "C"
{
#endif

typedef enum filetype
{
	ftUnknown = 0, 
	ftFile = 1, 
	ftDirectory = 2,
	ftInvalid = 99,
} filetype_t;


typedef struct node
{
	char name[FILENAME_MAXLEN];
	enum filetype type;
} node_t;

/**
 * Opens a file and gets a file handle.
 */
int fs_open(char const * name);

/**
 * Returns the type of the given file.
 */
enum filetype fs_type(int file);

/**
 * Gets information about a file handle.
 * @param file The file handle
 * @param node The target structure where the information will be stored.
 * @return     true on success, false when error
 */
bool fs_info(int file, struct node * node);

/**
 * Releases a file handle.
 */
void fs_close(int file);



/**
 * Reads a block of data from a file.
 * @param file   The file handle.
 * @param buffer The target memory
 * @param offset The start position in the file
 * @param length Number of bytes to read.
 * @return       Number of bytes read.
 */
uint32_t file_read(
	int file,
	void *buffer,
	uint32_t offset,
	uint32_t length);

/**
 * Returns the length of a file in bytes.
 */
uint32_t file_length(int file);


/**
 * Returns the number of entries in a directory.
 * @param file The file handle.
 */
uint32_t dir_length(int file);

/**
 * Gets a directory entry.
 * @param file  The file handle
 * @param index The index of the node in the directory. 
 * @param node  A pointer to a node to store the information.
 * @return      true if the entry is valid, false if not.
 * @remarks     If the directory entry does not exist, the node will have the type ftInvalid.
 */
bool dir_get(int file, int index, struct node * node);

#if defined(__cplusplus)
}
#endif

/**
 * Initializes the file system.
 */
void fs_init();