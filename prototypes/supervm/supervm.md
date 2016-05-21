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
|       CP | Code Pointer  | Stores the instruction which is executed next.  |
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

| Component   | Range             | Size | Function                                 |
|-------------|-------------------|------|------------------------------------------|
| execution Z | See below.        |    2 | Excution dependend on Zero?              |
| execution N | See below.        |    2 | Excution dependend on Negative?          |
| input0      | Zero/Pop/Peek/Arg |    2 | Where does input0 come from?             |
| input1      | Zero/Pop          |    1 | Where does input1 come from?             |
| command     | [6bit]            |    6 | Which command is executed?               |
| cmdinfo     | [16bit]           |   16 | Parameter value for the command.         |
| flagmod     | yes/no            |    1 | Does this command modifies flags?        |
| output      | Discard/Push/Jump |    2 | What is done with the output?            |
| argument    | [32bit]           |   32 | Some commands can take extra information |

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
|  8 | MATH        | output = input0 OP[info] input1      |
|  9 | SPGET       | output = SP + input0                 |
| 10 | SPSET       | output = SP + input0 = input1        |
| 11 | SYSCALL     | output = SysCall(input0, input1)     |
| 12 | HWIO        | output = HardwareIO(input0, input1)  |
| 13 |             |                                      |
| 14 |             |                                      |
| 15 |             |                                      |

### Math
The math operator is a compound operator that contains all 
arithmetic operations. The arithmetic operation is selected
by the `cmdinfo`.

| cmdinfo | Operation                   |
|---------|-----------------------------|
|       0 | Addition                    |
|       1 | Subtraction                 |
|       2 | Multiplication              |
|       3 | Division                    |
|       4 | Euclidean Division / Modulo |
|       5 | Bitwise Logic And           |
|       6 | Bitwise Logic Or            |
|       7 | Bitwise Logic Xor           |
|       8 | Bitwise Logic Not           |
|       9 | Rotating Bit Shift Left     |
|      10 | Rotating Bit Shift Right    |
|      11 | Arithmetic Bit Shift Left   |
|      12 | Arithmetic Bit Shift Right  |
|      13 | Logic Bit Shift Left        |
|      14 | Logic Bit Shift Right       |

## Assembler Mnemonics

| Mnemonic | Arg? | i0   | i1   | Cmd     | CmdInfo  | Output  | Flags? |
|----------|------|------|------|---------|----------|---------|--------|
| nop      |   no | zero | zero | copy    | 0        | discard | no     |
| push     |  yes | arg  | zero | copy    | 0        | push    | no     |
| drop     |   no | pop  | zero | copy    | 0        | discard | no     |
| dup      |   no | peek | zero | copy    | 0        | push    | no     |
| jmp      |  yes | arg  | zero | copy    | 0        | jump    | no     |
| jmpi     |   no | pop  | zero | copy    | 0        | jump    | no     |
| ret      |   no | pop  | zero | copy    | 0        | jump    | no     |
| load     |  yes | arg  | zero | load    | 0        | push    | no     |
| loadi    |   no | pop  | zero | load    | 0        | push    | no     |
| store    |  yes | arg  | pop  | store   | 0        | discard | no     |
| storei   |   no | pop  | pop  | store   | 0        | discard | no     |
| get      |  yes | arg  | zero | get     | 0        | push    | no     |
| geti     |   no | pop  | zero | get     | 0        | push    | no     |
| set      |  yes | arg  | pop  | set     | 0        | discard | no     |
| seti     |   no | pop  | pop  | set     | 0        | discard | no     |
| bpget    |   no | zero | zero | bpget   | 0        | push    | no     |
| bpset    |   no | pop  | zero | bpset   | 0        | discard | no     |
| add      |   no | pop  | pop  | math    | 0        | push    | no     |
| sub      |   no | pop  | pop  | math    | 1        | push    | no     |
| cmp      |   no | pop  | pop  | math    | 1        | discard | yes    |
| mul      |   no | pop  | pop  | math    | 2        | push    | no     |
| div      |   no | pop  | pop  | math    | 3        | push    | no     |
| mod      |   no | pop  | pop  | math    | 4        | push    | no     |
| and      |   no | pop  | pop  | math    | 5        | push    | no     |
| or       |   no | pop  | pop  | math    | 6        | push    | no     |
| xor      |   no | pop  | pop  | math    | 7        | push    | no     |
| not      |   no | pop  | zero | math    | 8        | push    | no     |
| rol      |   no | pop  | pop  | math    | 9        | push    | no     |
| ror      |   no | pop  | pop  | math    | 10       | push    | no     |
| asl      |   no | pop  | pop  | math    | 11       | push    | no     |
| asr      |   no | pop  | pop  | math    | 12       | push    | no     |
| shl      |   no | pop  | pop  | math    | 13       | push    | no     |
| shr      |   no | pop  | pop  | math    | 14       | push    | no     |
| syscall  |  yes | zero | zero | syscall | 0        | discard | no     |
| hwio     |  yes | zero | zero | hwio    | 0        | discard | no     |