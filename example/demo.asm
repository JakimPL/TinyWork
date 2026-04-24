; Minimal 32-bit assembly demo
; Exports: frame(), image[]

    %include "../asm/fconsts.asm"

    global frame
    global image

    section .text

; Simple demo: fill screen with gradient
frame:
    pusha

    xor eax, eax
    mov edi, image
    mov ecx, BUFFER_SIZE

.fill_loop:
; Simple gradient pattern
    mov al, cl
    shr al, 8
    stosb
    loop .fill_loop

    popa
    ret

    section .bss
image:
    resb BUFFER_SIZE
