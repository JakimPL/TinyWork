    org 0x0100

    %include "tiny/consts.asm"
start:
.set_video_mode:
    mov al, VIDEO_MODE_13H
    int BIOS_VIDEO_INTERRUPT

palette:
    %include "tiny/palette.asm"

    section .text
main_loop:
.push_video_memory_segment:
    push VIDEO_MEMORY_SEGMENT
    pop es

    %ifndef NO_VSYNC
vsync:
    mov dx, VGA_INPUT_STATUS_REGISTER
.wait_start:
    in al, dx
    test al, VERTICAL_RETRACE_STATUS_BIT
    jz .wait_start
    %endif

    %include "tiny/frame.asm"

    section .text
check_input:
    in al, KEYBOARD_DATA_PORT
    dec ax
    jnz main_loop

    %ifdef RETURN_TO_DOS
return_to_dos:
    mov ax, TEXT_MODE_3H
    int BIOS_VIDEO_INTERRUPT
    ret
    %endif

    %include "tiny/includes.asm"
