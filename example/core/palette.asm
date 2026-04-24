    %ifndef CORE_PALETTE_ASM
    %define CORE_PALETTE_ASM

; Grayscale palette generation
; This is the loop body only - framework provides the wrapper
; and defines the PALETTE_OUT macro:
; - On DOS: outputs to VGA palette port (dx = PALETTE_DATA_PORT)
; - On 32-bit: writes to palette buffer (edi as write offset)

.palette_loop:
    mov al, cl

    PALETTE_OUT al                     ; R
    PALETTE_OUT al                     ; G
    PALETTE_OUT al                     ; B

    %endif
