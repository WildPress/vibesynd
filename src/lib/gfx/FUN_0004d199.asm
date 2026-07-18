; FUN_0004d199 @ 0004d199  (66 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004d199.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004d199:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ecx                              ; 51
        push    edi                              ; 57
        mov     ecx, dword ptr [ebp + 0x10]      ; 8b4d10
        test    ecx, 3                           ; f7c103000000
        je      0x4d1cc                          ; 7423
        test    ecx, 2                           ; f7c102000000
        je      0x4d1bd                          ; 740c
        mov     edi, dword ptr [ebp + 8]         ; 8b7d08
        mov     eax, dword ptr [ebp + 0xc]       ; 8b450c
        rep stosb byte ptr es:[edi], al          ; f3aa
        pop     edi                              ; 5f
        pop     ecx                              ; 59
        leave                                    ; c9
        ret                                      ; c3
        shr     ecx, 1                           ; d1e9
        mov     edi, dword ptr [ebp + 8]         ; 8b7d08
        mov     eax, dword ptr [ebp + 0xc]       ; 8b450c
        rep stosw word ptr es:[edi], ax          ; f366ab
        pop     edi                              ; 5f
        pop     ecx                              ; 59
        leave                                    ; c9
        ret                                      ; c3
        shr     ecx, 2                           ; c1e902
        mov     edi, dword ptr [ebp + 8]         ; 8b7d08
        mov     eax, dword ptr [ebp + 0xc]       ; 8b450c
        rep stosd dword ptr es:[edi], eax        ; f3ab
        pop     edi                              ; 5f
        pop     ecx                              ; 59
        leave                                    ; c9
        ret                                      ; c3
