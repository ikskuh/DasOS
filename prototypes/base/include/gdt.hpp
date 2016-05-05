#pragma once

#include <inttypes.h>

enum class SegmentAccess : uint8_t
{
  None = 0,
  Accessed = (1 << 0),
  
  Readable = (1<<1),
  Writable = (1<<1),
  
  Direction = (1<<2),
  Conforming = (1<<2),
  
  Executable = (1 << 3),
  Segment = (1 << 4),
  
  Ring0 = 0,
  Ring1 = (1<<5),
  Ring2 = (1<<6),
  Ring3 = (1<<5) | (1<<6),
  
  Present = (1 << 7),
};

static inline SegmentAccess operator | (SegmentAccess lhs, SegmentAccess rhs)
{
  return static_cast<SegmentAccess>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

static inline SegmentAccess operator & (SegmentAccess lhs, SegmentAccess rhs)
{
  return static_cast<SegmentAccess>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

struct SegmentDescriptor
{
  uint16_t limit0;
  uint16_t base0;
  uint8_t base1;
  SegmentAccess access;
  uint8_t limit1 : 4;
  uint8_t flags : 4;
  uint8_t base2;
  
  uint32_t limit()
  {
    return this->limit0 | (this->limit1 << 16);
  }
  
  void setLimit(uint32_t value)
  {
      this->limit0 = (value & 0x0FFFF) >> 0;
      this->limit1 = (value & 0xFFFFF) >> 16;
  }
  
  uint32_t base()
  {
    return this->base0 | (this->base1 << 16) | (this->base2 << 24);
  }
  
  void setBase(uint32_t value)
  {
    this->base0 = (value & 0x0000FFFF) >> 0;
    this->base1 = (value & 0x00FF0000) >> 16;
    this->base2 = (value & 0xFF000000) >> 24;
  }
} __attribute__((packed));

static_assert(sizeof(SegmentDescriptor) == 8, "SegmentDescriptor must be 8 bytes large.");

class GDT
{
public:
  static const uint32_t length = 8;
private:
  static SegmentDescriptor descriptors[length];
  GDT() = delete;
public:
  
  static SegmentDescriptor & descriptor(uint32_t index);
};