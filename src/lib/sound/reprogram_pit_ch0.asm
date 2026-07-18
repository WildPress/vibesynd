; reprogram_pit_ch0 @ 00039467  (46 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses reprogram_pit_ch0.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
reprogram_pit_ch0:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        mov     al, 0x36                         ; b036
        out     0x43, al                         ; e643
        mov     eax, dword ptr [ebp + 8]         ; 8b4508
        mov     dword ptr [0xbcf6], eax          ; a3f6bc0000
        jmp     0x3947c                          ; eb00
        out     0x40, al                         ; e640
        mov     al, ah                           ; 8ac4
        jmp     0x39482                          ; eb00
        out     0x40, al                         ; e640
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x3948f                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3
