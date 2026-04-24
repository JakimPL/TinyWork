; Minimal COM demo using TinyWork framework
; Standalone 256-byte DOS executable

    org 0x0100

    %include "../asm/fconsts.asm"

    section .text
start:
; Set VGA mode 13h (320x200 256 colors)
    mov al, VIDEO_MODE_13H
    int BIOS_VIDEO_INTERRUPT

; Setup video memory segment
    push VIDEO_MEMORY_SEGMENT
    pop es

main_loop:
; Clear screen / draw frame
    xor di, di
    mov cx, VIDEO_BUFFER_SIZE
    xor ax, ax

draw_frame:
; Simple pattern: fill with gradient
    mov al, cl
    stosb
    loop draw_frame

; Check for ESC key
    in al, KEYBOARD_DATA_PORT
    dec ax
    jnz main_loop

; Return to text mode
    mov ax, TEXT_MODE_3H
    int BIOS_VIDEO_INTERRUPT
    ret
