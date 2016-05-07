#pragma once

#include "pmm.hpp"
#include "enums.hpp"
#include "pagedirectory.hpp"

enum class VMMFlags
{
	None = 0,
	Present = (1<<0),
	Writable = (1<<1),
	UserSpace = (1<<2),
	SystemAllocated = (1<<3),
};

ENUM_CLASS_OPERATORS(VMMFlags)

class VMMContext
{
	friend class VMM;
private:
	PageDirectory *directory;
	
	/**
	 * Gets the uint32_t that describes the given virtual address
	 */
	uint32_t & getPageDescriptor(virtual_t virt);
public:
	VMMContext();
	~VMMContext();
	
	/**
	 * Maps an arbitrary page into the virtual memory.
	 */
	void provide(virtual_t virt, VMMFlags flags);
	
	/**
	 * Maps a given page into the virtual memory.
	 */
	void map(virtual_t virt, physical_t phys, VMMFlags flags);
	
	/**
	 * Unmaps a given page from the virtual memory.
	 */
	void unmap(virtual_t virt);
	
};

class VMM
{
private:
	VMM() = delete;
public:
	/**
	 * Enables paging
	 */
	static void enable();
	
	/**
	 * Sets the given context as the current context.
	 */
	static void activate(VMMContext & context);
};
