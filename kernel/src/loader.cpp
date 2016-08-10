#include <stddef.h>
#include <string.h>
#include "elf.hpp"
#include "fs.h"
#include "syscalls.h"
#include "console.hpp"
#include "loader.hpp"

mainfunction_t load_elf(const char *fileName)
{
	int fd = fs_open(fileName);
	if(fd == 0) {
		return NULL;
	}
	if(fs_type(fd) != ftFile) {
		Console::main << "Invalid file type.\n";
		return NULL;
	}
	
	elf::Header header;
	file_read(fd, &header, 0, sizeof(elf::Header));
	
	if(header.magic != elf::MAGIC)
	{
		Console::main << "Invalid ELF magic.\n";
		return NULL;
	}
	Console::main << "ELF Version: " << header.version << "\n";
	
	
	
	uint32_t index = header.ph_offset;
	
	using namespace console_tools;
	for(uint32_t i = 0; i < header.ph_entry_count; i++, index += sizeof(elf::ProgramHeader))
	{
		elf::ProgramHeader programHeader;
		file_read(fd, &programHeader, index, sizeof(elf::ProgramHeader));
	
		Console::main 
			<< "Program Header " << (uint32_t)i << ": " 
			<< hex(programHeader.type) << ", "
			<< hex(programHeader.virt_addr) << "@" << hex(programHeader.offset)
			<< "\n";
		/* Nur Program Header vom Typ LOAD laden */
		if (programHeader.type != 1) {
			if(programHeader.type == 0x10) {
				Console::main << "Load icon here...\n";
				// blitIcon(((char*)&file) + programHeader.offset);
			}
			continue;
		}
		
		if(programHeader.virt_addr < USERSTART) {
			Console::main << "ELF Section below USERSTART, skipping...\n";
			continue;
		}
	
		void* dest = (void*)programHeader.virt_addr;
		
		memset(dest, 0, programHeader.mem_size);
		file_read(fd, dest, programHeader.offset, programHeader.file_size);
	}
	
	return (mainfunction_t)header.entry;
}