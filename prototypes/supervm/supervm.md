# SuperVM

SuperVM is a stack machine with a simple, but flexible command
set.

## Purpose of this document

This document is meant to give a complete overview over the concepts and abstract
workings of SuperVM.

It is targeted at uses who program SuperVM with the native assembly language,
system programmers who want to include the virtual machine in their system or
create their own SuperVM implementation.

## Concepts

SuperVM is a virtual machine that emulates a 32 bit stack machine. Instead of utilizing
registers operations take their operands from the stack and push their results to
it.

An instruction is split into two parts:
The instruction configuration and the command. The command defines what operation should
be performed (memory access, calculation, ...), whereas the configuration defines the
behaviour of instruction (stack/flag-modifications).

## Memory Areas
The virtual machine has three separarated memory areas. Each area serves a specific
purpose and should not overlap the others.

### Code Memory
The code memory contains an immutable block of code that is instruction indexable.
Each instruction is 64 bit wide.

### Stack Memory
The virtual machine utilizes a stack to provide operands to instructions.
This stack stores temporary values the program is working with.

Each entry on the stack is an 32 bit value that is mostly interpreted as
a pointer, an index or an unsigned or signed integer. It is also possible
to store a 32bit IEEE floating point number on the stack.

The size of the stack is defined by the implementation, but it should contain at
least 1024 entries. This allows a fair recursive depth of 128 recursions with an average
of 6 local variables per function call.

### Data Memory
SuperVM also provides a memory model that allows storing persistent data that is
accessed by different parts of the code.
The data memory is byte accessible and can be written or read.

It is implementation defined how the memory is managed and accessible. It can be a
sparse memory with different sections, it could utilize a software-implemented paging
process or just be a flat chunk of memory.

As most programs require a minimum of global variables, the data memory should be
at least 16kB large.

Every pointer that accesses data memory (e.g. via `store` and `load`) contains the 
address of a byte in memory, starting with zero.

## Registers and Flags

The SuperVM virtual machine is a stack machine, but has also some control
registers that can be set with special instructions. The registers mainly
control stack access or control flow.

Each register has a size of 32 bits. Only exception is the flag register which
contains a single bit per flag.

| Mnemonic | Register      | Function                                        |
|----------|---------------|-------------------------------------------------|
|       SP | Stack Pointer | Stores the current 'top' position of the stack. |
|       BP | Base Pointer  | Stores the current stack frame position.        |
|       CP | Code Pointer  | Stores the instruction which is executed next.  |
|       FG | Flag Register | Stores the state of the flags.                  |

Stack, Base and Code Pointer store indexes instead of actual memory addresses.
This prevents the VM to execute invalid instructions as the code pointer
always points to the start of an instruction.

Unlike common on most of the current CPUs, the stack and base pointer are growing upwards,
each push increments the stack pointer by one, each pop decrements it.

All registers start initialized with a zero.

### Stack Pointer
The stack pointer points to the top of the stack. Each `push` operation increases
the stack pointer by one, each `pop` operation reduces it by one.

### Base Pointer and Function Calls
The base pointer is a pointer that can be set to access the stack relative to it.
This relative access is done by the commands `get` and `set`.

The base pointer is designed to create stack frames for functions with local variables
as it is not possible to access local variables on the stack with only push and pop
operations.

### Code Pointer
The code pointer contains the instruction which is executed next. Modifying the
code pointer is equivalent to a jump operation.

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

| State | Binary Representation |
| X     | 0b00                  |
| 0     | 0b10                  |
| 1     | 0b11                  |

An instruction is only executed when all conditions are met.

| Flag      | Range |
|-----------|-------|
| Zero      | X/0/1 |
| Negative  | X/0/1 |

### Inputs
Each instruction has a defined set of inputs. The inputs are parameters for the
executed commands and allow a flexible configuration.

The first input can utilize all input methods, the second only provides `zero` and `pop`.

| # | Method   | Description                                      |
|---|----------|--------------------------------------------------|
| 0 | Zero     | The input value is zero.                         |
| 1 | Pop      | The input value is popped from the stack.        |
| 2 | Peek     | The input value is the top value of the stack.   |
| 3 | Argument | The instruction argument is copied to the input. |

### Commands
A command is the execution part of an instruction. It defines a core operation
which does the effective calculations.

Each command can be seen as a function defined as:

	output command(input0, input1, argument, cmdinfo)

The function has 4 inputs which can be used to calculate the output or change the
vm state. Each command also has the option to output a specific value that can be 
processed further.

