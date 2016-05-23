# Common Assembler Mnemonics

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
