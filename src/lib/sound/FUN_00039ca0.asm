; FUN_00039ca0 @ 00039ca0  (418 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00039ca0.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00039ca0:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    edi                              ; 57
        push    esi                              ; 56
        mov     word ptr [0xbdf2], 1             ; 66c705f2bd00000100
        mov     byte ptr [0x537e], 0x80          ; c6057e53000080
        mov     word ptr [0x5398], 0             ; 66c705985300000000
        mov     eax, dword ptr [ebp + 0xc]       ; 8b450c
        mov     dword ptr [0xbdf4], eax          ; a3f4bd0000
        push    0x40                             ; 6a40
        push    0x200                            ; 6800020000
        push    0xbdfc                           ; 68fcbd0000
        call    0x3a598                          ; e8bd080000     -> FUN_0003a598
        add     esp, 0xc                         ; 83c40c
        cmp     eax, 0                           ; 83f800
        jle     0x39e3a                          ; 0f8e53010000
        mov     dword ptr [0xbdd0], eax          ; a3d0bd0000
        mov     word ptr [0xbde4], 0             ; 66c705e4bd00000000
        mov     word ptr [0xbdee], 0             ; 66c705eebd00000000
        mov     ah, 0x2c                         ; b42c
        int     0x21                             ; cd21
        mov     ax, 0x64                         ; 66b86400
        mul     dh                               ; f6e6
        mov     dh, 0                            ; b600
        add     dx, ax                           ; 6603d0
        mov     word ptr [0xbdec], dx            ; 668915ecbd0000
        mov     eax, 4                           ; b804000000
        push    eax                              ; 50
        push    0xbddc                           ; 68dcbd0000
        push    dword ptr [0xbdd0]               ; ff35d0bd0000
        call    0x3a7c4                          ; e89a0a0000     -> FUN_0003a7c4
        add     esp, 0xc                         ; 83c40c
        push    2                                ; 6a02
        push    0xbdd4                           ; 68d4bd0000
        push    dword ptr [0xbdd0]               ; ff35d0bd0000
        call    0x3a7c4                          ; e8850a0000     -> FUN_0003a7c4
        add     esp, 0xc                         ; 83c40c
        mov     ax, word ptr [0xbdd4]            ; 66a1d4bd0000
        cmp     ax, 0xaf12                       ; 663d12af
        jne     0x39d55                          ; 7507
        call    0x39ee2                          ; e88f010000     -> FUN_00039ee2
        jmp     0x39d14                          ; ebbf
        cmp     ax, 0xf1fa                       ; 663dfaf1
        jne     0x39e0d                          ; 0f85ae000000
        mov     ax, word ptr [0xbdd6]            ; 66a1d6bd0000
        sub     ax, 1                            ; 6683e801
        mov     word ptr [0xbdd6], ax            ; 66a3d6bd0000
        mov     ax, word ptr [0xbde4]            ; 66a1e4bd0000
        add     ax, 1                            ; 6683c001
        mov     word ptr [0xbde4], ax            ; 66a3e4bd0000
        push    dword ptr [ebp + 0x10]           ; ff7510
        call    0x391a8                          ; e821f4ffff     -> FUN_000391a8
        add     esp, 4                           ; 83c404
        call    0x39e42                          ; e8b3000000     -> FUN_00039e42
        cmp     dword ptr [0xbdf8], 0            ; 833df8bd000000
        je      0x39dbf                          ; 7427
        cmp     word ptr [0xbe2e], 1             ; 66833d2ebe000001
        jne     0x39da7                          ; 7505
        call    0x39f92                          ; e8eb010000     -> FUN_00039f92
        push    ecx                              ; 51
        push    edi                              ; 57
        push    esi                              ; 56
        mov     edi, 0xa0000                     ; bf00000a00
        mov     esi, dword ptr [0x5368]          ; 8b3568530000   0x5368=g_screen_buf
        mov     ecx, 0x3e80                      ; b9803e0000
        rep movsd dword ptr es:[edi], dword ptr [esi] ; f3a5
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     ecx                              ; 59
        call    0x3a19a                          ; e8d6030000     -> FUN_0003a19a
        mov     ax, word ptr [0xbdd6]            ; 66a1d6bd0000
        cmp     ax, 0                            ; 6683f800
        jne     0x39dd2                          ; 7502
        jmp     0x39e0d                          ; eb3b
        mov     bl, 0x36                         ; b336
        mov     ax, 0x1200                       ; 66b80012
        int     0x10                             ; cd10
        cmp     dword ptr [ebp + 8], 0           ; 837d0800
        je      0x39dfd                          ; 741d
        cmp     byte ptr [0x10b3f], 0            ; 803d3f0b010000
        jne     0x39df4                          ; 750b
        cmp     byte ptr [0x537e], 0x80          ; 803d7e53000080
        jg      0x39df4                          ; 7f02
        jmp     0x39dfd                          ; eb09
        mov     word ptr [0xbdf2], 0             ; 66c705f2bd00000000
        cmp     word ptr [0xbdf2], 0             ; 66813df2bd00000000
        je      0x39e0d                          ; 7405
        jmp     0x39d14                          ; e907ffffff
        push    dword ptr [0xbdd0]               ; ff35d0bd0000
        call    0x3a89d                          ; e8850a0000     -> FUN_0003a89d
        add     esp, 4                           ; 83c404
        mov     bx, 0x1f4                        ; 66bbf401
        cmp     word ptr [0xbdf2], 0             ; 66833df2bd000000
        je      0x39e3a                          ; 7411
        dec     bx                               ; 664b
        jne     0x39e1f                          ; 75f2
        dec     word ptr [0xbdf2]                ; 66ff0df2bd0000
        jne     0x39cca                          ; 0f8590feffff
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        leave                                    ; c9
        ret                                      ; c3
