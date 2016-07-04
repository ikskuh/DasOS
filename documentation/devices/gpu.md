# GPU
The GPU provides a basic graphics interface to the outer world via
a shared back buffer.

The gpu flips the back buffer in a GPU-defined time interval.

## Registers

| # | Ac | Description                  |
|---|----|------------------------------|
| 0 | RO | Mode                         |
| 1 | RO | Width                        |
| 2 | RO | Height                       |
| 3 | RO | Color Depth (1,4,8,16,24,32) |
| 4 | RO | Framebuffer Base Address     |

### Mode
This register contains the id of the current video mode.
The video mode id is a predefined combination of width,
height and color depth.

### Width
This register contains the screen width in pixels.

### Height
This register contains the screen height in pixels.

### Color Depth
The color depth defines how much bits a single pixel takes and in
which format the pixels are stored.

| Depth | Color Format |
|-------|--------------|
|     1 |  Black/White |
|     4 |   16 indexed |
|     8 |  256 indexed |
|    16 |       R5G6B5 |
|    24 |       R8G8B8 |
|    32 |     R8G8B8X8 |

### Framebuffer Base Address
This register holds the address of the linear frame buffer.

## Memory Mapped Framebuffer

A linear frame buffer is located at the address defines by register `#4`.
This framebuffer contains the pixels in row-major order.

## Interrupts

| # | Description      |
|---|------------------|
| 0 | Vertical Retrace |

### Vertical Retrace
This interrupt is triggered when the GPU does a vertical retrace after
finishing the current image.