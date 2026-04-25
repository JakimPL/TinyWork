    %ifndef FRAMEWORK_CONSTS_FRAME
    %define FRAMEWORK_CONSTS_FRAME

    section .text
frame:
    %ifndef COM
    pusha
    %endif

    %include "core/frame.asm"

    %ifndef COM
    popa
    ret

    %include "tiny/includes.asm"

    section .bss
image:
    resb BUFFER_SIZE
    %endif

    %endif
