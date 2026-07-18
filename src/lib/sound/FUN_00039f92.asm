; FUN_00039f92 @ 00039f92  (161 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00039f92.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00039f92:
        push    esi                              ; 56
        mov     word ptr [0xbe2e], 0             ; 66c7052ebe00000000
        cmp     dword ptr [0xbdf4], 1            ; 833df4bd000001
        jne     0x3a031                          ; 0f8588000000
        mov     edi, dword ptr [0x10ab0]         ; 8b3db00a0100
        sub     bx, bx                           ; 662bdb
        mov     ax, word ptr [edi]               ; 668b07
        add     edi, 2                           ; 83c702
        mov     esi, dword ptr [0x10aa8]         ; 8b35a80a0100
        push    ax                               ; 6650
        mov     ecx, 0                           ; b900000000
        mov     cl, byte ptr [edi]               ; 8a0f
        inc     edi                              ; 47
        add     esi, ecx                         ; 03f1
        add     esi, ecx                         ; 03f1
        add     esi, ecx                         ; 03f1
        mov     dx, 0                            ; 66ba0000
        mov     dl, byte ptr [edi]               ; 8a17
        inc     edi                              ; 47
        cmp     dl, 0                            ; 80fa00
        jne     0x39fde                          ; 7504
        mov     dx, 0x100                        ; 66ba0001
        mov     al, byte ptr [edi]               ; 8a07
        inc     edi                              ; 47
        mov     byte ptr [esi], al               ; 8806
        inc     esi                              ; 46
        mov     al, byte ptr [edi]               ; 8a07
        inc     edi                              ; 47
        mov     byte ptr [esi], al               ; 8806
        inc     esi                              ; 46
        mov     al, byte ptr [edi]               ; 8a07
        inc     edi                              ; 47
        mov     byte ptr [esi], al               ; 8806
        inc     esi                              ; 46
        dec     dx                               ; 664a
        cmp     dx, 0                            ; 6683fa00
        jne     0x39fde                          ; 75e6
        pop     ax                               ; 6658
        dec     ax                               ; 6648
        cmp     ax, 0                            ; 6683f800
        jne     0x39fbe                          ; 75bc
        mov     esi, dword ptr [0x10aa8]         ; 8b35a80a0100
        mov     edi, esi                         ; 8bfe
        mov     cx, 0x300                        ; 66b90003
        mov     dx, 0x3c8                        ; 66bac803
        mov     ax, 0                            ; 66b80000
        out     dx, al                           ; ee
        cld                                      ; fc
        mov     bl, 0x36                         ; b336
        mov     ax, 0x1201                       ; 66b80112
        int     0x10                             ; cd10
        mov     dx, 0x3da                        ; 66bada03
        in      al, dx                           ; ec
        test    al, 8                            ; a808
        je      0x3a024                          ; 74fb
        mov     dx, 0x3c9                        ; 66bac903
        rep outsb dx, byte ptr [esi]             ; f36e
        mov     edi, esi                         ; 8bfe
        pop     esi                              ; 5e
        ret                                      ; c3
