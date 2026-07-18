; FUN_0003979c @ 0003979c  (48 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0003979c.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0003979c:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        mov     ebx, dword ptr [ebp + 8]         ; 8b5d08
        shl     ebx, 1                           ; d1e3
        cmp     word ptr [ebx + 0xbc38], 2       ; 6683bb38bc000002
        jne     0x397bb                          ; 7509
        mov     word ptr [ebx + 0xbc38], 1       ; 66c78338bc00000100
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x397c6                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3
