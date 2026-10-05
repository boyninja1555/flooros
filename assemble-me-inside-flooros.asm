# Assemble with:
# 1.    as -o egg.o assemble-me.asm
# 2.    ld -o egg egg.o

.intel_syntax noprefix
.global _start

.section .data
    note:
        .string "Thanks for using FloorOS!\n"

.text
_start:
    # sys_write
    mov rax, 1             # syscall: write
    mov rdi, 1             # fd: stdout
    lea rsi, [rip + note]  # buf: note
    mov rdx, 26            # count: 26
    syscall

    # sys_exit
    mov rax, 60  # syscall: exit
    mov rdi, 0   # status: 0
    syscall
