.section .text

.extern SYSCALLS

loader_run: // void (*)(void* main)
	
	// Get argument
	mov 4(%esp), %eax
	
	// Save kernel stack
	mov %esp, (kernel_stack_save)
	
	// Setup user stack
	mov $user_stack, %esp
	
	// Call main
	push $SYSCALLS
	call %eax
	add %esp, 4 // bullshit!

// jump here from userspace to restore kernel stack and functionality
userspace_exit:
	mov (kernel_stack_save), %esp
	
	ret

.section .data
// Storage of the current kernel stack pointer
kernel_stack_save:
.int 0

// 8 kB user space stack
.section .bss
.space 8192
user_stack:


.global loader_run
.global userspace_exit