; FUN_00039722 @ 00039722  (37 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00039722.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00039722:
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        mov     esi, 0xf                         ; be0f000000
        push    esi                              ; 56
        call    0x396d5                          ; e8a4ffffff     -> FUN_000396d5
        add     esp, 4                           ; 83c404
        dec     esi                              ; 4e
        jge     0x3972b                          ; 7df4
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x39742                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        ret                                      ; c3
