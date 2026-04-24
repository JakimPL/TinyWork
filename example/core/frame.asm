; Simple gradient effect

    section .text
set_registers:
    %ifdef COM
    xor di, di
    %else
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    mov edi, image
    %endif

draw:
    mov bx, SCREEN_HEIGHT
.draw_loop:
    mov cx, SCREEN_WIDTH
    mov ax, bx
    shr ax, 1
    rep stosb
    dec bx
    jnz .draw_loop
.exit:
