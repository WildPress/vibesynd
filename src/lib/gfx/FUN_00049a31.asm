; FUN_00049a31 @ 00049a31  (70 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00049a31.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00049a31:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    eax                              ; 50
        push    ecx                              ; 51
        push    esi                              ; 56
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x49a5e                          ; 741e
        mov     esi, dword ptr [ebp + 8]         ; 8b7508
        mov     eax, dword ptr [ebp + 0xc]       ; 8b450c
        mov     cx, 0x640                        ; 66b94006
        mov     dword ptr [esi], eax             ; 8906
        mov     dword ptr [esi + 4], eax         ; 894604
        mov     dword ptr [esi + 8], eax         ; 894608
        mov     dword ptr [esi + 0xc], eax       ; 89460c
        add     esi, 0x50                        ; 83c650
        dec     cx                               ; 6649
        jne     0x49a4a                          ; 75ee
        jmp     0x49a72                          ; eb14
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x49a69                          ; 7402
        jmp     0x49a72                          ; eb09
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x49a72                          ; 7400
        pop     esi                              ; 5e
        pop     ecx                              ; 59
        pop     eax                              ; 58
        leave                                    ; c9
        ret                                      ; c3