| ID | Command     | Action                               | Description 
|----|-------------|--------------------------------------|--------------------------------------|
|  0 | COPY        | output = input0                      | Copies a value to the output.                          |
|  1 | STORE       | output = MEMORY[input0] = input1     | Stores a value in process memory.                      |
|  2 | LOAD        | output = MEMORY[input0]              | Loads a value from process memory.                     |
|  3 | GET         | output = STACK[BP + input0]          | Reads a value from the stack with base pointer offset. |
|  4 | SET         | output = STACK[BP + input0] = input1 | Writes a value to the stack with base pointer offset.  |
|  5 | BPGET       | output = BP                          | Gets the base pointer.                                 |
|  6 | BPSET       | output = BP = input0                 | Sets the base pointer.                                 |
|  7 | CPGET       | output = CP + cmdinfo                | Gets the current program counter with an offset.       |
|  8 | MATH        | output = input0 OP[info] input1      | Does an ALU operation.                                 |
|  9 | SPGET       | output = SP + input0                 | Gets the current stack pointer.                        |
| 10 | SPSET       | output = SP + input0 = input1        | Sets the current stack pointer.                        |
| 11 | SYSCALL     | output = SysCall(input0, input1)     | Calls an OS dependend operation.                       |
| 12 | HWIO        | output = HardwareIO(input0, input1)  | Calls an abstract hardware operation.                  |

#### Copy
This command just copies the first input value to the output value. It
can be used for a broad variety of instructions like modifying the stack,
jumping or constant flag modification.

#### Store, Load
These command accesses the process memory. `Store` writes value to process memory,
`Load` reads a value from it. The command info defines, what kind of
value is written:

| cmdinfo | Value type | Size in Bytes |
|---------|------------|---------------|
|       0 | uint8_t    | 1             |
|       1 | uint16_t   | 2             |
|       2 | uint32_t   | 4             |

#### Get, Set
`Get` and `Set` are used to modify the local stack frame. They allow modification
of the stack around the base pointer with a given offset.
`Get` reads a value from the stack, `Set` writes a value to the stack.

#### BpGet, BpSet
These commands modify the base pointer to set the stack offset for `Get` and `Set`.

#### CpGet
This command reads the current program counter and returns it. The program counter
is also offsetted by the command info.

#### Math
The math command is a compound operator that contains all 
ALU operations. The ALU operation is selected
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

#### SpGet, SpSet
These commands modify the stack pointer directly.
`SpGet` reads the stack pointer, `SpSet` writes the stack pointer.

#### SysCall
This command provides an interface to the executing host system.
The effects, parameters and results for this command must be defined by
the host.

#### HwIO
This command also provides an interface to the executiing host system,
but is focused on hardware IO. The effects, parameters and results are
also defined by the host.

### Command Info
The `cmdinfo` part of the instruction is passed to the executing command
adding some non-dynamic information for the execution.

### Flag Modification
This part defines if the instruction should change the flags according to
its output.
The *Zero* flag is set when the output of a command is zero, the *negative* flag
is set when the highest bit is set.

### Output
Each instruction can emit an output value. The output can be used in the following ways:

| # | Output  | Effect                                                       |
|---|---------|--------------------------------------------------------------|
| 0 | discard | The output value is discarded.                               |
| 1 | push    | The output is pushed to the stack.                           |
| 2 | jump    | The code pointer is set to the output, thus a jump is taken. |

### Argument
The instruction argument can provide static input which can be used
as a value source for the first input value.

## Common Assembler Mnemonics

The following table lists a set of practial mnemonics for the use in an assembly language.
Each mnemonic declares a specific configuration of an instruction.

