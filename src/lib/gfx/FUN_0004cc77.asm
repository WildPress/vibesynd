; FUN_0004cc77 @ 0004cc77  (161 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004cc77.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004cc77:
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
        mov     edx, 0                           ; ba00000000
        mov     dx, word ptr [edi]               ; 668b17
        add     edi, 2                           ; 83c702
        ror     edx, cl                          ; d3ca
        xchg    dh, dl                           ; 86d6
        rol     edx, 0x10                        ; c1c210
        xchg    dh, dl                           ; 86d6
        rol     edx, 0x10                        ; c1c210
        test    si, 1                            ; 66f7c60100
        je      0x4ccbe                          ; 7404
        or      dword ptr [ebx], edx             ; 0913
        jmp     0x4ccc4                          ; eb06
        not     edx                              ; f7d2
        and     dword ptr [ebx], edx             ; 2113
        not     edx                              ; f7d2
        test    si, 2                            ; 66f7c60200
        je      0x4ccd3                          ; 7408
        or      dword ptr [ebx + 0x7d00], edx    ; 0993007d0000
        jmp     0x4ccdd                          ; eb0a
        not     edx                              ; f7d2
        and     dword ptr [ebx + 0x7d00], edx    ; 2193007d0000
        not     edx                              ; f7d2
        test    si, 4                            ; 66f7c60400
        je      0x4ccec                          ; 7408
        or      dword ptr [ebx + 0xfa00], edx    ; 099300fa0000
        jmp     0x4ccf6                          ; eb0a
        not     edx                              ; f7d2
        and     dword ptr [ebx + 0xfa00], edx    ; 219300fa0000
        not     edx                              ; f7d2
        test    si, 8                            ; 66f7c60800
        je      0x4cd05                          ; 7408
        or      dword ptr [ebx + 0x17700], edx   ; 099300770100
        jmp     0x4cd0f                          ; eb0a
        not     edx                              ; f7d2
        and     dword ptr [ebx + 0x17700], edx   ; 219300770100
        not     edx                              ; f7d2
        add     ebx, 0x50                        ; 83c350
        dec     ax                               ; 6648
        jne     0x4cc9c                          ; 7586
        pop     esi                              ; 5e
        ret                                      ; c3
