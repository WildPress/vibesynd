; FUN_0004cd3e @ 0004cd3e  (164 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004cd3e.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004cd3e:
        push    esi                              ; 56
        mov     si, dx                           ; 668bf2
        imul    bx, bx, 0x50                     ; 666bdb50
        push    ax                               ; 6650
        sar     ax, 3                            ; 66c1f803
        add     bx, ax                           ; 6603d8
        and     ebx, 0xffff                      ; 81e3ffff0000
        add     ebx, dword ptr [0x5368]          ; 031d68530000   0x5368=g_screen_buf
        pop     ax                               ; 6658
        and     ax, 7                            ; 6683e007
        xchg    cx, ax                           ; 6691
        mov     dx, 0                            ; 66ba0000
        mov     dl, byte ptr [edi]               ; 8a17
        inc     edi                              ; 47
        ror     dx, cl                           ; 66d3ca
        test    si, 1                            ; 66f7c60100
        je      0x4cd79                          ; 7405
        or      word ptr [ebx], dx               ; 660913
        jmp     0x4cd82                          ; eb09
        not     dx                               ; 66f7d2
        and     word ptr [ebx], dx               ; 662113
        not     dx                               ; 66f7d2
        test    si, 2                            ; 66f7c60200
        je      0x4cd92                          ; 7409
        or      word ptr [ebx + 0x7d00], dx      ; 660993007d0000
        jmp     0x4cd9f                          ; eb0d
        not     dx                               ; 66f7d2
        and     word ptr [ebx + 0x7d00], dx      ; 662193007d0000
        not     dx                               ; 66f7d2
        test    si, 4                            ; 66f7c60400
        je      0x4cdaf                          ; 7409
        or      word ptr [ebx + 0xfa00], dx      ; 66099300fa0000
        jmp     0x4cdbc                          ; eb0d
        not     dx                               ; 66f7d2
        and     word ptr [ebx + 0xfa00], dx      ; 66219300fa0000
        not     dx                               ; 66f7d2
        test    si, 8                            ; 66f7c60800
        je      0x4cdcc                          ; 7409
        or      word ptr [ebx + 0x17700], dx     ; 66099300770100
        jmp     0x4cdd9                          ; eb0d
        not     dx                               ; 66f7d2
        and     word ptr [ebx + 0x17700], dx     ; 66219300770100
        not     dx                               ; 66f7d2
        add     ebx, 0x50                        ; 83c350
        dec     ax                               ; 6648
        jne     0x4cd63                          ; 7583
        pop     esi                              ; 5e
        ret                                      ; c3
