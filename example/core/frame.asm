; Simple gradient effect

    %ifdef COM
    %define REG(x) x
    %define MEM(offset) [es:offset]
    %else
    %define REG(x) e%+x
    %define MEM(offset) [image + offset]
    %endif

    xor REG(cx), REG(cx)
    xor REG(di), REG(di)

.draw_loop:
    mov al, cl

    %ifdef COM
    stosb
    %else
    mov [image + edi], al
    inc edi
    %endif

    inc REG(cx)
    cmp REG(cx), BUFFER_SIZE
    jl .draw_loop
