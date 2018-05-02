	.globl	zfunction
zfunction:	
	pushl	%ebp
	movl	%esp,%ebp
	movl	8(%ebp),%eax    # Move from BRELOC to 
	#movl	12(%ebp),%ebx		# to relocated area

	and	$0xffc01fff,%eax  # Size to Move (should be _edata)
	shl	$4,%eax
	#leave
	movl	%ebp, %esp
	pop	%ebp	
	ret

