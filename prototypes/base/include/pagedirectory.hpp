#pragma once

#include "pagetable.hpp"

class PageDirectory
{
private:
	uint32_t tables[1024];
public:
	PageDirectory();
	~PageDirectory();
	
	uint32_t & table(uint32_t index) {
		return this->tables[index];
	}
};

static_assert(sizeof(PageDirectory) == 4096, "PageDirectory must be 4096 bytes large");