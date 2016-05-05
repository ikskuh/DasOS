#include "gdt.hpp"

static SegmentDescriptor invalid;
SegmentDescriptor GDT::descriptors[GDT::length];

SegmentDescriptor & GDT::descriptor(uint32_t index)
{
  if(index >= GDT::length) {
    return invalid;
  }
  return GDT::descriptors[index];
}

void GDT::initialize()
{
	// Null descriptor
	GDT::descriptor(0) = SegmentDescriptor();
	GDT::descriptor(1) = SegmentDescriptor(
		0x00000000,
		0xFFFFFFFF, 
		SegmentAccess::Present | SegmentAccess::Executable | SegmentAccess::Segment | SegmentAccess::Ring0, 
		SegmentFlags::Use4KSize | SegmentFlags::Use32Bit);
	GDT::descriptor(2) = SegmentDescriptor(
		0x00000000,
		0xFFFFFFFF, 
		SegmentAccess::Present | SegmentAccess::Writeable | SegmentAccess::Segment | SegmentAccess::Ring0, 
		SegmentFlags::Use4KSize | SegmentFlags::Use32Bit);
	GDT::descriptor(3) = SegmentDescriptor(
		0x00000000,
		0xFFFFFFFF, 
		SegmentAccess::Present | SegmentAccess::Executable | SegmentAccess::Segment | SegmentAccess::Ring3, 
		SegmentFlags::Use4KSize | SegmentFlags::Use32Bit);
	GDT::descriptor(4) = SegmentDescriptor(
		0x00000000,
		0xFFFFFFFF, 
		SegmentAccess::Present | SegmentAccess::Writeable | SegmentAccess::Segment | SegmentAccess::Ring3, 
		SegmentFlags::Use4KSize | SegmentFlags::Use32Bit);
	
	struct
	{
		uint16_t limit;
		void* pointer;
	} __attribute__((packed)) gdtp =
	{
		.limit = GDT::length *sizeof(SegmentDescriptor) - 1,
		.pointer = &GDT::descriptors,
	};
	asm volatile("lgdt %0" : : "m" (gdtp));
	
	asm volatile("mov $0x10, %ax");
	asm volatile("mov %ax, %ds");
	asm volatile("mov %ax, %es");
	asm volatile("mov %ax, %fs");
	asm volatile("mov %ax, %gs");
	asm volatile("mov %ax, %ss");
	asm volatile("ljmp $0x8, $.1");
	asm volatile(".1:");
}