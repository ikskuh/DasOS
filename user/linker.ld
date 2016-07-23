ENTRY(__usrinit)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

PHDRS
{
	text PT_LOAD;
	data PT_LOAD;
	icon 0x10;
}

SECTIONS
{
	icon : {
		*(icon)
	} : icon
	
	. = 0x40000000;

	.text : {
			*(multiboot)
			*(.text)
	} : text
	.data ALIGN(4096) : {
			start_ctors = .;
			KEEP(*( .ctors ));
			KEEP(*(SORT_BY_INIT_PRIORITY( .ctors.* )));
			end_ctors = .;

			start_dtors = .;
			KEEP(*( .fini_array ));
			end_dtors = .;

			*(.data)
	} : data
	.rodata ALIGN(4096) : {
			*(.rodata)
	} : data
	.bss ALIGN(4096) : {
			*(.bss)
	} : data
}