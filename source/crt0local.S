/* minimal replacement of crt0.o which is else provided by C library */

.globl main
.globl _start
.globl __start
.globl _heap_stack_start
.globl _heap_stack_end
.set noat
.set noreorder
.ent _start

.text

__start:
_start:
#if defined(__PIC__) || defined(__pic__)
	bal     next
	nop
next:
	.set    noreorder
	.cpload $31
	.set    reorder
#else
	la      $gp, _gp
#endif
	la      $sp, _heap_stack_end
	addi    $a0, $zero, 0
	addi    $a1, $zero, 0
	jal     main
	nop
quit:
	addi    $a0, $zero, 0
	addi    $v0, $zero, 4001  /* SYS_exit */

	syscall

loop:	break
	beq     $zero, $zero, loop
	nop

.end _start

.bss

_heap_stack_start:
	.skip  0x800000
_heap_stack_end:
