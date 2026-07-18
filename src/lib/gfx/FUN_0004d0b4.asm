; FUN_0004d0b4 @ 0004d0b4  (229 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004d0b4.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004d0b4:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x4d17d                          ; 0f84b3000000
        mov     eax, dword ptr [ebp + 8]         ; 8b4508
        shl     eax, 2                           ; c1e002
        imul    esi, dword ptr [ebp + 0xc], 0x500 ; 69750c00050000
        add     esi, eax                         ; 03f0
        mov     eax, dword ptr [ebp + 0x10]      ; 8b4510
        shl     eax, 2                           ; c1e002
        imul    edi, dword ptr [ebp + 0x14], 0x500 ; 697d1400050000
        add     edi, eax                         ; 03f8
        add     esi, dword ptr [0x5370]          ; 033570530000   0x5370=g_back_buf
        add     edi, dword ptr [0x5368]          ; 033d68530000   0x5368=g_screen_buf
        mov     ebx, dword ptr [ebp + 0x1c]      ; 8b5d1c
        shl     ebx, 4                           ; c1e304
        mov     eax, dword ptr [ebp + 0x18]      ; 8b4518
        mov     edx, 0x14                        ; ba14000000
        sub     edx, eax                         ; 2bd0
        shl     edx, 2                           ; c1e202
        push    esi                              ; 56
        push    edi                              ; 57
        push    ebx                              ; 53
        mov     ecx, eax                         ; 8bc8
        rep movsd dword ptr es:[edi], dword ptr [esi] ; f3a5
        add     esi, edx                         ; 03f2
        add     edi, edx                         ; 03fa
        dec     ebx                              ; 4b
        jg      0x4d10a                          ; 7ff5
        pop     ebx                              ; 5b
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        add     edi, 0x7d00                      ; 81c7007d0000
        add     esi, 0x7d00                      ; 81c6007d0000
        push    esi                              ; 56
        push    edi                              ; 57
        push    ebx                              ; 53
        mov     ecx, eax                         ; 8bc8
        rep movsd dword ptr es:[edi], dword ptr [esi] ; f3a5
        add     esi, edx                         ; 03f2
        add     edi, edx                         ; 03fa
        dec     ebx                              ; 4b
        jg      0x4d127                          ; 7ff5
        pop     ebx                              ; 5b
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        add     edi, 0x7d00                      ; 81c7007d0000
        add     esi, 0x7d00                      ; 81c6007d0000
        push    esi                              ; 56
        push    edi                              ; 57
        push    ebx                              ; 53
        mov     ecx, eax                         ; 8bc8
        rep movsd dword ptr es:[edi], dword ptr [esi] ; f3a5
        add     esi, edx                         ; 03f2
        add     edi, edx                         ; 03fa
        dec     ebx                              ; 4b
        jg      0x4d144                          ; 7ff5
        pop     ebx                              ; 5b
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        add     edi, 0x7d00                      ; 81c7007d0000
        add     esi, 0x7d00                      ; 81c6007d0000
        push    esi                              ; 56
        push    edi                              ; 57
        push    ebx                              ; 53
        mov     ecx, eax                         ; 8bc8
        rep movsd dword ptr es:[edi], dword ptr [esi] ; f3a5
        add     esi, edx                         ; 03f2
        add     edi, edx                         ; 03fa
        dec     ebx                              ; 4b
        jg      0x4d161                          ; 7ff5
        pop     ebx                              ; 5b
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        add     edi, 0x7d00                      ; 81c7007d0000
        add     esi, 0x7d00                      ; 81c6007d0000
        jmp     0x4d191                          ; eb14
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x4d188                          ; 7402
        jmp     0x4d191                          ; eb09
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x4d191                          ; 7400
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        leave                                    ; c9
        ret                                      ; c3
