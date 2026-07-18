; clear_voice_tables @ 00039393  (77 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses clear_voice_tables.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
clear_voice_tables:
        push    esi                              ; 56
        push    edi                              ; 57
        push    es                               ; 06
        pushfd                                   ; 9c
        cli                                      ; fa
        push    ds                               ; 1e
        pop     es                               ; 07
        cld                                      ; fc
        mov     dword ptr [0xbce2], 0xffffffff   ; c705e2bc0000ffffffff
        mov     edi, 0xbc38                      ; bf38bc0000
        mov     ecx, 0x11                        ; b911000000
        mov     eax, 0                           ; b800000000
        rep stosw word ptr es:[edi], ax          ; f366ab
        mov     edi, 0xbc5a                      ; bf5abc0000
        mov     ecx, 0x11                        ; b911000000
        rep stosd dword ptr es:[edi], eax        ; f3ab
        mov     edi, 0xbc9e                      ; bf9ebc0000
        mov     ecx, 0x11                        ; b911000000
        rep stosd dword ptr es:[edi], eax        ; f3ab
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x393da                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     es                               ; 07
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        ret                                      ; c3
