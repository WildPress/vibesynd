; FUN_00049edf @ 00049edf  (989 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00049edf.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00049edf:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x4a2a0                          ; 0f84ab030000
        imul    eax, dword ptr [ebp + 0xc], 0x500 ; 69450c00050000
        add     eax, 0x10                        ; 83c010
        mov     esi, dword ptr [0x5368]          ; 8b3568530000   0x5368=g_screen_buf
        mov     edi, dword ptr [0x536c]          ; 8b3d6c530000
        add     esi, eax                         ; 03f0
        add     edi, eax                         ; 03f8
        mov     dx, 0x3c4                        ; 66bac403
        mov     ax, 0x102                        ; 66b80201
        out     dx, ax                           ; 66ef
        mov     ecx, 0x10                        ; b910000000
        mov     ebx, dword ptr [ebp + 8]         ; 8b5d08
        cmp     byte ptr [ebx], 0                ; 803b00
        jle     0x49fdc                          ; 0f8eb2000000
        mov     eax, dword ptr [esi]             ; 8b06
        mov     dword ptr [edi], eax             ; 8907
        mov     eax, dword ptr [esi + 0x50]      ; 8b4650
        mov     dword ptr [edi + 0x50], eax      ; 894750
        mov     eax, dword ptr [esi + 0xa0]      ; 8b86a0000000
        mov     dword ptr [edi + 0xa0], eax      ; 8987a0000000
        mov     eax, dword ptr [esi + 0xf0]      ; 8b86f0000000
        mov     dword ptr [edi + 0xf0], eax      ; 8987f0000000
        mov     eax, dword ptr [esi + 0x140]     ; 8b8640010000
        mov     dword ptr [edi + 0x140], eax     ; 898740010000
        mov     eax, dword ptr [esi + 0x190]     ; 8b8690010000
        mov     dword ptr [edi + 0x190], eax     ; 898790010000
        mov     eax, dword ptr [esi + 0x1e0]     ; 8b86e0010000
        mov     dword ptr [edi + 0x1e0], eax     ; 8987e0010000
        mov     eax, dword ptr [esi + 0x230]     ; 8b8630020000
        mov     dword ptr [edi + 0x230], eax     ; 898730020000
        mov     eax, dword ptr [esi + 0x280]     ; 8b8680020000
        mov     dword ptr [edi + 0x280], eax     ; 898780020000
        mov     eax, dword ptr [esi + 0x2d0]     ; 8b86d0020000
        mov     dword ptr [edi + 0x2d0], eax     ; 8987d0020000
        mov     eax, dword ptr [esi + 0x320]     ; 8b8620030000
        mov     dword ptr [edi + 0x320], eax     ; 898720030000
        mov     eax, dword ptr [esi + 0x370]     ; 8b8670030000
        mov     dword ptr [edi + 0x370], eax     ; 898770030000
        mov     eax, dword ptr [esi + 0x3c0]     ; 8b86c0030000
        mov     dword ptr [edi + 0x3c0], eax     ; 8987c0030000
        mov     eax, dword ptr [esi + 0x410]     ; 8b8610040000
        mov     dword ptr [edi + 0x410], eax     ; 898710040000
        mov     eax, dword ptr [esi + 0x460]     ; 8b8660040000
        mov     dword ptr [edi + 0x460], eax     ; 898760040000
        mov     eax, dword ptr [esi + 0x4b0]     ; 8b86b0040000
        mov     dword ptr [edi + 0x4b0], eax     ; 8987b0040000
        add     esi, 4                           ; 83c604
        add     edi, 4                           ; 83c704
        inc     ebx                              ; 43
        dec     ecx                              ; 49
        jne     0x49f21                          ; 0f8537ffffff
        sub     edi, 0x40                        ; 83ef40
        sub     esi, 0x40                        ; 83ee40
        mov     ax, 0x202                        ; 66b80202
        out     dx, ax                           ; 66ef
        mov     ecx, 0x10                        ; b910000000
        mov     ebx, dword ptr [ebp + 8]         ; 8b5d08
        cmp     byte ptr [ebx], 0                ; 803b00
        jle     0x4a0c0                          ; 0f8eb9000000
        mov     eax, dword ptr [esi + 0x7d00]    ; 8b86007d0000
        mov     dword ptr [edi], eax             ; 8907
        mov     eax, dword ptr [esi + 0x7d50]    ; 8b86507d0000
        mov     dword ptr [edi + 0x50], eax      ; 894750
        mov     eax, dword ptr [esi + 0x7da0]    ; 8b86a07d0000
        mov     dword ptr [edi + 0xa0], eax      ; 8987a0000000
        mov     eax, dword ptr [esi + 0x7df0]    ; 8b86f07d0000
        mov     dword ptr [edi + 0xf0], eax      ; 8987f0000000
        mov     eax, dword ptr [esi + 0x7e40]    ; 8b86407e0000
        mov     dword ptr [edi + 0x140], eax     ; 898740010000
        mov     eax, dword ptr [esi + 0x7e90]    ; 8b86907e0000
        mov     dword ptr [edi + 0x190], eax     ; 898790010000
        mov     eax, dword ptr [esi + 0x7ee0]    ; 8b86e07e0000
        mov     dword ptr [edi + 0x1e0], eax     ; 8987e0010000
        mov     eax, dword ptr [esi + 0x7f30]    ; 8b86307f0000
        mov     dword ptr [edi + 0x230], eax     ; 898730020000
        mov     eax, dword ptr [esi + 0x7f80]    ; 8b86807f0000
        mov     dword ptr [edi + 0x280], eax     ; 898780020000
        mov     eax, dword ptr [esi + 0x7fd0]    ; 8b86d07f0000
        mov     dword ptr [edi + 0x2d0], eax     ; 8987d0020000
        mov     eax, dword ptr [esi + 0x8020]    ; 8b8620800000
        mov     dword ptr [edi + 0x320], eax     ; 898720030000
        mov     eax, dword ptr [esi + 0x8070]    ; 8b8670800000
        mov     dword ptr [edi + 0x370], eax     ; 898770030000
        mov     eax, dword ptr [esi + 0x80c0]    ; 8b86c0800000
        mov     dword ptr [edi + 0x3c0], eax     ; 8987c0030000
        mov     eax, dword ptr [esi + 0x8110]    ; 8b8610810000
        mov     dword ptr [edi + 0x410], eax     ; 898710040000
        mov     eax, dword ptr [esi + 0x8160]    ; 8b8660810000
        mov     dword ptr [edi + 0x460], eax     ; 898760040000
        mov     eax, dword ptr [esi + 0x81b0]    ; 8b86b0810000
        mov     dword ptr [edi + 0x4b0], eax     ; 8987b0040000
        add     esi, 4                           ; 83c604
        add     edi, 4                           ; 83c704
        inc     ebx                              ; 43
        dec     ecx                              ; 49
        jne     0x49ffe                          ; 0f8530ffffff
        sub     edi, 0x40                        ; 83ef40
        sub     esi, 0x40                        ; 83ee40
        mov     ax, 0x402                        ; 66b80204
        out     dx, ax                           ; 66ef
        mov     ecx, 0x10                        ; b910000000
        mov     ebx, dword ptr [ebp + 8]         ; 8b5d08
        cmp     byte ptr [ebx], 0                ; 803b00
        jle     0x4a1a4                          ; 0f8eb9000000
        mov     eax, dword ptr [esi + 0xfa00]    ; 8b8600fa0000
        mov     dword ptr [edi], eax             ; 8907
        mov     eax, dword ptr [esi + 0xfa50]    ; 8b8650fa0000
        mov     dword ptr [edi + 0x50], eax      ; 894750
        mov     eax, dword ptr [esi + 0xfaa0]    ; 8b86a0fa0000
        mov     dword ptr [edi + 0xa0], eax      ; 8987a0000000
        mov     eax, dword ptr [esi + 0xfaf0]    ; 8b86f0fa0000
        mov     dword ptr [edi + 0xf0], eax      ; 8987f0000000
        mov     eax, dword ptr [esi + 0xfb40]    ; 8b8640fb0000
        mov     dword ptr [edi + 0x140], eax     ; 898740010000
        mov     eax, dword ptr [esi + 0xfb90]    ; 8b8690fb0000
        mov     dword ptr [edi + 0x190], eax     ; 898790010000
        mov     eax, dword ptr [esi + 0xfbe0]    ; 8b86e0fb0000
        mov     dword ptr [edi + 0x1e0], eax     ; 8987e0010000
        mov     eax, dword ptr [esi + 0xfc30]    ; 8b8630fc0000
        mov     dword ptr [edi + 0x230], eax     ; 898730020000
        mov     eax, dword ptr [esi + 0xfc80]    ; 8b8680fc0000
        mov     dword ptr [edi + 0x280], eax     ; 898780020000
        mov     eax, dword ptr [esi + 0xfcd0]    ; 8b86d0fc0000
        mov     dword ptr [edi + 0x2d0], eax     ; 8987d0020000
        mov     eax, dword ptr [esi + 0xfd20]    ; 8b8620fd0000
        mov     dword ptr [edi + 0x320], eax     ; 898720030000
        mov     eax, dword ptr [esi + 0xfd70]    ; 8b8670fd0000
        mov     dword ptr [edi + 0x370], eax     ; 898770030000
        mov     eax, dword ptr [esi + 0xfdc0]    ; 8b86c0fd0000
        mov     dword ptr [edi + 0x3c0], eax     ; 8987c0030000
        mov     eax, dword ptr [esi + 0xfe10]    ; 8b8610fe0000
        mov     dword ptr [edi + 0x410], eax     ; 898710040000
        mov     eax, dword ptr [esi + 0xfe60]    ; 8b8660fe0000
        mov     dword ptr [edi + 0x460], eax     ; 898760040000
        mov     eax, dword ptr [esi + 0xfeb0]    ; 8b86b0fe0000
        mov     dword ptr [edi + 0x4b0], eax     ; 8987b0040000
        add     esi, 4                           ; 83c604
        add     edi, 4                           ; 83c704
        inc     ebx                              ; 43
        dec     ecx                              ; 49
        jne     0x4a0e2                          ; 0f8530ffffff
        sub     edi, 0x40                        ; 83ef40
        sub     esi, 0x40                        ; 83ee40
        mov     ax, 0x802                        ; 66b80208
        out     dx, ax                           ; 66ef
        mov     ecx, 0x10                        ; b910000000
        mov     ebx, dword ptr [ebp + 8]         ; 8b5d08
        cmp     byte ptr [ebx], 0                ; 803b00
        jle     0x4a28a                          ; 0f8ebb000000
        dec     byte ptr [ebx]                   ; fe0b
        mov     eax, dword ptr [esi + 0x17700]   ; 8b8600770100
        mov     dword ptr [edi], eax             ; 8907
        mov     eax, dword ptr [esi + 0x17750]   ; 8b8650770100
        mov     dword ptr [edi + 0x50], eax      ; 894750
        mov     eax, dword ptr [esi + 0x177a0]   ; 8b86a0770100
        mov     dword ptr [edi + 0xa0], eax      ; 8987a0000000
        mov     eax, dword ptr [esi + 0x177f0]   ; 8b86f0770100
        mov     dword ptr [edi + 0xf0], eax      ; 8987f0000000
        mov     eax, dword ptr [esi + 0x17840]   ; 8b8640780100
        mov     dword ptr [edi + 0x140], eax     ; 898740010000
        mov     eax, dword ptr [esi + 0x17890]   ; 8b8690780100
        mov     dword ptr [edi + 0x190], eax     ; 898790010000
        mov     eax, dword ptr [esi + 0x178e0]   ; 8b86e0780100
        mov     dword ptr [edi + 0x1e0], eax     ; 8987e0010000
        mov     eax, dword ptr [esi + 0x17930]   ; 8b8630790100
        mov     dword ptr [edi + 0x230], eax     ; 898730020000
        mov     eax, dword ptr [esi + 0x17980]   ; 8b8680790100
        mov     dword ptr [edi + 0x280], eax     ; 898780020000
        mov     eax, dword ptr [esi + 0x179d0]   ; 8b86d0790100
        mov     dword ptr [edi + 0x2d0], eax     ; 8987d0020000
        mov     eax, dword ptr [esi + 0x17a20]   ; 8b86207a0100
        mov     dword ptr [edi + 0x320], eax     ; 898720030000
        mov     eax, dword ptr [esi + 0x17a70]   ; 8b86707a0100
        mov     dword ptr [edi + 0x370], eax     ; 898770030000
        mov     eax, dword ptr [esi + 0x17ac0]   ; 8b86c07a0100
        mov     dword ptr [edi + 0x3c0], eax     ; 8987c0030000
        mov     eax, dword ptr [esi + 0x17b10]   ; 8b86107b0100
        mov     dword ptr [edi + 0x410], eax     ; 898710040000
        mov     eax, dword ptr [esi + 0x17b60]   ; 8b86607b0100
        mov     dword ptr [edi + 0x460], eax     ; 898760040000
        mov     eax, dword ptr [esi + 0x17bb0]   ; 8b86b07b0100
        mov     dword ptr [edi + 0x4b0], eax     ; 8987b0040000
        add     esi, 4                           ; 83c604
        add     edi, 4                           ; 83c704
        inc     ebx                              ; 43
        dec     ecx                              ; 49
        jne     0x4a1c6                          ; 0f852effffff
        sub     edi, 0x40                        ; 83ef40
        sub     esi, 0x40                        ; 83ee40
        jmp     0x4a2b4                          ; eb14
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x4a2ab                          ; 7402
        jmp     0x4a2b4                          ; eb09
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x4a2b4                          ; 7400
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        leave                                    ; c9
        ret                                      ; c3
