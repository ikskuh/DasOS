# SuperVM System Documentation

This document contains descriptions of the SuperVM host system which
utilized a SPU Mark I to execute code.


## Executable Program Format EXP

SuperVM uses a simple format for storing its executables.

The format is an archive which stores key-value-pairs for configuration
and sections containing code or data.

### Sections
A section can be either code or data and has a defined memory position and length.


## System Memory
SuperVM provides access to up to 4 GBs of RAM with the x86 paging concept:

The memory is organized in page tables and a page directories. The directory
contains pointers to a page table which then contains the pointers to 4096 byte
large pages.

