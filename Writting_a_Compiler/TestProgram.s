.globl _main
_main:
push %ebp
movl %esp, %ebp
subl $8,%esp
movl $1, %eax
movl %eax, -4(%ebp)
push %eax
movl $0, %eax
movl %eax, -8(%ebp)
while_loop0:
while_loop0:
movl -4(%ebp), %eax
push %eax
movl $0, %eax
push %eax
pop %ecx
pop %eax
cmpl %ecx, %eax
movl $0, %eax
setg %al
cmpl $0, %eax
je exit_while0
subl $4,%esp
movl -8(%ebp), %eax
push %eax
movl -4(%ebp), %eax
push %eax
pop %ecx
pop %eax
addl %ecx, %eax
movl %eax, -12(%ebp)
push %eax
movl -12(%ebp), %eax
push %eax
lea text0, %eax
push %eax
call _printf
addl $8, %esp
movl -4(%ebp), %eax
movl %eax, -8(%ebp)
movl -12(%ebp), %eax
movl %eax, -4(%ebp)
addl $4,%esp
jmp while_loop0
exit_while0:
while_loop1:
movl $1, %eax
cmpl $0, %eax
je exit_while1
jmp while_loop1
exit_while1:
addl $8,%esp
movl $0, %eax
movl %ebp, %esp
pop %ebp
ret
.globl text0
.data
text0:
.string "%d\n"
