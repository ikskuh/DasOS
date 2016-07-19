#include <new>
#include "vmm.hpp"
#include "console.hpp"
#include "asm.hpp"

void VMM::enable()
{
	uint32_t val;
	ASM_READ_REG(cr0, val);
	val |= (1 << 31);
	ASM_WRITE_REG(cr0, val);
}

void VMM::activate(VMMContext & context)
{
	ASM_WRITE_REG(cr3, context.directory);
}

VMMContext::VMMContext() : 
	directory(PMM::alloc().data<PageDirectory>())
{
	new (this->directory) PageDirectory(); 
}

VMMContext::~VMMContext()
{
	this->directory->~PageDirectory();
}


uint32_t & VMMContext::getPageDescriptor(virtual_t v)
{
	uint32_t virt = v.numeric();
	uint32_t directoryIndex = (virt >> 12) / 1024;
	uint32_t tableIndex     = (virt >> 12) % 1024;

	uint32_t & tableDesc = this->directory->table(directoryIndex);
	if((tableDesc & 0x01) == 0)
	{
		auto pageMemory = PMM::alloc();
		tableDesc = 
			pageMemory.numeric() |
			static_cast<uint32_t>(
				VMMFlags::Present | 
				VMMFlags::Writable | 
				VMMFlags::UserSpace |
				VMMFlags::SystemAllocated);
		PageTable *table = reinterpret_cast<PageTable*>(tableDesc & 0xFFFFF000);
		new (table) PageTable();
	}
	PageTable *table = reinterpret_cast<PageTable*>(tableDesc & 0xFFFFF000);
	auto & entry = table->pageDescriptor(tableIndex);
	/*
	Console::main <<
		v << " -> " << directoryIndex << "/" << tableIndex << ": " << tableDesc << "/" << entry << "\n";
	//*/
	return entry;
}

/**
 * Maps an arbitrary page into the virtual memory.
 */
void VMMContext::provide(virtual_t virt, VMMFlags flags)
{
	this->map(virt, PMM::alloc(), flags | VMMFlags::SystemAllocated);
}

/**
 * Maps a given page into the virtual memory.
 */
void VMMContext::map(virtual_t virt, physical_t phys, VMMFlags flags)
{
	using namespace console_tools;
	
	flags |= VMMFlags::Present;
	
	uint32_t & pageDesc = this->getPageDescriptor(virt);
	pageDesc = 
		phys.numeric() |
		static_cast<uint32_t>(flags);
	
	/*
	Console::main <<
		"Mapping " << virt << " -> " << phys << " [" << bin(static_cast<int>(flags)) << "]: " << hex(pageDesc) << "\n";
	//*/
	ASM::invlpg(virt.data());
}

/**
 * Unmaps a given page from the virtual memory.
 */
void VMMContext::unmap(virtual_t virt)
{
	
}



PageDirectory::PageDirectory()
{
	for(uint32_t i = 0; i < 1024; i++) {
		this->tables[i] = 0;
	}
}

PageDirectory::~PageDirectory()
{
	for(uint32_t i = 0; i < 1024; i++) {
		if(this->tables[i] & 0x01) {
				PMM::free(physical_t(this->tables[i] & 0xFFFFF000));
		}
	}
}

PageTable::PageTable()
{
	for(uint32_t i = 0; i < 1024; i++) {
		this->pages[i] = 0;
	}
}

PageTable::~PageTable()
{
	for(uint32_t i = 0; i < 1024; i++) {
		if(this->pages[i] & static_cast<uint32_t>(VMMFlags::SystemAllocated)) {
				PMM::free(physical_t(this->pages[i] & 0xFFFFF000));
		}
	}
}