.section .text
 
.extern init
.extern compat_call_ctors
.extern compat_call_dtors

.global _start
_start:
  mov $kernel_stack, %esp
 
  call compat_call_ctors
  call init
  call compat_call_dtors

_stop:
  cli
  hlt
  jmp _stop
    
.section .bss
.space 8192
kernel_stack: