; FUN_00049922 @ 00049922  (25 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00049922.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00049922:
        push    ax                               ; 6650
        push    dx                               ; 6652
        mov     dx, 0x3ce                        ; 66bace03
        mov     ax, 0xff08                       ; 66b808ff
        out     dx, ax                           ; 66ef
        mov     ax, 5                            ; 66b80500
        out     dx, ax                           ; 66ef
        pop     dx                               ; 665a
        pop     ax                               ; 6658
        ret                                      ; c3
