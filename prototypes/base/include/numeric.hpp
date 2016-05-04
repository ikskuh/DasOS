#pragma once

#include <inttypes.h>
#include <stddef.h>

class Numeric
{
private:
  Numeric() = delete;
public:

  /**
   * Converts an unsigned number to a string.
   * @param buffer The target buffer where the string should be stored.
   * @param length The length of the buffer.
   * @param number The number that should be converted.
   * @param radix  The numeric base for the printed number.
   * @return The length of the converted strings.
   */
  static size_t toString(
    char *buffer, 
    size_t length,
    uint32_t number, 
    uint32_t radix = 10);
  static size_t toString(
    char *buffer, 
    size_t length,
    uint64_t number, 
    uint32_t radix = 10);
    
  /**
   * Converts a signed number to a string.
   * @param buffer The target buffer where the string should be stored.
   * @param length The length of the buffer.
   * @param number The number that should be converted.
   * @param radix  The numeric base for the printed number.
   * @return The length of the converted strings.
   */
  static size_t toString(
    char *buffer, 
    size_t length,
    int32_t number, 
    uint32_t radix = 10);
  static size_t toString(
    char *buffer, 
    size_t length,
    int64_t number, 
    uint32_t radix = 10);
};