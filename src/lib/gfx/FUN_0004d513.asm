; FUN_0004d513 @ 0004d513  (25 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004d513.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004d513:
        mov     cx, 8                            ; 66b90800
        rol     eax, 4                           ; c1c004
        mov     ebx, eax                         ; 8bd8
        and     ebx, 0xf                         ; 83e30f
        mov     bl, byte ptr [ebx + 0x3fde4]     ; 8a9be4fd0300
        mov     byte ptr [edi], bl               ; 881f
        inc     edi                              ; 47
        loop    0x4d517                          ; 67e2ec
        ret                                      ; c3
