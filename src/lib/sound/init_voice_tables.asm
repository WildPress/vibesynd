; init_voice_tables @ 0003954c  (106 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses init_voice_tables.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
init_voice_tables:
        push    esi                              ; 56
        push    edi                              ; 57
        push    es                               ; 06
        pushfd                                   ; 9c
        cli                                      ; fa
        mov     word ptr [0xbdca], ds            ; 668c1dcabd0000
        mov     es, word ptr [0xbdca]            ; 668e05cabd0000
        mov     word ptr [0xbbf0], 0             ; 66c705f0bb00000000
        mov     word ptr [0xbbf2], 0             ; 66c705f2bb00000000
        cld                                      ; fc
        mov     edi, 0xbcfa                      ; bffabc0000
        mov     ecx, 0x10                        ; b910000000
        mov     eax, 0                           ; b800000000
        rep stosd dword ptr es:[edi], eax        ; f3ab
        mov     edi, 0xbd3a                      ; bf3abd0000
        mov     ecx, 0x10                        ; b910000000
        mov     eax, 0xffffffff                  ; b8ffffffff
        rep stosd dword ptr es:[edi], eax        ; f3ab
        mov     edi, 0xbd7a                      ; bf7abd0000
        mov     ecx, 0x10                        ; b910000000
        mov     eax, 0                           ; b800000000
        rep stosd dword ptr es:[edi], eax        ; f3ab
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x395b0                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     es                               ; 07
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        ret                                      ; c3
