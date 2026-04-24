    global frame
    global image

    %include "core/demo.asm"
    %include "framework/asm/palette.asm"

    %ifndef COM
    section .bss
image:
    resb BUFFER_SIZE
    %endif
