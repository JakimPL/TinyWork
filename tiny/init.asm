    %ifndef TINYWORK_INIT
    %define TINYWORK_INIT

    section .text
initialize:
    %ifndef COM
    pusha
    %endif

    %include "core/init.asm"

    %ifndef COM
    popa
    ret
    %endif

    %endif
