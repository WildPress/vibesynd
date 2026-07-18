; FUN_000397f1 @ 000397f1  (85 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_000397f1.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_000397f1:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        mov     ebx, dword ptr [ebp + 8]         ; 8b5d08
        shl     ebx, 1                           ; d1e3
        movzx   eax, word ptr [ebx + 0xbc38]     ; 0fb78338bc0000
        push    eax                              ; 50
        mov     word ptr [ebx + 0xbc38], 1       ; 66c78338bc00000100
        shl     ebx, 1                           ; d1e3
        mov     eax, dword ptr [ebp + 0xc]       ; 8b450c
        mov     dword ptr [ebx + 0xbc9e], eax    ; 89839ebc0000
        mov     dword ptr [ebx + 0xbc5a], 0      ; c7835abc000000000000
        call    0x394c6                          ; e89efcffff     -> recompute_timer_period
        pop     eax                              ; 58
        mov     ebx, dword ptr [ebp + 8]         ; 8b5d08
        shl     ebx, 1                           ; d1e3
        mov     word ptr [ebx + 0xbc38], ax      ; 66898338bc0000
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x39840                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3
