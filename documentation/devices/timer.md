# Timer

The timer is a configurable timer with flexible frequency generation.
It sends an interrupt in a specified interval.

## Registers

| # | Ac | Description            | Default |
|---|----|------------------------|---------|
| 0 | RO | Base Frequency         |         |
| 1 | RW | Counter                |    0x00 |
| 2 | RW | Limit                  |    0x00 |

### Base Frequency 
The frequency with which the timer ticks.
Each tick `Counter` is incremented by 1.

### Counter
This register is incremented every time the timer ticks.

### Limit
If after a tick `Counter` equals `Limit`, the timer will
reset `Counter` to zero and fire the `Overflow` interrupt.

If `Limit` is set to zero, the timer is disabled.

## Interrupts

| # | Description |
|---|-------------|
| 0 | Overflow    |

### Overflow
This interrupt is triggered each time the `Counter` register
reaches the value stored in `Limit`.