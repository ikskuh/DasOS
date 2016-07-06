# Assembler Mnemonics

## nop
Does nothing.

## push
Pushes $arg on the stack.

## drop
Removes the top value from the stack.

## dup
Duplicates the top value of the stack.

## jmp
Jumps to the command at index $arg.

## jmpi
Pops an index from the stack and jumps to it.

## ret
Returns from a function. Is exactly the same as `jmpi`, but is ment for returning from a function instead of an indirected jump.

## load8
Loads a byte (8 bit) from $arg.

## load16
Loads a half world (16 bit) from $arg.

## load32
Loads a word (32 bit) from $arg.

## load8i
Pops an address from the stack and loads the byte (8 bit) located at the popped address.

## load16i
Pops an address from the stack and loads the half world (16 bit) located at the popped address.

## load32i
Pops an address from the stack and loads the word (32 bit) located at the popped address.

## store8
Pops a value and stores it as a byte (8 bit) at $arg.

## store16
Pops a value and stores it as a half word (16 bit) at $arg.

## store32
Pops a value and stores the word (32 bit) at $arg.

## store8i
First pops an address from the stack, then a value. Then stores the value as a byte (8 bit) at the popped address.

## store16i
First pops an address from the stack, then a value. Then stores the value as a half word (16 bit) at the popped address.

## store32i
First pops an address from the stack, then a value. Then stores the value (32 bit) at the popped address.

## get


## geti


## set


## seti


## bpget
Pushes the current base pointer to the stack.

## bpset
Pops a value into the base pointer.

## spget
Pushes the current stack pointer to the stack.

## spset
Pops a value into the stack pointer.

## cpget
Pushes the current code pointer incremented by one. This allows pushing return values for function calls.

## add


## sub


## cmp


## cmpi


## mul


## div


## mod


## and


## or


## xor


## not


## rol


## ror


## asl


## asr


## shl


## shr


## syscall


## hwio


## int
Raises the interrupt $arg.

## sei
Sets the interrupt flag and thus enables interrupts.

## cli
Clears the interrupt flag and thus disables interrupts.

## in
Reads a word from the port $arg.

## out
Pops a word and writes it to the port $arg.

