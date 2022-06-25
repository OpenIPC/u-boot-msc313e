/*------------------------------------------------------------------------------
    Function Code
-------------------------------------------------------------------------------*/

	.align	5
.globl reset_cpu
reset_cpu:

    ldr   r1, =0x1F003C18
    mov   r3, #0x00FF
    str   r3, [r1]
    mov   r3, #0x0079
	str   r3, [r1]
	nop
	nop
	nop
	nop

_loop_forever:
	b	_loop_forever
