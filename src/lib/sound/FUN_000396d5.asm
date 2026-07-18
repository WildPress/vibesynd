; FUN_000396d5 @ 000396d5  (77 bytes) -- hand-written assembly (fully commented).
;
; FUN_000396d5: release a sequence slot -- the counterpart to FUN_00039625. Mark the
; slot free and decrement the active-sequence count; when it reaches zero (the last
; sequence has stopped) stop PIT channel 0 and uninstall the timer ISR, tearing the
; whole timer subsystem back down.
;
; Args (stack, cdecl):  [ebp+8] = slot index (0..15), or -1 for "none" (no-op)
; No return value. Runs inside a cli critical section.
;
; Globals:  0xbc38 g_seq_state[17] (word)   0xbbf0 g_active_seq_count
; Calls:    0x39467 reprogram_pit_ch0(0) (stop PIT)   0x3942f FUN_0003942f (uninstall ISR)
;
FUN_000396d5:
        push    ebp                              ; 55
        mov     ebp, esp                          ; 8bec
        push    esi                               ; 56
        push    edi                               ; 57
        pushfd                                    ; 9c    save caller FLAGS
        cli                                       ; fa    enter critical section
        mov     ebx, dword ptr [ebp + 8]          ; 8b5d08       ebx = slot index (arg)
        cmp     ebx, -1                            ; 83fbff      slot == -1 (nothing to do)?
        je      0x39711                            ; 742d        yes -> .exit
        shl     ebx, 1                             ; d1e3        ebx = slot*2 (word index)
        cmp     word ptr [ebx + 0xbc38], 0         ; 6683bb38bc000000  already free?
        je      0x39711                            ; 7421        yes -> .exit
        mov     word ptr [ebx + 0xbc38], 0         ; 66c78338bc00000000  g_seq_state[slot] = 0 (free)
        dec     word ptr [0xbbf0]                  ; 66ff0df0bb0000  g_active_seq_count--
        jne     0x39711                            ; 750f        others still active -> .exit
; --- last sequence gone: tear down the timer subsystem ---
        push    0                                  ; 6a00        reload count 0 = stop PIT ch0
        call    0x39467                            ; e85efdffff -> reprogram_pit_ch0(0)
        add     esp, 4                             ; 83c404
        call    0x3942f                            ; e81efdffff -> FUN_0003942f: uninstall timer ISR
; .exit (0x39711): critical-section exit
        push    ebp                               ; 55
        mov     ebp, esp                          ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502      restore caller IF?
        cli                                      ; fa
        je      0x3971c                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3
