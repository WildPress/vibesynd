; FUN_00039e42 @ 00039e42  (160 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00039e42.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00039e42:
        push    2                                ; 6a02
        push    0xbde0                           ; 68e0bd0000
        push    dword ptr [0xbdd0]               ; ff35d0bd0000
        call    0x3a7c4                          ; e870090000     -> FUN_0003a7c4
        add     esp, 0xc                         ; 83c40c
        push    8                                ; 6a08
        push    dword ptr [0x5370]               ; ff3570530000   0x5370=g_back_buf
        push    dword ptr [0xbdd0]               ; ff35d0bd0000
        call    0x3a7c4                          ; e85a090000     -> FUN_0003a7c4
        add     esp, 0xc                         ; 83c40c
        mov     ax, word ptr [0xbde0]            ; 66a1e0bd0000
        cmp     ax, 0                            ; 6683f800
        jne     0x39e7b                          ; 7502
        jmp     0x39ee1                          ; eb66
        sub     word ptr [0xbde0], 1             ; 66832de0bd000001
        push    4                                ; 6a04
        push    0xbddc                           ; 68dcbd0000
        push    dword ptr [0xbdd0]               ; ff35d0bd0000
        call    0x3a7c4                          ; e82f090000     -> FUN_0003a7c4
        add     esp, 0xc                         ; 83c40c
        push    2                                ; 6a02
        push    0xbdd4                           ; 68d4bd0000
        push    dword ptr [0xbdd0]               ; ff35d0bd0000
        call    0x3a7c4                          ; e81a090000     -> FUN_0003a7c4
        add     esp, 0xc                         ; 83c40c
        mov     ax, word ptr [0xbdd4]            ; 66a1d4bd0000
        cmp     ax, 7                            ; 6683f807
        jne     0x39ec0                          ; 7507
        call    0x3a033                          ; e875010000     -> FUN_0003a033
        jmp     0x39e6d                          ; ebad
        cmp     ax, 4                            ; 6683f804
        jne     0x39ecd                          ; 7507
        call    0x39f69                          ; e89e000000     -> FUN_00039f69
        jmp     0x39e6d                          ; eba0
        cmp     ax, 0xf                          ; 6683f80f
        jne     0x39eda                          ; 7507
        call    0x3a10c                          ; e834020000     -> FUN_0003a10c
        jmp     0x39e6d                          ; eb93
        call    0x39f49                          ; e86a000000     -> FUN_00039f49
        jmp     0x39e6d                          ; eb8c
        ret                                      ; c3
