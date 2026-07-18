; FUN_0004a574 @ 0004a574  (52 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004a574.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004a574:
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x4a589                          ; 7407
        call    0x4a5a8                          ; e821000000     -> FUN_0004a5a8
        jmp     0x4a5a2                          ; eb19
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x4a599                          ; 7407
        call    0x4a639                          ; e8a2000000     -> FUN_0004a639
        jmp     0x4a5a2                          ; eb09
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x4a5a2                          ; 7400
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        ret                                      ; c3
