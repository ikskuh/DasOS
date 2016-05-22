
	; Just print a "START" flag
	[i0:arg] syscall [ci:1] 'S'


	push 0 ; pushs string pointer argument
	cpget  ; pushs return addr
	jmp @print_str ; Calls print_str(0)
	drop ; releases argument
	
	
	; Print the "END" marker
	[i0:arg] syscall [ci:1] 'Q'
	
	; End the program
	syscall [ci:0]
	
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
	;		char c = *ptr; // which is implicitly done by our while(*ptr)
	
	;		putc(c);
	[i0:pop] syscall [ci:1] ; removes the loaded character and prints it
	
	;		ptr++;
	[i0:arg] add 1 ; adds 1 to the stack top
	
	;	}
	jmp @print_str_loop
	
print_str_end_loop:
	drop ; discard the result from loadi
	drop ; discard our pointer
	
	; return
	bpget ; leave function
	spset ; by restoring parent base pointer
	bpset
	jmpi  ; and jumping back.
