# DasOS Virtual Device

DasOS emulates a virtual device that is shared between
all platform implementations. This device provides a
uniform environment with fixed specifications of hardware
and software as well as syscalls.

## Hardware

This section describes the virtual hardware components.

### CPU
The virtual device uses an *SPU Mark I* as the main processor.

### RAM
The *CPU* has a 32 MB *RAM* connected. In this RAM, both
data and code resides and the RAM has no memory protection.

### Screen
The screen connected to the *vGPU* has a resolution of
640x480 pixels and a color depth of 24 bits.

### vGPU
The *vGPU* provides access to the *Screen* and allows the
manipulation of its contents.

The gpu uses a double buffering system with a front and a
back buffer where as the front buffer is displayed on the
screen and the back buffer is modified by the CPU.

The gpu provides functions for printing text to the screen
as well as drawing visual primitives and clearing or swapping
the back buffer.

In addition to the features mentioned above, the back buffer is
accessible by a memory mapped area in the *RAM*.

### Persistent Storage
Two persistent storage devices are connected to the virtual device.
Both storage devices behave like IDE block devices where as the first
device is a fixed device and the second device is hot-pluggable.

The size and file format of the storage device is not specified.

## Software
This section describes the basic software components and interfaces
of the virtual device.

### BIOS
Each device implementation provides a *BIOS* that initialized
the device and provides host-dependant configuration.

### Boot Sequence
The boot sequence is initiated by the *BIOS* of the device
and can only be changed with a BIOS configuration.

- Determine existence of persistant storage devices
  - Fixed device
  - Swappable device
- Bootloading
  - Searching for a fitting partition
    - FAT32 formatted partition
    - `/system.exp` must exist
  - *Exploading* `/system.exp` into RAM
- Clearing *vGPU* text screen.
- Jump to *Entry Point*

### Memory Areas

#### Memory Mapped IO
The memory between `0x200000‬` and `0x‭400000‬` is reserved for
hardware use.

#### Entry Point
After successfully initialization, the virtual device jumps
to the address `0x‭400000‬` which is equal to the instruction
index `0x‭100000‬`.

### vGPU Interface

#### Back Buffer
The back buffer of the vGPU is organized in n RGB24 format
where pixels are stored in a 24 bpp format with R as the first
byte, G as the second and B as the last.
It is 900 kB large and maps the full screen area of 640x480 pixels
as a linear frame buffer from `0x200000‬` to `0x2E1000`.

#### MMIO Registers
The *vGPU* also uses some registers accessible via memory mapped io.

| Register | Address  | Type | Description                             |
|----------|----------|------|-----------------------------------------|
| COLOR    | 0x2E1000 | u32  | The color used for any draw commands.   |
| CURSORX  | 0x2E1004 | u16  | The x position of the draw cursor       |
| CURSORY  | 0x2E1006 | u16  | The y position of the draw cursor       |
| SPRITE   | 0x2E1008 | ptr  | The address of the sprite to copy       |
| WIDTH    | 0x2E100C | u16  | The width of the sprite.                |
| HEIGHT   | 0x2E101E | u16  | The height of the sprite.               |
| PITCH    | 0x2E1010 | u32  | The pitch (memory width) of the sprite. |

#### HWIO Interface
The vGPU is controllable via the HWIO command of the SPU
with the command info >= `0x10000`.

| Command | CmdInfo | Input0 | Input1 | Description                                               |
|---------|---------|--------|--------|-----------------------------------------------------------|
| Clear   | 0x10000 | y      | x      | Clears the screen and moves the cursor to (x,y)           |
| Line    | 0x10001 | y      | x      | Draws a line from the cursor to (x,y).                    |
| Rect    | 0x10002 | y      | x      | Draws a rectangle between the cursor and (x,y).           |
| Fill    | 0x10003 | y      | x      | Fills a rectangle between the cursor and (x,y).           |
| Copy    | 0x10004 | y      | x      | Renders the current sprite at (x,y) and moves the cursor. |
| Pixel   | 0x10005 | y      | x      | Sets the pixel at (x,y).                                  |
| Move    | 0x10006 | y      | x      | Moves the cursor to (x,y).                                |

# TODO: Text Commands