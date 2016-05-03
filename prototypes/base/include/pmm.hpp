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
   * Marks a page as occupied by external memory management.
   * @returns true if the page was previously free, false if it was already allocated.
   */
  static bool markOccupied(physical_t page);

  /**
   * Allocates a single page.
   * @param success This boolean will contain true if the allocation was successful.
   */
  static physical_t alloc(bool &success);

  /**
   * Frees a given page by pointer.
   */
  static void free(physical_t page);
};