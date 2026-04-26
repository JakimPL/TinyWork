    %ifndef TINYWORK_FRAME
    %define TINYWORK_FRAME

    section .text
frame:
    %ifndef COM
    pusha
    %endif

    %include "frame.asm"

    %ifndef COM
    popa
    ret

    %include "tiny/includes.asm"

    section .bss
image:
    resb BUFFER_SIZE
    %endif

    %endif
