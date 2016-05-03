#pragma once

#include "errors.hpp"

/**
 * This class provides the blue screen of death.
 * Discaimer: Color may vary!
 */
class BSOD
{
private:
  BSOD() = delete;
public:

  /**
   * Dies with a simple message and error code display.
   */
  static void die(Error code, const char *msg);

};