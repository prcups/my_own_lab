.section .data
str:
.asciz "Hello World!\n"

.section .text
.globl main
main:
	pushl $str
	call printf
	popl %ebx
	movl $1, %eax
	movl $2, %ebx
	int $0x80
