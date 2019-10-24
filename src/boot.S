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

bits 32
cpuidavailable:
    pushfd
    pushfd
    xor dword [esp],0x00200000
    popfd
    pushfd
    pop eax
    xor eax,[esp]
    popfd
    and eax,0x00200000
    ret

section .text
entry:
    cli
    mov esp, stack_top
    push eax
    push ebx
    lgdt [gdt]
    jmp 0x08:.reload_cs
    .reload_cs:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
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
