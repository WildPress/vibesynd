; set_seq_period @ 000397f1  (85 bytes) -- hand-written assembly (fully commented).
;
; set_seq_period: set a sequence's tick period (in microseconds) and re-derive the shared
; PIT rate from it. It stores the new period, resets the slot's tick accumulator, and
; runs recompute_timer_period -- which scans every active slot's period, takes the
; smallest, and reprograms PIT channel 0. To make sure this slot's new period is
; considered even if it was idle, the routine forces the state word to 1 (active) across
; the recompute, then restores whatever state the slot had before.
;
; Args (stack, cdecl):  [ebp+8] = slot index    [ebp+0xc] = period in microseconds
; No return value. Runs inside a cli critical section.
;
; Globals:  0xbc38 g_seq_state[17] (word)   0xbc9e g_seq_period[17] (dword, us)
;           0xbc5a g_seq_accum[17] (dword, tick accumulator)
; Calls:    0x394c6 recompute_timer_period
;
set_seq_period:
        push    ebp                              ; 55
        mov     ebp, esp                          ; 8bec
        push    esi                               ; 56
        push    edi                               ; 57
        pushfd                                    ; 9c    save caller FLAGS
        cli                                       ; fa    enter critical section
        mov     ebx, dword ptr [ebp + 8]          ; 8b5d08       ebx = slot index
        shl     ebx, 1                            ; d1e3        ebx = slot*2 (word index)
        movzx   eax, word ptr [ebx + 0xbc38]      ; 0fb78338bc0000  eax = current state (save it)
        push    eax                               ; 50           stash old state on the stack
        mov     word ptr [ebx + 0xbc38], 1        ; 66c78338bc00000100  force state = 1 for the recompute
        shl     ebx, 1                            ; d1e3        ebx = slot*4 (dword index)
        mov     eax, dword ptr [ebp + 0xc]        ; 8b450c       eax = new period (us)
        mov     dword ptr [ebx + 0xbc9e], eax     ; 89839ebc0000  g_seq_period[slot] = period
        mov     dword ptr [ebx + 0xbc5a], 0       ; c7835abc000000000000  g_seq_accum[slot] = 0 (reset phase)
        call    0x394c6                           ; e89efcffff  -> recompute_timer_period (reprogram PIT)
        pop     eax                               ; 58           eax = saved old state
        mov     ebx, dword ptr [ebp + 8]          ; 8b5d08       ebx = slot index
        shl     ebx, 1                            ; d1e3        ebx = slot*2 (word index)
        mov     word ptr [ebx + 0xbc38], ax       ; 66898338bc0000  restore original state word
; --- critical-section exit ---
        push    ebp                               ; 55
        mov     ebp, esp                          ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502      restore caller IF?
        cli                                      ; fa
        je      0x39840                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3
