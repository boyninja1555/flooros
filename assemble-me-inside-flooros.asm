; Assemble with:
; 1.    nasm -f elf64 -o egg.o assemble-me.asm
; 2.    ld -o egg egg.o

bits 64

section .data
    note db "Thanks for using FloorOS!", 0x0A, 0

section .text
    global _start

_start:
    ; sys_write
    mov rax, 1     ; syscall: write
    mov rdi, 1     ; fd: stdout
    mov rsi, note  ; buf: note
    mov rdx, 26    ; count: 26
    syscall

    ; sys_exit
    mov rax, 60  ; syscall: exit
    mov rdi, 0   ; status: 0
    syscall
