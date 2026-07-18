; timer_rate_critsec @ 0x39846  (51 bytes) -- hand-written assembly (fully commented).
;
; timer_rate_critsec: set a voice's timer/callback rate, expressed as a frequency in Hz,
; inside a critical section. It converts the requested frequency to a period in
; microseconds (period = 1_000_000 / freq) and forwards it to set_seq_period, which stores
; it in the voice's timer slot and calls recompute_timer_period so the shared PIT ends up
; at the fastest voice's rate.
;
; Args (cdecl): [ebp+8]  = voice index (0..15)
;               [ebp+0xc] = requested frequency in Hz (the divisor below)
; The div: edx:eax = 0xf4240 (1,000,000) / [ebp+0xc]  ->  eax = microseconds per tick.
; Calls: set_seq_period(voice, period_us) which in turn runs recompute_timer_period.

timer_rate_critsec:
        push    ebp
        mov     ebp, esp
        push    esi
        push    edi
        pushfd                               ; save flags (incl. IF)
        cli                                  ; critical section
        mov     edx, 0                       ; edx:eax dividend high = 0
        mov     eax, 0xf4240                 ; 1,000,000 microseconds per second
        mov     ebx, dword ptr [ebp + 0xc]   ; ebx = frequency in Hz
        div     ebx                          ; eax = period in microseconds
        push    eax                          ; arg2 = period_us
        push    dword ptr [ebp + 8]          ; arg1 = voice index
        call    0x397f1                      ; -> set_seq_period: store period, recompute PIT rate
        add     esp, 8
        push    ebp                          ; --- restore caller's interrupt state, then flags ---
        mov     ebp, esp
        test    byte ptr [ebp + 5], 2        ; saved EFLAGS bit 9 (IF) set on entry?
        cli
        je      flags_done
        sti
flags_done:
        pop     ebp
        popfd
        pop     edi
        pop     esi
        leave
        ret
