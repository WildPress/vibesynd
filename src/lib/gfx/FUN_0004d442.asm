; FUN_0004d442 @ 0004d442  (15 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004d442.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004d442:
        mov     ah, 1                            ; b401
        int     0x16                             ; cd16
        je      0x4d44e                          ; 7406
        mov     ah, 0                            ; b400
        int     0x16                             ; cd16
        jmp     0x4d450                          ; eb02
        mov     al, 0                            ; b000
        ret                                      ; c3
