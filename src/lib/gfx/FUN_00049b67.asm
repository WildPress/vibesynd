; FUN_00049b67 @ 00049b67  (886 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00049b67.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00049b67:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x49ec3                          ; 0f8446030000
        mov     eax, dword ptr [ebp + 8]         ; 8b4508
        add     eax, dword ptr [0x10ab8]         ; 0305b80a0100   0x10ab8=g_marker_phase_b
        and     eax, 0xf                         ; 83e00f
        shl     eax, 2                           ; c1e002
        mov     ebx, dword ptr [ebp + 0xc]       ; 8b5d0c
        add     ebx, dword ptr [0x10ab4]         ; 031db40a0100   0x10ab4=g_marker_phase_a
        cmp     ebx, 0x19                        ; 83fb19
        jl      0x49b9d                          ; 7c03
        sub     ebx, 0x19                        ; 83eb19
        imul    ebx, ebx, 0x500                  ; 69db00050000
        add     eax, ebx                         ; 03c3
        mov     esi, dword ptr [0x5370]          ; 8b3570530000   0x5370=g_back_buf
        add     esi, eax                         ; 03f0
        mov     eax, dword ptr [ebp + 8]         ; 8b4508
        shl     eax, 2                           ; c1e002
        imul    ebx, dword ptr [ebp + 0xc], 0x500 ; 695d0c00050000
        add     eax, ebx                         ; 03c3
        add     eax, 0x10                        ; 83c010
        mov     edi, dword ptr [0x5368]          ; 8b3d68530000   0x5368=g_screen_buf
        add     edi, eax                         ; 03f8
        mov     eax, dword ptr [esi]             ; 8b06
        mov     dword ptr [edi], eax             ; 8907
        mov     eax, dword ptr [esi + 0x7d00]    ; 8b86007d0000
        mov     dword ptr [edi + 0x7d00], eax    ; 8987007d0000
        mov     eax, dword ptr [esi + 0xfa00]    ; 8b8600fa0000
        mov     dword ptr [edi + 0xfa00], eax    ; 898700fa0000
        mov     eax, dword ptr [esi + 0x17700]   ; 8b8600770100
        mov     dword ptr [edi + 0x17700], eax   ; 898700770100
        mov     eax, dword ptr [esi + 0x50]      ; 8b4650
        mov     dword ptr [edi + 0x50], eax      ; 894750
        mov     eax, dword ptr [esi + 0x7d50]    ; 8b86507d0000
        mov     dword ptr [edi + 0x7d50], eax    ; 8987507d0000
        mov     eax, dword ptr [esi + 0xfa50]    ; 8b8650fa0000
        mov     dword ptr [edi + 0xfa50], eax    ; 898750fa0000
        mov     eax, dword ptr [esi + 0x17750]   ; 8b8650770100
        mov     dword ptr [edi + 0x17750], eax   ; 898750770100
        mov     eax, dword ptr [esi + 0xa0]      ; 8b86a0000000
        mov     dword ptr [edi + 0xa0], eax      ; 8987a0000000
        mov     eax, dword ptr [esi + 0x7da0]    ; 8b86a07d0000
        mov     dword ptr [edi + 0x7da0], eax    ; 8987a07d0000
        mov     eax, dword ptr [esi + 0xfaa0]    ; 8b86a0fa0000
        mov     dword ptr [edi + 0xfaa0], eax    ; 8987a0fa0000
        mov     eax, dword ptr [esi + 0x177a0]   ; 8b86a0770100
        mov     dword ptr [edi + 0x177a0], eax   ; 8987a0770100
        mov     eax, dword ptr [esi + 0xf0]      ; 8b86f0000000
        mov     dword ptr [edi + 0xf0], eax      ; 8987f0000000
        mov     eax, dword ptr [esi + 0x7df0]    ; 8b86f07d0000
        mov     dword ptr [edi + 0x7df0], eax    ; 8987f07d0000
        mov     eax, dword ptr [esi + 0xfaf0]    ; 8b86f0fa0000
        mov     dword ptr [edi + 0xfaf0], eax    ; 8987f0fa0000
        mov     eax, dword ptr [esi + 0x177f0]   ; 8b86f0770100
        mov     dword ptr [edi + 0x177f0], eax   ; 8987f0770100
        mov     eax, dword ptr [esi + 0x140]     ; 8b8640010000
        mov     dword ptr [edi + 0x140], eax     ; 898740010000
        mov     eax, dword ptr [esi + 0x7e40]    ; 8b86407e0000
        mov     dword ptr [edi + 0x7e40], eax    ; 8987407e0000
        mov     eax, dword ptr [esi + 0xfb40]    ; 8b8640fb0000
        mov     dword ptr [edi + 0xfb40], eax    ; 898740fb0000
        mov     eax, dword ptr [esi + 0x17840]   ; 8b8640780100
        mov     dword ptr [edi + 0x17840], eax   ; 898740780100
        mov     eax, dword ptr [esi + 0x190]     ; 8b8690010000
        mov     dword ptr [edi + 0x190], eax     ; 898790010000
        mov     eax, dword ptr [esi + 0x7e90]    ; 8b86907e0000
        mov     dword ptr [edi + 0x7e90], eax    ; 8987907e0000
        mov     eax, dword ptr [esi + 0xfb90]    ; 8b8690fb0000
        mov     dword ptr [edi + 0xfb90], eax    ; 898790fb0000
        mov     eax, dword ptr [esi + 0x17890]   ; 8b8690780100
        mov     dword ptr [edi + 0x17890], eax   ; 898790780100
        mov     eax, dword ptr [esi + 0x1e0]     ; 8b86e0010000
        mov     dword ptr [edi + 0x1e0], eax     ; 8987e0010000
        mov     eax, dword ptr [esi + 0x7ee0]    ; 8b86e07e0000
        mov     dword ptr [edi + 0x7ee0], eax    ; 8987e07e0000
        mov     eax, dword ptr [esi + 0xfbe0]    ; 8b86e0fb0000
        mov     dword ptr [edi + 0xfbe0], eax    ; 8987e0fb0000
        mov     eax, dword ptr [esi + 0x178e0]   ; 8b86e0780100
        mov     dword ptr [edi + 0x178e0], eax   ; 8987e0780100
        mov     eax, dword ptr [esi + 0x230]     ; 8b8630020000
        mov     dword ptr [edi + 0x230], eax     ; 898730020000
        mov     eax, dword ptr [esi + 0x7f30]    ; 8b86307f0000
        mov     dword ptr [edi + 0x7f30], eax    ; 8987307f0000
        mov     eax, dword ptr [esi + 0xfc30]    ; 8b8630fc0000
        mov     dword ptr [edi + 0xfc30], eax    ; 898730fc0000
        mov     eax, dword ptr [esi + 0x17930]   ; 8b8630790100
        mov     dword ptr [edi + 0x17930], eax   ; 898730790100
        mov     eax, dword ptr [esi + 0x280]     ; 8b8680020000
        mov     dword ptr [edi + 0x280], eax     ; 898780020000
        mov     eax, dword ptr [esi + 0x7f80]    ; 8b86807f0000
        mov     dword ptr [edi + 0x7f80], eax    ; 8987807f0000
        mov     eax, dword ptr [esi + 0xfc80]    ; 8b8680fc0000
        mov     dword ptr [edi + 0xfc80], eax    ; 898780fc0000
        mov     eax, dword ptr [esi + 0x17980]   ; 8b8680790100
        mov     dword ptr [edi + 0x17980], eax   ; 898780790100
        mov     eax, dword ptr [esi + 0x2d0]     ; 8b86d0020000
        mov     dword ptr [edi + 0x2d0], eax     ; 8987d0020000
        mov     eax, dword ptr [esi + 0x7fd0]    ; 8b86d07f0000
        mov     dword ptr [edi + 0x7fd0], eax    ; 8987d07f0000
        mov     eax, dword ptr [esi + 0xfcd0]    ; 8b86d0fc0000
        mov     dword ptr [edi + 0xfcd0], eax    ; 8987d0fc0000
        mov     eax, dword ptr [esi + 0x179d0]   ; 8b86d0790100
        mov     dword ptr [edi + 0x179d0], eax   ; 8987d0790100
        mov     eax, dword ptr [esi + 0x320]     ; 8b8620030000
        mov     dword ptr [edi + 0x320], eax     ; 898720030000
        mov     eax, dword ptr [esi + 0x8020]    ; 8b8620800000
        mov     dword ptr [edi + 0x8020], eax    ; 898720800000
        mov     eax, dword ptr [esi + 0xfd20]    ; 8b8620fd0000
        mov     dword ptr [edi + 0xfd20], eax    ; 898720fd0000
        mov     eax, dword ptr [esi + 0x17a20]   ; 8b86207a0100
        mov     dword ptr [edi + 0x17a20], eax   ; 8987207a0100
        mov     eax, dword ptr [esi + 0x370]     ; 8b8670030000
        mov     dword ptr [edi + 0x370], eax     ; 898770030000
        mov     eax, dword ptr [esi + 0x8070]    ; 8b8670800000
        mov     dword ptr [edi + 0x8070], eax    ; 898770800000
        mov     eax, dword ptr [esi + 0xfd70]    ; 8b8670fd0000
        mov     dword ptr [edi + 0xfd70], eax    ; 898770fd0000
        mov     eax, dword ptr [esi + 0x17a70]   ; 8b86707a0100
        mov     dword ptr [edi + 0x17a70], eax   ; 8987707a0100
        mov     eax, dword ptr [esi + 0x3c0]     ; 8b86c0030000
        mov     dword ptr [edi + 0x3c0], eax     ; 8987c0030000
        mov     eax, dword ptr [esi + 0x80c0]    ; 8b86c0800000
        mov     dword ptr [edi + 0x80c0], eax    ; 8987c0800000
        mov     eax, dword ptr [esi + 0xfdc0]    ; 8b86c0fd0000
        mov     dword ptr [edi + 0xfdc0], eax    ; 8987c0fd0000
        mov     eax, dword ptr [esi + 0x17ac0]   ; 8b86c07a0100
        mov     dword ptr [edi + 0x17ac0], eax   ; 8987c07a0100
        mov     eax, dword ptr [esi + 0x410]     ; 8b8610040000
        mov     dword ptr [edi + 0x410], eax     ; 898710040000
        mov     eax, dword ptr [esi + 0x8110]    ; 8b8610810000
        mov     dword ptr [edi + 0x8110], eax    ; 898710810000
        mov     eax, dword ptr [esi + 0xfe10]    ; 8b8610fe0000
        mov     dword ptr [edi + 0xfe10], eax    ; 898710fe0000
        mov     eax, dword ptr [esi + 0x17b10]   ; 8b86107b0100
        mov     dword ptr [edi + 0x17b10], eax   ; 8987107b0100
        mov     eax, dword ptr [esi + 0x460]     ; 8b8660040000
        mov     dword ptr [edi + 0x460], eax     ; 898760040000
        mov     eax, dword ptr [esi + 0x8160]    ; 8b8660810000
        mov     dword ptr [edi + 0x8160], eax    ; 898760810000
        mov     eax, dword ptr [esi + 0xfe60]    ; 8b8660fe0000
        mov     dword ptr [edi + 0xfe60], eax    ; 898760fe0000
        mov     eax, dword ptr [esi + 0x17b60]   ; 8b86607b0100
        mov     dword ptr [edi + 0x17b60], eax   ; 8987607b0100
        mov     eax, dword ptr [esi + 0x4b0]     ; 8b86b0040000
        mov     dword ptr [edi + 0x4b0], eax     ; 8987b0040000
        mov     eax, dword ptr [esi + 0x81b0]    ; 8b86b0810000
        mov     dword ptr [edi + 0x81b0], eax    ; 8987b0810000
        mov     eax, dword ptr [esi + 0xfeb0]    ; 8b86b0fe0000
        mov     dword ptr [edi + 0xfeb0], eax    ; 8987b0fe0000
        mov     eax, dword ptr [esi + 0x17bb0]   ; 8b86b07b0100
        mov     dword ptr [edi + 0x17bb0], eax   ; 8987b07b0100
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        leave                                    ; c9
        ret                                      ; c3
        jmp     0x49ed7                          ; eb14
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x49ece                          ; 7402
        jmp     0x49ed7                          ; eb09
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x49ed7                          ; 7400
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
