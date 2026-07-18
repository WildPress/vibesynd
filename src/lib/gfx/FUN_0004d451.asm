; FUN_0004d451 @ 0004d451  (39 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004d451.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004d451:
        xor     ebx, ebx                         ; 33db
        mov     cx, 0xa                          ; 66b90a00
        xor     dl, dl                           ; 32d2
        sub     eax, dword ptr [ebx + 0x3fd30]   ; 2b8330fd0300
        jb      0x4d465                          ; 7204
        inc     dl                               ; fec2
        jmp     0x4d459                          ; ebf4
        add     eax, dword ptr [ebx + 0x3fd30]   ; 038330fd0300
        add     dl, 0x30                         ; 80c230
        mov     byte ptr [edi], dl               ; 8817
        inc     edi                              ; 47
        add     ebx, 4                           ; 83c304
        loop    0x4d457                          ; 67e2e0
        ret                                      ; c3
