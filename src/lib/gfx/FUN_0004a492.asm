; FUN_0004a492 @ 0004a492  (226 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004a492.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004a492:
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x4a559                          ; 0f84b4000000
        mov     dx, 0x3c4                        ; 66bac403
        mov     esi, dword ptr [0x5368]          ; 8b3568530000   0x5368=g_screen_buf
        mov     edi, dword ptr [0x536c]          ; 8b3d6c530000
        mov     ax, 0x102                        ; 66b80201
        out     dx, ax                           ; 66ef
        mov     ebx, 0x190                       ; bb90010000
        mov     ecx, 4                           ; b904000000
        rep movsd dword ptr es:[edi], dword ptr [esi] ; f3a5
        add     esi, 0x40                        ; 83c640
        add     edi, 0x40                        ; 83c740
        dec     ebx                              ; 4b
        jne     0x4a4c0                          ; 75f0
        mov     esi, dword ptr [0x5368]          ; 8b3568530000   0x5368=g_screen_buf
        add     esi, 0x7d00                      ; 81c6007d0000
        mov     edi, dword ptr [0x536c]          ; 8b3d6c530000
        mov     ax, 0x202                        ; 66b80202
        out     dx, ax                           ; 66ef
        mov     ebx, 0x190                       ; bb90010000
        mov     ecx, 4                           ; b904000000
        rep movsd dword ptr es:[edi], dword ptr [esi] ; f3a5
        add     esi, 0x40                        ; 83c640
        add     edi, 0x40                        ; 83c740
        dec     ebx                              ; 4b
        jne     0x4a4ed                          ; 75f0
        mov     esi, dword ptr [0x5368]          ; 8b3568530000   0x5368=g_screen_buf
        add     esi, 0xfa00                      ; 81c600fa0000
        mov     edi, dword ptr [0x536c]          ; 8b3d6c530000
        mov     ax, 0x402                        ; 66b80204
        out     dx, ax                           ; 66ef
        mov     ebx, 0x190                       ; bb90010000
        mov     ecx, 4                           ; b904000000
        rep movsd dword ptr es:[edi], dword ptr [esi] ; f3a5
        add     esi, 0x40                        ; 83c640
        add     edi, 0x40                        ; 83c740
        dec     ebx                              ; 4b
        jne     0x4a51a                          ; 75f0
        mov     esi, dword ptr [0x5368]          ; 8b3568530000   0x5368=g_screen_buf
        mov     edi, dword ptr [0x536c]          ; 8b3d6c530000
        add     esi, 0x17700                     ; 81c600770100
        mov     ax, 0x802                        ; 66b80208
        out     dx, ax                           ; 66ef
        mov     ebx, 0x190                       ; bb90010000
        mov     ecx, 4                           ; b904000000
        rep movsd dword ptr es:[edi], dword ptr [esi] ; f3a5
        add     esi, 0x40                        ; 83c640
        add     edi, 0x40                        ; 83c740
        dec     ebx                              ; 4b
        jne     0x4a547                          ; 75f0
        jmp     0x4a56d                          ; eb14
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x4a564                          ; 7402
        jmp     0x4a56d                          ; eb09
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x4a56d                          ; 7400
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        ret                                      ; c3
