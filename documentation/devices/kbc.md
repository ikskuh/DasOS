# Keyboard Controller

The keyboard controller interfaces with a keyboard
and provides a simple interface to it.

## Registers

| # | Ac | Description            | Default |
|---|----|------------------------|---------|
| 0 | RO | Flags                  |         |
| 1 | RW | Data                   |         |

### Flags
A bitfield displaying the status of the KBC.

| Bit | Description        |
|-----|--------------------|
|   0 | Keyboard Available |
|   1 | Key Data Available |

#### Keyboard Available
This bit is set when a keyboard is plugged to the keyboard
controller.

#### Key Data Available
This bit is set when a key press/release is stored in the
buffer of the keyboard controller.

### Data
Bi-directional interface to the keyboard.
This register does not store values but provides
scan codes on reading it and setting the keyboard
state on writing.

#### Reading
As long as the keyboard controller has keypress data
available, reading from this register yields the
scancode of the pressed/released button. If no keypress
data is available, reading yields a value where all bits
are set.

#### Writing
Writing to this register sets the state of the keyboard LEDs.

| Bit | LED         |
|-----|-------------|
|   0 | NUM-Lock    |
|   1 | CAPS-Lock   |
|   2 | Scroll-Lock |

## Interrupts

| # | Description     |
|---|-----------------|
| 0 | Keyboard Action |

### Keyboard Action
This interrupt triggers when a key was pressed or released.