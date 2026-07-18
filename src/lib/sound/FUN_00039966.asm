; FUN_00039966 @ 00039966  (46 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00039966.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00039966:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        mov     ebx, dword ptr [ebp + 8]         ; 8b5d08
        cmp     ebx, 0x10                        ; 83fb10
        jae     0x39983                          ; 730e
        shl     ebx, 1                           ; d1e3
        shl     ebx, 1                           ; d1e3
        mov     dword ptr [ebx + 0xbcfa], 0      ; c783fabc000000000000
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x3998e                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3
