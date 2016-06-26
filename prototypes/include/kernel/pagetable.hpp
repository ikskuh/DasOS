#pragma once

class PageTable
{
private:
	uint32_t pages[1024];
public:
	PageTable();
	~PageTable();
	
	uint32_t & pageDescriptor(uint32_t pageIndex)
	{
		return this->pages[pageIndex];
	}
};

static_assert(sizeof(PageTable) == 4096, "PageDirectory must be 4096 bytes large");