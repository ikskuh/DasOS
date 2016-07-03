# DasOS Virtual Devices

> darklink:
> HDD: 1GB
> GPU: Kann spriting, tiling und palletierung.
> CPU: Hat ne FPU
> FPU: 32 und 64-bit floats; letztere können nicht im stack
>      gespeichert werden, sondern nur im RAM
> Timer

## Timer
Should send an interrupt in a specified interval

### Registers

| Off  | Ac | Description            | Default |
|------|----|------------------------|---------|
| [+0] | RO | Base Frequency         |         |
| [+1] | RW | Prescaler              | 0       |

#### Prescaler
The prescaler divides the timer frequency and defines the frequency
which triggers the timer interrupt. If the prescaler is zero,
the timer is disabled.

## FPU

Floating Point Unit that has its
own stack and set of instructions.
Each stack entry has 64 bit and contains
an IEEE floating point number.
pushf:
	Push 32bit float to floating stack
popf:
	Pop 32bit float from floating stack
addf, subf, divf, mulf, modf:
	Basic arithmetic operation, pops two, pushes result

## GPU

### Registers

| Off  | Ac | Description                  |
|------|----|------------------------------|
| [+0] | RO | Mode                         |
| [+1] | RO | Width                        |
| [+2] | RO | Height                       |
| [+3] | RO | Color Depth (1,4,8,16,24,32) |
| [+4] | RO | Framebuffer Base Address     |

### Actions

#### swap
Flips the back and front buffer

### Memory Mapped Backbuffer

A linear frame buffer located at [+4] which
contains the pixels of the image.
