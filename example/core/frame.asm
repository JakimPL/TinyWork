; Simple gradient effect

    %ifdef COM
    %define REG(x) x
    %else
    %define REG(x) e%+x
    %endif

    %ifdef COM
    xor di, di
    %else
    mov edi, image
    %endif

    mov bx, SCREEN_HEIGHT
    mov cx, SCREEN_WIDTH
.draw_loop:
    mov ax, bx
    shr ax, 1
    rep stosb
    add di, cx
    dec bx
    jnz .draw_loop
