; FUN_0004a734 @ 0004a734  (356 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004a734.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004a734:
        sar     ebx, 1                           ; d1fb
        sar     ecx, 1                           ; d1f9
        shr     dl, 1                            ; d0ea
        shr     dh, 1                            ; d0ee
        cmp     cx, 0xc8                         ; 6681f9c800
        jge     0x4a897                          ; 0f8d50010000
        cmp     cx, 0                            ; 6683f900
        jl      0x4a86b                          ; 0f8c1a010000
        mov     al, dh                           ; 8ac6
        cbw                                      ; 6698
        add     ax, cx                           ; 6603c1
        cmp     ax, 0xc8                         ; 663dc800
        jge     0x4a889                          ; 0f8d27010000
        cmp     bx, 0x140                        ; 6681fb4001
        jge     0x4a897                          ; 0f8d2a010000
        cmp     bx, 0                            ; 6683fb00
        jl      0x4a7bf                          ; 7c4c
        mov     al, dl                           ; 8ac2
        cbw                                      ; 6698
        add     ax, bx                           ; 6603c3
        cmp     ax, 0x140                        ; 663d4001
        jge     0x4a81d                          ; 0f8d99000000
        add     edi, ebx                         ; 03fb
        movsx   eax, cx                          ; 0fbfc1
        mov     bh, dh                           ; 8afe
        imul    eax, eax, 0x140                  ; 69c040010000
        add     edi, eax                         ; 03f8
        movzx   ecx, cl                          ; 0fb6c9
        mov     eax, 0xffffffff                  ; b8ffffffff
        mov     ebp, edi                         ; 8bef
        lodsb   al, byte ptr [esi]               ; ac
        cmp     al, ch                           ; 3ac5
        je      0x4a7ae                          ; 740c
        js      0x4a7aa                          ; 7806
        mov     cl, al                           ; 8ac8
        rep movsb byte ptr es:[edi], byte ptr [esi] ; f3a4
        jmp     0x4a79d                          ; ebf3
        sub     edi, eax                         ; 2bf8
        jmp     0x4a79d                          ; ebef
        add     ebp, 0x140                       ; 81c540010000
        mov     edi, ebp                         ; 8bfd
        dec     bh                               ; fecf
        jne     0x4a79d                          ; 75e3
        jmp     0x4a897                          ; e9d8000000
        movzx   eax, dl                          ; 0fb6c2
        add     eax, ebx                         ; 03c3
        js      0x4a897                          ; 0f88cd000000
        mov     eax, ecx                         ; 8bc1
        mov     ch, dh                           ; 8aee
        imul    eax, eax, 0x140                  ; 69c040010000
        add     edi, eax                         ; 03f8
        mov     ebp, edi                         ; 8bef
        mov     cl, bl                           ; 8acb
        movzx   ebx, bl                          ; 0fb6db
        lodsb   al, byte ptr [esi]               ; ac
        cmp     al, 0                            ; 3c00
        je      0x4a80d                          ; 742b
        js      0x4a7f2                          ; 780e
        mov     ah, al                           ; 8ae0
        lodsb   al, byte ptr [esi]               ; ac
        inc     bl                               ; fec3
        jle     0x4a7ec                          ; 7e01
        stosb   byte ptr es:[edi], al            ; aa
        dec     ah                               ; fecc
        jg      0x4a7e6                          ; 7ff6
        jmp     0x4a7dd                          ; ebeb
        mov     bh, bl                           ; 8afb
        sub     bl, al                           ; 2ad8
        jle     0x4a7dd                          ; 7ee5
        cmp     bh, 0                            ; 80ff00
        jge     0x4a804                          ; 7d07
        xor     bh, bh                           ; 32ff
        add     di, bx                           ; 6603fb
        jmp     0x4a809                          ; eb05
        movsx   eax, al                          ; 0fbec0
        sub     edi, eax                         ; 2bf8
        xor     bh, bh                           ; 32ff
        jmp     0x4a7dd                          ; ebd0
        add     ebp, 0x140                       ; 81c540010000
        mov     edi, ebp                         ; 8bfd
        mov     bl, cl                           ; 8ad9
        dec     ch                               ; fecd
        jne     0x4a7dd                          ; 75c2
        jmp     0x4a897                          ; eb7a
        add     edi, ebx                         ; 03fb
        neg     ebx                              ; f7db
        add     ebx, 0x140                       ; 81c340010000
        mov     eax, ecx                         ; 8bc1
        mov     ecx, edx                         ; 8bca
        mov     cl, bl                           ; 8acb
        xor     edx, edx                         ; 33d2
        imul    eax, eax, 0x140                  ; 69c040010000
        add     edi, eax                         ; 03f8
        mov     ebp, edi                         ; 8bef
        lodsb   al, byte ptr [esi]               ; ac
        cmp     al, 0                            ; 3c00
        je      0x4a85b                          ; 741d
        js      0x4a84e                          ; 780e
        mov     ah, al                           ; 8ae0
        lodsb   al, byte ptr [esi]               ; ac
        dec     bl                               ; fecb
        js      0x4a848                          ; 7801
        stosb   byte ptr es:[edi], al            ; aa
        dec     ah                               ; fecc
        jg      0x4a842                          ; 7ff6
        jmp     0x4a839                          ; ebeb
        add     bl, al                           ; 02d8
        jle     0x4a839                          ; 7ee7
        cbw                                      ; 6698
        movsx   eax, ax                          ; 0fbfc0
        sub     edi, eax                         ; 2bf8
        jmp     0x4a839                          ; ebde
        add     ebp, 0x140                       ; 81c540010000
        mov     edi, ebp                         ; 8bfd
        mov     bl, cl                           ; 8ad9
        dec     ch                               ; fecd
        jne     0x4a839                          ; 75d0
        jmp     0x4a897                          ; eb2c
        movzx   eax, dh                          ; 0fb6c6
        add     eax, ecx                         ; 03c1
        jle     0x4a897                          ; 7e25
        mov     dh, al                           ; 8af0
        xor     ah, ah                           ; 32e4
        lodsb   al, byte ptr [esi]               ; ac
        cmp     al, 0                            ; 3c00
        je      0x4a881                          ; 7406
        js      0x4a876                          ; 78f9
        add     esi, eax                         ; 03f0
        jmp     0x4a876                          ; ebf5
        inc     ecx                              ; 41
        jne     0x4a876                          ; 75f2
        jmp     0x4a762                          ; e9d9feffff
        mov     eax, 0xc8                        ; b8c8000000
        sub     eax, ecx                         ; 2bc1
        mov     dh, al                           ; 8af0
        jmp     0x4a762                          ; e9cbfeffff
        ret                                      ; c3