| Mnemonic | Arg? | i0   | i1   | Cmd     | CmdInfo  | Output  | Flags? | Description  1                                                                                   |
|----------|------|------|------|---------|----------|---------|--------|-------------------------------------------------------------------------------------------------|
| nop      |   no | zero | zero | copy    | 0        | discard | no     | Does noting                                                                                     |
| push     |  yes | arg  | zero | copy    | 0        | push    | no     | Pushes its argument on the stack                                                                |
| drop     |   no | pop  | zero | copy    | 0        | discard | no     | Removes the top value from the stack                                                            |
| dup      |   no | peek | zero | copy    | 0        | push    | no     | Duplicates the top value on the stack.                                                          |
| jmp      |  yes | arg  | zero | copy    | 0        | jump    | no     | Jumps to its argument.                                                                          |
| jmpi     |   no | pop  | zero | copy    | 0        | jump    | no     | Pops a value from the stack and jumps to it.                                                    |
| ret      |   no | pop  | zero | copy    | 0        | jump    | no     | Pops a value from the stack and jumps to it.                                                    |
| load     |  yes | arg  | zero | load    | 0        | push    | no     | Loads the value located at the argument from memory.                                            |
| loadi    |   no | pop  | zero | load    | 0        | push    | no     | Pops an address from the stack and loads the value from memory.                                 |
| store    |  yes | arg  | pop  | store   | 0        | discard | no     | Pops a value and stores it in memory at the argument address.                                   |
| storei   |   no | pop  | pop  | store   | 0        | discard | no     | First, pops an address from the stack, then a value. Stores the value at the address.           |
| get      |  yes | arg  | zero | get     | 0        | push    | no     | Pushes the value at the given base pointer offset at the stack.                                 |
| geti     |   no | pop  | zero | get     | 0        | push    | no     | Pops an offset from the stack, then pushes the value at the popped base pointer offset.         |
| set      |  yes | arg  | pop  | set     | 0        | discard | no     | Pops a value from the stack and copies it to the given stack offset.                            |
| seti     |   no | pop  | pop  | set     | 0        | discard | no     | Pops an offset from the stack, then a value. Stores the value at the given offset on the stack. |
| bpget    |   no | zero | zero | bpget   | 0        | push    | no     | Pushes the current base pointer.                                                                |
| bpset    |   no | pop  | zero | bpset   | 0        | discard | no     | Pops a value and sets the base pointer.                                                         |
| spget    |   no | zero | zero | spget   | 0        | push    | no     | Pushes the current stack pointer.                                                               |
| spset    |   no | pop  | zero | spset   | 0        | discard | no     | Pops a value and sets the stack pointer.                                                        |
| cpget    |   no | zero | zero | cpget   | 1        | push    | no     | Pushes the address of the second-next instruction.                                              |
| add      |   no | pop  | pop  | math    | 0        | push    | no     | Pops two values and pushes the sum.                                                             |
| sub      |   no | pop  | pop  | math    | 1        | push    | no     | Pops two values and pushes the difference.                                                      |
| cmp      |   no | pop  | pop  | math    | 1        | discard | yes    | Pops two values and subtracts them. Discards the result, but modifies the flags.                |
| mul      |   no | pop  | pop  | math    | 2        | push    | no     | Pops two values and pushes the product.                                                         |
| div      |   no | pop  | pop  | math    | 3        | push    | no     | Pops two values and pushes the quotient.                                                        |
| mod      |   no | pop  | pop  | math    | 4        | push    | no     | Pops two values and pushes the euclidean quotient.                                              |
| and      |   no | pop  | pop  | math    | 5        | push    | no     | Pops two values and pushes the bitwise and.                                                     |
| or       |   no | pop  | pop  | math    | 6        | push    | no     | Pops two values and pushes the bitwise or.                                                      |
| xor      |   no | pop  | pop  | math    | 7        | push    | no     | Pops two values and pushes the exclusive or.                                                    |
| not      |   no | pop  | zero | math    | 8        | push    | no     | Pops a single value and pushes the bitwise not                                                  |
| rol      |   no | pop  | pop  | math    | 9        | push    | no     | TO BE SPECIFIED                                                                                 |
| ror      |   no | pop  | pop  | math    | 10       | push    | no     | TO BE SPECIFIED                                                                                 |
| asl      |   no | pop  | pop  | math    | 11       | push    | no     | TO BE SPECIFIED                                                                                 |
| asr      |   no | pop  | pop  | math    | 12       | push    | no     | TO BE SPECIFIED                                                                                 |
| shl      |   no | pop  | pop  | math    | 13       | push    | no     | TO BE SPECIFIED                                                                                 |
| shr      |   no | pop  | pop  | math    | 14       | push    | no     | TO BE SPECIFIED                                                                                 |
| syscall  |  yes | zero | zero | syscall | 0        | discard | no     | Mnemonic for a generic syscall.                                                                 |
| hwio     |  yes | zero | zero | hwio    | 0        | discard | no     | Mnemonic for a generic hwio.                                                                    |

## Function Calls

The following chapter defines the SuperVM calling convention. It is required that all
functions conform to this convention.

To call a function, it is required that the return address is pushed to the stack.
After this, a jump is taken to the function address.

	call:
		push @returnPoint ; Pushing returnPoint as the return address
		jmp @function     ; Jumps to the function
	returnPoint:

SuperVM provides the instruction `cpget` which pushes by default the address of the 
second next instruction which resembles the code above. This behaviour allows position
independent code:

	call:
		cpget         ; pushs implicit returnPoint
		jmp @function ; Calls function

Functions can now return by calling `ret` when the return address is on top of the stack.
A simple function that does a system call may look like this:

	function:
		syscall
		ret

As most functions utilize local variables, a stack frame is required.
Creating this stack frame is done by pushing the current base pointer, then
setting the base pointer to the current stack pointer.

	enter:
		bpget ; Save current base pointer
		spget ; Get current stack pointer
		bpset ; Set new base pointer

Returning a function with this mechanism is by setting the stack pointer to the current
base pointer, then popping the previous base pointer from the stack.

	leave:
		bpget ; Get current base pointer
		spset ; Restore stack saved at the beginning
		bpset ; Restore previous base pointer
		ret   ; and jumping back.

This mechanism leaves the base pointer of the calling function intact and also provides
a new base pointer for the current function.

## TODO

- 64 Bit arithmetic instructions