#include "gdt.hpp"

SegmentDescriptor GDT::descriptors[GDT::length];

SegmentDescriptor & GDT::descriptor(uint32_t index)
{
  static SegmentDescriptor invalid;
  if(index >= GDT::length) {
    return invalid;
  }
  return GDT::descriptors[index];
}