/* Sample initialization file */
	
	.extern	main
	.extern	exit
	
/* .text is used instead of .section .text so it works with arm-aout too.  */
	.text
	.code 32
	.align 	0

	.global	_mainCRTStartup
	.global	_start
	.global	start

start:
_start:
_mainCRTStartup:

/* Start by setting up a stack */
	/*  Set up the stack pointer to end of bss */
	ldr	r3, .LC2
	mov 	sp, r3

	blx	main

	/* For Thumb, constants must be after the code since only 
	positive offsets are supported for PC relative addresses. */
	
	.align 0

.LC2:
	.word	__stack

