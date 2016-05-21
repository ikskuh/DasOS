
	[i0:arg] syscall [ci:1] 'H'
	[i0:arg] syscall [ci:1] 'a'
	[i0:arg] syscall [ci:1] 'l'
	[i0:arg] syscall [ci:1] 'l'
	[i0:arg] syscall [ci:1] 'o'

	load 0
	load 1
	load 2
	load 3
	
	store 67
	store 66
	store 65
	store 64
	




	push 0 ; Some comment
_loop:
	dup
	[i0:arg] sub 10 [f:yes] [r:discard]
	[ex(z)=1] jmp @_end
	[i0:arg] add 1
	jmp @_loop
_end:
	syscall [ci:0]; Exit