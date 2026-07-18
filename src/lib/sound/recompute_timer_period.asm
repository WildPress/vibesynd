; recompute_timer_period @ 000394c6  (134 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses recompute_timer_period.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
recompute_timer_period:
        push    esi                              ; 56
        push    edi                              ; 57
        push    es                               ; 06
        pushfd                                   ; 9c
        cli                                      ; fa
        cld                                      ; fc
        mov     dword ptr [0xbcf2], 0xffffffff   ; c705f2bc0000ffffffff
        mov     esi, 0                           ; be00000000
        mov     ebx, esi                         ; 8bde
        shl     ebx, 1                           ; d1e3
        cmp     word ptr [ebx + 0xbc38], 0       ; 6683bb38bc000000
        je      0x394fd                          ; 7414
        mov     eax, dword ptr [ebx*2 + 0xbc9e]  ; 8b045d9ebc0000
        cmp     eax, dword ptr [0xbcf2]          ; 3b05f2bc0000
        jae     0x394fd                          ; 7305
        mov     dword ptr [0xbcf2], eax          ; a3f2bc0000
        inc     esi                              ; 46
        cmp     esi, 0x10                        ; 83fe10
        jbe     0x394db                          ; 76d8
        mov     eax, dword ptr [0xbcf2]          ; a1f2bc0000
        cmp     eax, dword ptr [0xbce2]          ; 3b05e2bc0000
        je      0x3953b                          ; 742b
        mov     dword ptr [0xbcee], 0xffffffff   ; c705eebc0000ffffffff
        mov     dword ptr [0xbce2], eax          ; a3e2bc0000
        push    eax                              ; 50
        call    0x39495                          ; e870ffffff     -> FUN_00039495
        add     esp, 4                           ; 83c404
        mov     edi, 0xbc5a                      ; bf5abc0000
        mov     ecx, 0x11                        ; b911000000
        mov     eax, 0                           ; b800000000
        push    ds                               ; 1e
        pop     es                               ; 07
        rep stosd dword ptr es:[edi], eax        ; f3ab
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x39546                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     es                               ; 07
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        ret                                      ; c3
