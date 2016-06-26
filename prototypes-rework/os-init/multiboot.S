.section multiboot
 
#define MB_MAGIC 0x1badb002
#define MB_FLAGS 0x00
#define MB_CHECKSUM -(MB_MAGIC + MB_FLAGS)
 
.align 4

// Offset	Type	Field Name	   Note 
//  0     u32   magic          required 
//  4     u32   flags          required 
//  8     u32   checksum       required 
.int    MB_MAGIC
.int    MB_FLAGS
.int    MB_CHECKSUM

// 12     u32   header_addr    if flags[16] is set 
// 16     u32   load_addr	     if flags[16] is set 
// 20     u32   load_end_addr	 if flags[16] is set 
// 24     u32   bss_end_addr	 if flags[16] is set 
// 28     u32   entry_addr	   if flags[16] is set 
.int 0, 0, 0, 0, 0

// 32     u32   mode_type	     if flags[2] is set 
// 36     u32   width          if flags[2] is set 
// 40     u32   height	       if flags[2] is set 
// 44     u32   depth	         if flags[2] is set 
.int 0, 0, 0, 0