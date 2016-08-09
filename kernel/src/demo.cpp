#include "console.hpp"
#include "ata.hpp"
#include "fat.h"
#include "fs.h"

void dasos_demo()
{
	fs_init();

	int fd = fs_open("C:/textfile.txt");

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
		uint32_t size = file_size(fd);
		Console::main << "Dumping " << size << " bytes of data:\n";
		
		static uint8_t memory[2185];
		if(file_read(fd, memory, 0, 2185) == false) {
			Console::main << "Reading failed :(\n";
			return;
		}
		Console::main.write((char*)memory, 2185);
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
