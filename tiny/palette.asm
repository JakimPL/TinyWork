    global set_palette

    %ifdef DOS
    %macro PALETTE_OUT 0
    out dx, al
    %endmacro
    %else
    %macro PALETTE_OUT 0
    mov [palette_data + edi], al
    inc edi
    %endmacro

    section .data
    global palette_data
    palette_data: times 768 db 0       ; 256 colors × 3 RGB components
    %endif

    section .text
set_palette:
    %ifdef DOS
    %ifndef COM
    pusha
    xor bx, bx
    xor cx, cx
    mov dx, PALETTE_INDEX_PORT
    int BIOS_VIDEO_INTERRUPT
    %endif

    mov dx, PALETTE_DATA_PORT
    mov cl, 0xFF
    %else
    push ebx
    push edi
    xor ebx, ebx
    xor edi, edi
    mov ecx, 0x100
    %endif

    %include "core/palette.asm"

    %ifdef DOS
    %ifndef COM
    popa
    ret
    %endif
    %else
    pop edi
    pop ebx
    ret
    %endif
