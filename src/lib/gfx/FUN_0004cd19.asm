; FUN_0004cd19 @ 0004cd19  (37 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004cd19.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004cd19:
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x4cd29                          ; 7407
        call    0x4cd3e                          ; e817000000     -> FUN_0004cd3e
        jmp     0x4cd3d                          ; eb14
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x4cd34                          ; 7402
        jmp     0x4cd3d                          ; eb09
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x4cd3d                          ; 7400
        ret                                      ; c3
