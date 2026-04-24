    %ifndef CORE_PALETTE_ASM
    %define CORE_PALETTE_ASM

; Grayscale palette generation
; defines the PALETTE_OUT macro with al argument:
; - On DOS: outputs to VGA palette port (dx = PALETTE_DATA_PORT)
; - On 32-bit: writes to palette buffer (edi as write offset)

palette_loop:
    mov al, bl
    shr al, 2
    PALETTE_OUT                        ; G
    PALETTE_OUT                        ; B
    PALETTE_OUT                        ; B
    inc bx
    loop palette_loop

    %endif
