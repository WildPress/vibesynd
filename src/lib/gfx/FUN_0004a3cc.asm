; FUN_0004a3cc @ 0004a3cc  (99 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004a3cc.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004a3cc:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        mov     esi, dword ptr [0x5368]          ; 8b3568530000   0x5368=g_screen_buf
        mov     edi, 0x3cb74                     ; bf74cb0300
        movzx   eax, word ptr [ebp + 8]          ; 0fb74508
        imul    eax, eax, 0x50                   ; 6bc050
        add     esi, eax                         ; 03f0
        mov     ecx, 0x11                        ; b911000000
        cmp     word ptr [ebp + 8], 0x190        ; 66817d089001
        jge     0x4a427                          ; 7d31
        mov     edx, dword ptr [edi]             ; 8b17
        mov     dword ptr [esi], edx             ; 8916
        add     edi, 4                           ; 83c704
        mov     edx, dword ptr [edi]             ; 8b17
        mov     dword ptr [esi + 0x7d00], edx    ; 8996007d0000
        add     edi, 4                           ; 83c704
        mov     edx, dword ptr [edi]             ; 8b17
        mov     dword ptr [esi + 0xfa00], edx    ; 899600fa0000
        add     edi, 4                           ; 83c704
        mov     edx, dword ptr [edi]             ; 8b17
        mov     dword ptr [esi + 0x17700], edx   ; 899600770100
        add     edi, 4                           ; 83c704
        add     esi, 0x50                        ; 83c650
        inc     word ptr [ebp + 8]               ; 66ff4508
        loop    0x4a3ee                          ; e2c7
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        leave                                    ; c9
        ret                                      ; c3
