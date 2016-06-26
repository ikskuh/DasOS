#pragma once

#include "errors.hpp"
#include "cpustate.hpp"

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
	
  /**
   * Dies with a simple message and error code display.
   */
  static void die(Error code, const char *msg, CpuState *cpu);

};