/*------------------------------------------------------------------------------
    Function Code
-------------------------------------------------------------------------------*/

	.align	5
.globl reset_cpu
reset_cpu:

    mov   r3, #0x0000
    ldr   r1, =0x1F005C80
    str   r3, [r1]
    mov   r3, #0x0079
	ldr	  r1, =0x1F005CB8
	str   r3, [r1]
	nop
	nop
	nop
	nop

_loop_forever:
	b	_loop_forever
