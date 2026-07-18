; recompute_timer_period @ 0x394c6  (134 bytes) -- hand-written assembly (fully commented).
;
; recompute_timer_period: pick the fastest tick any active voice needs and, if that has
; changed, reprogram the PIT to it. Several voices each want the timer ISR called at
; their own rate; the shared hardware timer can only run at one rate, so the driver runs
; it at the smallest (fastest) requested period and derives every voice's own callbacks
; by sub-dividing. This routine rescans all 17 timer slots, finds that minimum, and
; only touches the PIT when the minimum differs from what is currently programmed.
;
; No args. Runs with interrupts disabled (it walks tables the ISR also reads).
; Globals:
;   0xbc38  g_timer_active[17]  word flag per slot (nonzero = slot wants ticks)
;   0xbc9e  g_timer_period[17]  dword requested period per slot (smaller = faster)
;   0xbcf2  scratch: running minimum period during the scan (starts 0xffffffff)
;   0xbce2  g_timer_cur_period  period currently programmed into the PIT
;   0xbcee  reset marker, set to -1 when the rate changes
;   0xbc5a  g_timer_accum[17]   per-slot accumulators, zeroed on a rate change
; Calls: program_pit_period_us (convert period -> PIT divisor and program channel 0).

recompute_timer_period:
        push    esi
        push    edi
        push    es
        pushfd                               ; save flags (incl. IF)
        cli                                  ; critical section
        cld
        mov     dword ptr [0xbcf2], 0xffffffff ; running minimum = +infinity
        mov     esi, 0                       ; esi = slot index 0
scan_loop:
        mov     ebx, esi
        shl     ebx, 1                        ; ebx = esi*2 (word index into g_timer_active)
        cmp     word ptr [ebx + 0xbc38], 0   ; is this slot active?
        je      next_voice                   ;   no -> skip it
        mov     eax, dword ptr [ebx*2 + 0xbc9e] ; eax = g_timer_period[esi]  (ebx*2 = esi*4)
        cmp     eax, dword ptr [0xbcf2]      ; smaller than current minimum?
        jae     next_voice                   ;   no -> keep old minimum
        mov     dword ptr [0xbcf2], eax       ; new minimum
next_voice:
        inc     esi
        cmp     esi, 0x10                    ; scan slots 0..16 (<=0x10)
        jbe     scan_loop
        mov     eax, dword ptr [0xbcf2]      ; eax = fastest period found
        cmp     eax, dword ptr [0xbce2]      ; already the programmed rate?
        je      done_scan                    ;   unchanged -> nothing to do
        mov     dword ptr [0xbcee], 0xffffffff ; mark the rate as reset
        mov     dword ptr [0xbce2], eax       ; remember the new current period
        push    eax
        call    0x39495                      ; -> program_pit_period_us: period -> divisor, program PIT ch0
        add     esp, 4
        mov     edi, 0xbc5a                  ; zero all 17 per-slot accumulators...
        mov     ecx, 0x11
        mov     eax, 0
        push    ds
        pop     es                           ; es = ds for the stos
        rep stosd dword ptr es:[edi], eax    ; g_timer_accum[0..16] = 0
done_scan:
        push    ebp                          ; --- restore caller's interrupt state, then flags ---
        mov     ebp, esp
        test    byte ptr [ebp + 5], 2        ; saved EFLAGS bit 9 (IF) set on entry?
        cli
        je      flags_done
        sti
flags_done:
        pop     ebp
        popfd
        pop     es
        pop     edi
        pop     esi
        ret
