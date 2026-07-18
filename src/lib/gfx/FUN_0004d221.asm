; FUN_0004d221 @ 0004d221  (305 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004d221.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004d221:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ebx                              ; 53
        push    edx                              ; 52
        xor     eax, eax                         ; 33c0
        mov     ax, word ptr [ebp + 8]           ; 668b4508
        or      ax, ax                           ; 660bc0
        jne     0x4d241                          ; 7510
        mov     bx, word ptr [ebp + 0xc]         ; 668b5d0c
        or      bx, bx                           ; 660bdb
        jne     0x4d245                          ; 750b
        xor     ax, ax                           ; 6633c0
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
        mov     bx, word ptr [ebp + 0xc]         ; 668b5d0c
        neg     bx                               ; 66f7db
        xor     dh, dh                           ; 32f6
        or      ax, ax                           ; 660bc0
        js      0x4d2cb                          ; 787c
        or      bx, bx                           ; 660bdb
        js      0x4d290                          ; 783c
        cmp     ax, bx                           ; 663bc3
        jl      0x4d274                          ; 7c1b
        xchg    bx, ax                           ; 6693
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        add     ax, 0x40                         ; 6683c040
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        neg     ax                               ; 66f7d8
        add     ax, 0x80                         ; 66058000
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
        neg     bx                               ; 66f7db
        cmp     ax, bx                           ; 663bc3
        jl      0x4d2b6                          ; 7c1e
        xchg    bx, ax                           ; 6693
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        neg     ax                               ; 66f7d8
        add     ax, 0x40                         ; 6683c040
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
        neg     ax                               ; 66f7d8
        or      bx, bx                           ; 660bdb
        js      0x4d30f                          ; 783c
        cmp     ax, bx                           ; 663bc3
        jl      0x4d2f6                          ; 7c1e
        xchg    bx, ax                           ; 6693
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        neg     ax                               ; 66f7d8
        add     ax, 0xc0                         ; 6605c000
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        add     ax, 0x80                         ; 66058000
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
        neg     bx                               ; 66f7db
        cmp     ax, bx                           ; 663bc3
        jl      0x4d332                          ; 7c1b
        xchg    bx, ax                           ; 6693
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        add     ax, 0xc0                         ; 6605c000
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        neg     ax                               ; 66f7d8
        add     ax, 0x100                        ; 66050001
        and     ax, 0xff                         ; 6625ff00
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
