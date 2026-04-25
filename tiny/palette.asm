    global set_palette

    %ifdef DOS
    %macro PALETTE_OUT 0
    out dx, al
    %endmacro
    %else
    %macro PALETTE_OUT 0
    stosb
    %endmacro

    section .data
    global palette_data
    palette_data: times 768 db 0       ; 256 colors × 3 RGB components
    %endif

    section .text
set_palette:
    %ifndef COM
    pusha
    %endif

    %ifdef DOS
    %ifndef COM
    xor bx, bx
    xor cx, cx
    mov dx, PALETTE_INDEX_PORT
    int BIOS_VIDEO_INTERRUPT
    %endif
    %else
    xor ebx, ebx
    xor ecx, ecx
    %endif

    %include "core/palette.asm"

    section .text
    %ifndef COM
    popa
    ret
    %endif
