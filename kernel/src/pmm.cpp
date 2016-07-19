#include <stdint.h>
#include <stddef.h>
#include "pmm.hpp"
#include "bsod.hpp"

/**
 * Number stored of pages in the bitmap
 */
static const uint32_t BitmapSize = 1048576; /* 4 GB */

/**
 * Number of 32-bit tuples in the bitmap.
 */
static const uint32_t BitmapLength = BitmapSize / 32; 

/**
 * Bitmap that stores the page status.
 * A set bit marks a page as free.
 * An unset bit marks a page as used.
 */
static uint32_t bitmap[BitmapLength];

/**
 * Checks if an interger is 4096-aligned
 */
static bool isAligned(uint32_t ptr)
{
  return (ptr % 4096) == 0;
}

void PMM::markFree(physical_t page)
{
  uint32_t ptr = page.numeric();
  if(!isAligned(ptr))
    ; // Do something about it!
  uint32_t pageId = ptr / 4096;
  
  uint32_t idx = pageId / 32;
  uint32_t bit = pageId % 32;
  
  bitmap[idx] |= (1<<bit);
}

void PMM::markUsed(physical_t page)
{
  uint32_t ptr = page.numeric();
  if(!isAligned(ptr))
    ; // Do something about it!
  uint32_t pageId = ptr / 4096;
  
  uint32_t idx = pageId / 32;
  uint32_t bit = pageId % 32;
  
  bitmap[idx] &= ~(1<<bit);
}

physical_t PMM::alloc()
{
  for(uint32_t idx = 0; idx < BitmapLength; idx++) {
    // fast skip when no bit is set
    if(bitmap[idx] == 0) {
      continue;
    }
    for(uint32_t bit = 0; bit < 32; bit++) {
      uint32_t mask = (1<<bit);
      if((bitmap[idx] & mask) == 0) {
        // If bit is not set, ignore the bit.
        continue;
      }
      
      // Mark the selected bit as used.
      bitmap[idx] &= ~mask;
      
      uint32_t pageId = 32 * idx + bit;
      uint32_t ptr = 4096 * pageId;
      
      return physical_t(ptr);
    }
  }
  BSOD::die(Error::OutOfMemory, "Out of physical memory!");
	return physical_t::invalid;
}

void PMM::free(physical_t page)
{
  uint32_t ptr = page.numeric();
  if(!isAligned(ptr))
    ; // Do something about it!
  uint32_t pageId = ptr / 4096;
  
  uint32_t idx = pageId / 32;
  uint32_t bit = pageId % 32;
  
  // Mark the selected bit as free.
  bitmap[idx] |= (1<<bit);
}

uint32_t PMM::getFreeMemory()
{
  uint32_t freeMemory = 0;
  for(uint32_t idx = 0; idx < BitmapLength; idx++) {
    // fast skip when no bit is set
    if(bitmap[idx] == 0) {
      continue;
    }
    for(uint32_t bit = 0; bit < 32; bit++) {
      uint32_t mask = (1<<bit);
      if((bitmap[idx] & mask) == 0) {
        // If bit is not set, ignore the bit.
        continue;
      }
      freeMemory += 0x1000; 
    }
  }
  return freeMemory;
}