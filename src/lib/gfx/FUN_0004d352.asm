; FUN_0004d352 @ 0004d352  (65 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004d352.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004d352:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    bx                               ; 6653
        push    cx                               ; 6651
        push    dx                               ; 6652
        xor     eax, eax                         ; 33c0
        mov     cx, word ptr [ebp + 8]           ; 668b4d08
        or      cx, cx                           ; 660bc9
        je      0x4d38b                          ; 7425
        bsr     ax, cx                           ; 660fbdc1
        mov     bx, word ptr [eax*2 + 0x3fc81]   ; 668b1c4581fc0300
        mov     ax, cx                           ; 668bc1
        xor     dx, dx                           ; 6633d2
        div     bx                               ; 66f7f3
        cmp     ax, bx                           ; 663bc3
        jge     0x4d388                          ; 7d08
        add     bx, ax                           ; 6603d8
        shr     bx, 1                            ; 66d1eb
        jmp     0x4d372                          ; ebea
        mov     ax, bx                           ; 668bc3
        pop     dx                               ; 665a
        pop     cx                               ; 6659
        pop     bx                               ; 665b
        leave                                    ; c9
        ret                                      ; c3
