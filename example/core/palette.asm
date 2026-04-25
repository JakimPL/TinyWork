    %ifndef CORE_PALETTE_ASM
    %define CORE_PALETTE_ASM

; Grayscale palette generation
; defines the PALETTE_OUT macro with al argument:
; - On DOS: outputs to VGA palette port (dx = PALETTE_DATA_PORT)
; - On 32-bit: writes to palette buffer (edi as write offset)

palette:
    %ifdef DOS
    mov dx, PALETTE_DATA_PORT
    mov cx, 0x100                      ; 256 colors, consider using cl
    %else
    mov edi, palette_data
    mov ecx, 0x100
    %endif

.palette_loop:
    mov al, bl
    shr al, 2
    PALETTE_OUT                        ; G
    PALETTE_OUT                        ; B
    PALETTE_OUT                        ; B
    inc bx
    loop .palette_loop

    %endif
