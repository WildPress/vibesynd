; FUN_0004cc4d @ 0004cc4d  (42 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004cc4d.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004cc4d:
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x4cc5d                          ; 7407
        call    0x4cc77                          ; e81c000000     -> FUN_0004cc77
        jmp     0x4cc76                          ; eb19
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x4cc6d                          ; 7407
        call    0x4cd18                          ; e8ad000000     -> FUN_0004cd18
        jmp     0x4cc76                          ; eb09
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x4cc76                          ; 7400
        ret                                      ; c3
