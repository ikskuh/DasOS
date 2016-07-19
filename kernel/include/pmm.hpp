#pragma once

#include "pointer.hpp"

/**
 * Physical memory management tool.
 */
class PMM
{
private:
  PMM() = delete;
public:
  /**
   * Marks a page as free by external memory management.
   */
  static void markFree(physical_t page);
  
  /**
   * Marks a page as used by external memory management.
   */
  static void markUsed(physical_t page);

  /**
   * Allocates a single page.
   * @remarks This method will either return a valid value or die with a BSOD if out of memory.
   */
  static physical_t alloc();

  /**
   * Frees a given page by pointer.
   */
  static void free(physical_t page);
  
  /**
   * Returns the free memory.
   */
  static uint32_t getFreeMemory();
};