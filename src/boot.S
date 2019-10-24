bits 64
%define MAGIC 0x1badb002
%define FLAGS (1<<0 | 1<<1)
%define CHECKSUM -(MAGIC + FLAGS)

extern kernel_main
global entry
global cpuidavailable

section .multiboot
mb_header:
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .data
align 4
gdt:
    dw .gdt_end - .gdt_start - 1
    dd .gdt_start
    align 16
    .gdt_start:
        dq 0
        dw 0xffff
        dw 0x0000
        db 0x00
        db 10011010b
        db 11001111b
        db 0x00
        dw 0xffff
        dw 0x0000
        db 0x00
        db 10010010b
        db 11001111b
        db 0x00
        .gdt_end:

cpuidavailable:
    pushfq
    pushfq
    xor dword [rsp],0x00200000
    popfq
    pushfq
    pop rax
    xor rax,[rsp]
    popfq
    and rax,0x00200000
    ret

section .text
entry:
    cli
    mov rsp, stack_top
    push rax
    push rbx
    lgdt [gdt]
    cld
    call kernel_main
    .stop:
    cli
    hlt
    jmp .stop

section .bss
stack_bottom:
    resb 16384 ; 16 KiB
stack_top:
