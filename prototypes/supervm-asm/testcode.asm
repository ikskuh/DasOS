	push 0 ; Some comment
loop:
	dup
	[f:yes] sub 10 [r:discard]
	[ex(z)=1] jmp @end
	add 1
	jmp @loop
end: