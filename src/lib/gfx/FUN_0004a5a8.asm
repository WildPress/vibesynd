; FUN_0004a5a8 @ 0004a5a8  (145 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004a5a8.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004a5a8:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        add     esp, -8                          ; 83c4f8
        cld                                      ; fc
        mov     dx, 0x3ce                        ; 66bace03
        mov     ax, 0xff08                       ; 66b808ff
        out     dx, ax                           ; 66ef
        mov     ax, 5                            ; 66b80500
        out     dx, ax                           ; 66ef
        mov     dl, 0xc4                         ; b2c4
        mov     esi, dword ptr [0x5368]          ; 8b3568530000   0x5368=g_screen_buf
        mov     edi, dword ptr [0x536c]          ; 8b3d6c530000
        mov     ebx, 0x32                        ; bb32000000
        mov     ax, 0x102                        ; 66b80201
        out     dx, ax                           ; 66ef
        mov     ecx, 0x140                       ; b940010000
        rep movsw word ptr es:[edi], word ptr [esi] ; f366a5
        sub     edi, 0x280                       ; 81ef80020000
        add     esi, 0x7a80                      ; 81c6807a0000
        mov     ax, 0x202                        ; 66b80202
        out     dx, ax                           ; 66ef
        mov     ecx, 0x140                       ; b940010000
        rep movsw word ptr es:[edi], word ptr [esi] ; f366a5
        sub     edi, 0x280                       ; 81ef80020000
        add     esi, 0x7a80                      ; 81c6807a0000
        mov     ax, 0x402                        ; 66b80204
        out     dx, ax                           ; 66ef
        mov     ecx, 0x140                       ; b940010000
        rep movsw word ptr es:[edi], word ptr [esi] ; f366a5
        sub     edi, 0x280                       ; 81ef80020000
        add     esi, 0x7a80                      ; 81c6807a0000
        mov     ax, 0x802                        ; 66b80208
        out     dx, ax                           ; 66ef
        mov     ecx, 0x140                       ; b940010000
        rep movsw word ptr es:[edi], word ptr [esi] ; f366a5
        sub     esi, 0x17700                     ; 81ee00770100
        dec     ebx                              ; 4b
        jne     0x4a5d2                          ; 759b
        leave                                    ; c9
        ret                                      ; c3
