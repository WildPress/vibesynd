; FUN_0004d1db @ 0004d1db  (70 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004d1db.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004d1db:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ecx                              ; 51
        push    edi                              ; 57
        push    esi                              ; 56
        mov     ecx, dword ptr [ebp + 0x10]      ; 8b4d10
        test    ecx, 3                           ; f7c103000000
        je      0x4d211                          ; 7425
        test    ecx, 2                           ; f7c102000000
        je      0x4d201                          ; 740d
        mov     esi, dword ptr [ebp + 8]         ; 8b7508
        mov     edi, dword ptr [ebp + 0xc]       ; 8b7d0c
        rep movsb byte ptr es:[edi], byte ptr [esi] ; f3a4
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     ecx                              ; 59
        leave                                    ; c9
        ret                                      ; c3
        shr     ecx, 1                           ; d1e9
        mov     esi, dword ptr [ebp + 8]         ; 8b7508
        mov     edi, dword ptr [ebp + 0xc]       ; 8b7d0c
        rep movsw word ptr es:[edi], word ptr [esi] ; f366a5
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     ecx                              ; 59
        leave                                    ; c9
        ret                                      ; c3
        shr     ecx, 2                           ; c1e902
        mov     esi, dword ptr [ebp + 8]         ; 8b7508
        mov     edi, dword ptr [ebp + 0xc]       ; 8b7d0c
        rep movsd dword ptr es:[edi], dword ptr [esi] ; f3a5
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     ecx                              ; 59
        leave                                    ; c9
        ret                                      ; c3
