; FUN_00047a7e @ 00047a7e  (140 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00047a7e.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00047a7e:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x49862                          ; 0f84cf1d0000
        mov     ax, word ptr [ebp + 8]           ; 668b4508
        mov     bx, word ptr [ebp + 0xc]         ; 668b5d0c
        mov     cx, ax                           ; 668bc8
        mov     dx, bx                           ; 668bd3
        sar     ax, 8                            ; 66c1f808
        sar     bx, 8                            ; 66c1fb08
        shl     bx, 7                            ; 66c1e307
        mov     si, ax                           ; 668bf0
        add     si, bx                           ; 6603f3
        and     esi, 0xffff                      ; 81e6ffff0000
        shl     esi, 2                           ; c1e602
        and     cx, 0xff                         ; 6681e1ff00
        and     dx, 0xff                         ; 6681e2ff00
        mov     bx, dx                           ; 668bda
        add     bx, cx                           ; 6603d9
        mov     eax, 0                           ; b800000000
        cmp     bx, 0x100                        ; 6681fb0001
        jl      0x47adb                          ; 7c03
        add     eax, 8                           ; 83c008
        cmp     cx, dx                           ; 663bca
        jl      0x47ae3                          ; 7c03
        add     eax, 4                           ; 83c004
        add     esi, dword ptr [0x5358]          ; 033558530000   0x5358=g_map_cols
        jmp     dword ptr [eax + 0x3a3a7]        ; ffa0a7a30300
        lds     esp, ptr [ebx - 0x4d88fffd]      ; c5a3030077b2
        add     eax, dword ptr [eax]             ; 0300
        mov     bh, 0xa3                         ; b7a3
        add     eax, dword ptr [eax]             ; 0300
        ret     0x3a3                            ; c2a303
        add     byte ptr [ecx + 0x200c6], al     ; 0081c6000200
        add     cl, ch                           ; 00e9
        mov     ch, 0xe                          ; b50e
        add     byte ptr [eax], al               ; 0000
