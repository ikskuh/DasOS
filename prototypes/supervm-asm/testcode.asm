
	[i0:arg] syscall [ci:1] 'S'

	push 1
	push 2
	push 3
	push 4

	cpget
	jmp @print_str
	
	[i0:arg] syscall [ci:1] 'Q'
	
	syscall [ci:0] ; Exit
	
print_str:	
	spget
	bpset
	
	push 200
	get -1
	get -2
	get -3
	get -4
	push 400
	
	get 1
	
	push 600
	
	
	
	bpget
	spset
	jmpi
	
;	load 0
;	load 1
;	load 2
;	load 3
	
;	store 67
;	store 66
;	store 65
;	store 64
	




;	push 0 ; Some comment
;_loop:
;	dup
;	[i0:arg] sub 10 [f:yes] [r:discard]
;	[ex(z)=1] jmp @_end
;	[i0:arg] add 1
;	jmp @_loop
;_end:
;	syscall [ci:0]; Exit