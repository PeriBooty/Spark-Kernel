MODULEALIGN equ  1 << 0
MEMINFO equ  1 << 1
VIDEO equ  1 << 2
FLAGS equ MODULEALIGN | MEMINFO | VIDEO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

KERNEL_VMA equ 0xFFFFFFFF80000000

extern kernel_main
bits 32

global _loader
global loader
loader equ (_loader - KERNEL_VMA)

section .multiboot
align 64
mb_header:
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 1280
    dd 720
    dd 32

section .data
align 0x1000
init_pml4:
    dq init_pdp1 - KERNEL_VMA + 3
    times 255 dq 0
    dq phys_pdp - KERNEL_VMA + 3
    times 254 dq 0
    dq init_pdp2 - KERNEL_VMA + 3

%macro gen_pd_2mb 3
    %assign i %1
    %rep %2
        dq (i | 0x83)
        %assign i i + 0x200000
    %endrep
    %rep %3
        dq 0
    %endrep
%endmacro

align 0x1000
init_pdp1:
    dq init_pd - KERNEL_VMA + 3
    times 511 dq 0

align 0x1000
init_pdp2:
    times 510 dq 0
    dq init_pd - KERNEL_VMA + 3
    dq 0

align 0x1000
phys_pdp:
    dq phys_pd - KERNEL_VMA + 3
    times 511 dq 0

align 0x1000
init_pd:
    gen_pd_2mb 0, 64, 448

align 0x1000
phys_pd:
    gen_pd_2mb 0, 512, 0

section .data

align 0x10
init_gdt:
    dq 0
    dq 0x00209A0000000000
init_gdt_end:

init_gdt_ptr:
    dw init_gdt_end - init_gdt - 1
    dq init_gdt - KERNEL_VMA

init_gdt_ptr_high:
    dw init_gdt_end - init_gdt - 1
    dq init_gdt

section .text

_loader:
    cli
    lgdt [init_gdt_ptr - KERNEL_VMA]
    mov esp, stack_end - KERNEL_VMA
	push 0
    push eax
    push 0
    push ebx 
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb stop
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz stop  
    mov eax, cr4
    or eax, 0x000000A0
    mov cr4, eax
    mov eax, init_pml4 - KERNEL_VMA
    mov cr3, eax
    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x00000101
    wrmsr
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax
    jmp 0x08:higher_half_entry

bits 64

%macro pushf 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popf 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r8
	pop r9
	pop rdi
	pop rsi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

extern irq1_handler
global irq1
irq1:
    pushf
    call irq1_handler
    popf
    iretq

higher_half_entry equ (_higher_half_entry - KERNEL_VMA)

_higher_half_entry:
    mov rax, _entry
    jmp rax

_entry:
    lgdt [init_gdt_ptr_high]
    mov ax, 0x0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov gs, ax
    mov fs, ax
    add rsp, KERNEL_VMA
    pop rdi
    pop rsi
    call kernel_main
    hlt

stop:
    hlt

section .bss
align 0x1000
stack:
    resb 0x10000

stack_end: