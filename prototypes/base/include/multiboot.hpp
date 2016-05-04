#pragma once

#include <inttypes.h>
#include "pointer.hpp"

#define MB_MEMSIZE (1<<0)
#define MB_BOOTDEVICE (1<<1)
#define MB_COMMANDLINE (1<<2)
#define MB_MODULES (1<<3)
#define MB_SYMS_AOUT (1<<4)
#define MB_SYMS_ELF (1<<5)
#define MB_MEMORYMAP (1<<6)
#define MB_DRIVES (1<<7)
#define MB_CONFIG_TABLE (1<<8)
#define MB_BOOTLOADER_NAME (1<<9)
#define MB_APS_TABLE (1<<10)
#define MB_VBE (1<<11)

#define MB_ASSERT_SIZE(type, len) static_assert(sizeof(type) == len, "multiboot::" #type " must be " #len " bytes large.")

namespace multiboot
{
  template<typename T>
  class mbarray
  {
  public:
    const uint32_t length;
  private:
    T *data;
    
    mbarray() = delete;
  public:
    T const & operator [](size_t idx) {
      return this->data[idx];
    }
  } __attribute__((packed));
  
  // Make sure the size is not dependend on the template parameter
  MB_ASSERT_SIZE(mbarray<uint8_t>, 8);
  MB_ASSERT_SIZE(mbarray<uint32_t>, 8);

  struct MemoryMap
  {
      uint32_t entry_size;
      uint64_t base;
      uint64_t length;
      uint32_t type;
  } __attribute__((packed));

  MB_ASSERT_SIZE(MemoryMap, 24);

  struct Module
  {
      physical_t start;
      physical_t end;
      const char * name;
      uint32_t reserved;
  } __attribute__((packed));

  MB_ASSERT_SIZE(Module, 16);

  struct Drive
  {
      uint32_t size;
      uint8_t number;
      uint8_t mode;
      uint16_t cylinders;
      uint8_t heads;
      uint8_t sectors;
      
      // 0x10	size-0x10	drive_ports	I/O-Ports, die von diesem Gerät benutzt werden
      // uint16_t ports[0];
      
      /**
       * Gets the number of ports used by this drive.
       */
      uint32_t portCount() const {
        return (this->size - 0x10) / sizeof(uint16_t);
      }
      
      /** 
       * Gets the given port.
       * @return The port #idx or 0 if out of range.
       */
      uint16_t port(size_t idx) const {
        uint16_t const * ports = reinterpret_cast<uint16_t const *>(reinterpret_cast<uint8_t const *>(this) + 0x10);
        if(idx >= this->portCount()) {
          return 0;
        }
        return ports[idx];
      }
  } __attribute__((packed));

  static_assert(sizeof(Drive) >= 10, "multiboot::Drive must be at least 12 bytes large.");

  struct APMTable
  {
      uint16_t version;
      uint16_t cseg;
      uint32_t offset;
      uint16_t cseg_16;
      uint16_t dseg;
      uint16_t flags;
      uint16_t cseg_len;
      uint16_t cseg_16_len;
      uint16_t dseg_len;
  } __attribute__((packed));

  MB_ASSERT_SIZE(APMTable, 20);
  
  struct Structure
  {
      uint32_t flags;
      physical_t memLower;
      physical_t memUpper;
      uint32_t bootDevice;
      const char * commandline;
      mbarray<Module> modules;
      union {
          struct {
              uint32_t tabsize;
              uint32_t strsize;
              uint32_t addr;
              uint32_t reserved;
          } __attribute__((packed)) symsAssemblerOut;
          struct {
              uint32_t num;
              uint32_t size;
              uintptr_t addr;
              uintptr_t shndx;
          } __attribute__((packed)) symsELF;
      };
      mbarray<MemoryMap> memoryMaps;
      mbarray<Drive> drives;
      physical_t configTable;
      const char * bootLoaderName;
      const APMTable * apmTable;
      struct {
        uint32_t ontrolInfo;
        uint32_t modeInfo;
        uint16_t mode;
        uint16_t interfaceSegment;
        uint16_t interfaceOffset;
        uint16_t interfaceLength;
      } __attribute__((packed)) vbe;
  } __attribute__((packed));
  
  MB_ASSERT_SIZE(Structure, 88);

}