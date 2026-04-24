    section .text
frame:
    %ifndef COM
    pusha
    %endif

    %include "core/frame.asm"

    %ifndef COM
    popa
    ret

    %include "framework/asm/includes.asm"

    section .bss
image:
    resb BUFFER_SIZE
    %endif
