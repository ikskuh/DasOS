	push 0 ; Some comment
loop:
	dup
	[i0:arg] sub 10 [f:yes] [r:discard]
	[ex(z)=1] jmp @end
	[i0:arg] add 1
	jmp @loop
end: