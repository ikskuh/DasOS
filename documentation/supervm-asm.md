# SuperVM Assembly Language

## Purpose of this Document
The SuperVM Virtual Machine only specifies assembler mnemonics and an instruction format,
but does not define an assembler dialect.

This document describes the reference dialect of the SuperVM assembly language.

## Overview

The SuperVM Assembly Language, short *SVMA*, provides a full interface to the features
of the SuperVM Virtual Machine Instruction Set. It is possible to create any kind of
program, even optimized ones that utilize the full power of the instruction set.

The assembly language provides five basic components:

- Labels
- Mnemonics
- Parameters
- Attributes
- Comments

The following code snippet gives a brief overview over the assembly language:

	_start:
		push 10
		push 33
		add     ; Add 10 and 33
		[i0:arg] sub -1

All components are present in this snippet. `_start:` is a label, `push`, `add` and
`sub` are mnemonics, `10`, `33`, `-1` are the parameters and `[i0:arg]` is an attribute.
`; Add 10 and 33` is a comment.

## Components

The SVMA is a line based language where a line can be blank, a sole comment or a mnemonic definition
following one of the next schemes.

	[comment]
	label [comment]
	[label] [attribute]* mnemonic [attribute]* [argument] [attribute]* [comment]

Each component that is written in `[]` is optional. If a component is followed by an
asterisk (`*`) the component can be written repeatedly. A singular white space in this 
scheme can be replaced by any non-empty combination of whitespace and tab characters.

The verbal description of the language is rather easy: Each line may be terminated by a comment.
A line may start with a label and if it does so, the mnemonic can be omitted. If no label
is given, a mnemonic is necessary. A mnemonic can be followed by an argument.
Attributes may be inserted between any of the components, but can not be in front of the label
or behind the comment.

### Labels
A label defines a kind of marker that stores the current code position.
Labels are commonly used to create jump targets for instructions.

A label is a word string that is follow by a color (`:`) at the beginning of a line.
Anything before the colon is the name of the label and can later be referenced.

### Mnemonics
Mnemonics are short hand names of SuperVM instructions. Each mnemonic will be emitted as
an instruction in the binary code later.

A mnemonics is a short, usually 3 or 4 letter long word that describes the instruction
beeing emitted.

A list of all mnemnonics with descriptions can be found in the Document *Common Assembler Mnemonics*.

### Arguments
Each mnemonic can be followed by an argument which will be emitted as a 32 bit value.
SVMA provides 5 different types of argument descriptions:

| Type    | Example  | Description                            |
|---------|----------|----------------------------------------|
| Binary  | `0b1101` | Provides the literal binary value.     |
| Decimal | `123`    | Provides the literal decimal value     |
| Hexcode | `0xF00F` | Provides the literal hexadeciaml value |
| Char    | `'c'`    | Provides an ascii character code.      |
| Label   | `@label` | Provides the position of the label.    |

### Attributes
An attribute overrides any part of the mnemonic with a custom option. It can be used
to optimize instructions so they require a minimum of input.

#### Primitive Attributes

The following table lists all possible primitive attributes an instruction can have:

| Attribute   | Modification                                       |
|-------------|----------------------------------------------------|
| `f:yes`     | Sets the flag option in the instruction.           |
| `f:no`      | Clears the flag option in the instruction.         |
| `r:discard` | Sets the output mode to *discard*.                 |
| `r:push`    | Sets the output mode to *push*.                    |
| `r:jump`    | Sets the output mode to *jump*.                    |
| `i0:zero`   | Sets input0 to *zero*.                             |
| `i0:pop`    | Sets input0 to *pop value*.                        |
| `i0:peek`   | Sets input0 to *peek value*.                       |
| `i0:arg`    | Sets input0 to *use argument*.                     |
| `i1:zero`   | Sets input1 to *zero*.                             |
| `i1:pop`    | Sets input1 to *pop value*.                        |
| `ex(z)=x`   | Sets the execution for the Z flag to *don't care*. |
| `ex(z)=0`   | Sets the execution for the Z flag to *0*.          |
| `ex(z)=1`   | Sets the execution for the Z flag to *1*.          |
| `ex(n)=x`   | Sets the execution for the N flag to *don't care*. |
| `ex(n)=0`   | Sets the execution for the N flag to *0*.          |
| `ex(n)=1`   | Sets the execution for the N flag to *1*.          |

#### Modifying cmdinfo
The SVMA provides two more complex attributes for modificating the command and the cmdinfo
of an instruction.

To modify the cmdinfo part of the instruction, the attribute `[ci:???]` is used. `???` must
be replaced by any valid decimal unsigned 16 bit wide number.

#### Modifying the command
Another complex attribute allows the modification of the actual command used: `[cmd:???]`.

Here `???` can be replaced with one of the following commands:

| Command |
|---------|
| Copy    |
| Store   |
| Load    |
| Get     |
| Set     |
| BpGet   |
| BpSet   |
| CpGet   |
| Math    |
| SpGet   |
| SpSet   |
| SysCall |
| HwIO    |

## Example Code
This example implements the given C function `print_str`;

	void print_str(char *string)
	{
		char *ptr = string;
		while(*ptr) {
			char c = *ptr;
			putc(c); // Syscall 1
			ptr++;
		}
		return;
	}

which is represented in SVMA:

	; void print_str(char *string);
	print_str:
		bpget ; enter function by
		spget ; saving the parents base pointer
		bpset ; and storing the current stack pointer
			 
		
		; char *ptr = string;
		get -2 ; get argument 0 into our local variable '#1'
		
		; while(*ptr) {
	print_str_loop:
		[i0:peek] loadi [f:yes] ; write flags, also load result, don't discard pointer
		[ex(z)=1] jmp @print_str_end_loop ; when *ptr == 0, then goto print_str_end_loop
		; char c = *ptr; // which is implicitly done by our while(*ptr)
		
		; putc(c);
		[i0:pop] syscall [ci:1] ; removes the loaded character and prints it
		
		; ptr++;
		[i0:arg] add 1 ; adds 1 to the stack top
		
		; }
		jmp @print_str_loop
		
	print_str_end_loop:
		drop ; discard the result from loadi
		drop ; discard our pointer
		
		; return
		bpget ; leave function
		spset ; by restoring parent base pointer
		bpset
		jmpi  ; and jumping back.