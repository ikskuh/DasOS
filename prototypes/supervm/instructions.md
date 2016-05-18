# SuperVM

SuperVM is a stack machine with a simple, but flexible command
set.

## The Stack
The virtual machine utilizes a stack to provide operands to instructions.
This stack stores temporary values the program is working with.

## Registers and Flags

The SuperVM virtual machine is a stack machine, but has also some control
registers that can be set with special instructions. The registers mainly
control stack access or control flow.

| Mnemonic | Register      | Function                                        |
|----------|---------------|-------------------------------------------------|
|       SP | Stack Pointer | Stores the current 'top' position of the stack. |
|       BP | Base Pointer  | Stores the current stack frame position.        |
|       FG | Flag Register | Stores the state of the flags.                  |

### Stack Pointer
The stack pointer points to the top of the stack. Each `push` operation increases
the stack pointer by one, each `pop` operation reduces it by one.

### Base Pointer
The base pointer is a pointer that can be set to access the stack relative to it.
The indended use is to create a stack frame with the base pointer by pushing the
previous base pointer to the stack and setting the new base pointer to the current
stack pointer.

Returning a function with this mechanism is moving the stack pointer to the current
base pointer, then popping the previous base pointer from the stack.

### Flag Register

| Bit | Flag     | Option                          |
|-----|----------|---------------------------------|
|   0 | Zero     | Is set when the output is zero. |
|   1 | Negative | Is set when the MSB is set.     |

## Instructions

An SuperVM instruction is composed of multiple components:

| Component | Range             | Size | Function                                 |
|-----------|-------------------|------|-------------------------------------------|
| execution | See below.        |    6 | When is the instruction executed.        |
| input0    | Zero/Pop/Peek/Arg |    2 | Where does input0 come from?             |
| input1    | Zero/Pop          |    1 | Where does input1 come from?             |
| command   | [8bit]            |    8 | Which command is executed?               |
| cmdinfo   | [12bit]           |   12 | Parameter value for the command.         |
| flagmod   | yes/no            |    1 | Does this command modifies flags?        |
| output    | Discard/Push/Jump |    2 | What is done with the output?            |
| argument  | [32bit]           |   32 | Some commands can take extra information |

### Execution Modes

The execution mode checks whether the instruction will be execution or not. The execution
depends on the state of the flags. An `X` means "Don't care", a `0` means the flag must be
cleared and a `1` means the flag must be set.

An instruction is only executed when all conditions are met.

| Flag      | Range |
|-----------|-------|
| Zero      | X/0/1 |
| Negative  | X/0/1 |

### Commands

| ID | Command     | Action                               |
|----|-------------|--------------------------------------|
|  0 | COPY        | output = input0                      |
|  1 | STORE       | output = MEMORY[input0] = input1     |
|  2 | LOAD        | output = MEMORY[input0]              |
|  3 | GET         | output = STACK[BP + input0]          |
|  4 | SET         | output = STACK[BP + input0] = input1 |
|  5 | BPGET       | output = BP                          |
|  6 | BPSET       | output = BP = input0                 |
|  7 | RSTSTACK    | output = SP = BP                     |
|  8 |             |                                      |
|  9 |             |                                      |
| 10 |             |                                      |
| 11 |             |                                      |
| 12 |             |                                      |
| 13 |             |                                      |
| 14 |             |                                      |
| 15 |             |                                      |


## Assembler Mnemonics