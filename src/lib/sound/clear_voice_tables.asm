; clear_voice_tables @ 0x39393  (77 bytes) -- hand-written assembly (fully commented).
;
; clear_voice_tables: reset the timer-scheduling tables (the ones recompute_timer_period
; scans), as opposed to the voice-dispatch tables that init_voice_tables handles. Runs
; with interrupts disabled and forces ES=DS for the string stores.
;
; What it clears:
;   0xbce2  g_timer_cur_period  -> 0xffffffff  (no rate programmed yet)
;   0xbc38  g_timer_active[17]  -> 0  (17 words; no slot wants ticks)
;   0xbc5a  g_timer_accum[17]   -> 0  (17 dwords; per-slot accumulators)
;   0xbc9e  g_timer_period[17]  -> 0  (17 dwords; requested periods)
; The counts are 0x11 (17) rather than 16 -- these timer tables carry one extra slot
; beyond the 16 voices.

clear_voice_tables:
        push    esi
        push    edi
        push    es
        pushfd                               ; save flags (incl. IF)
        cli                                  ; critical section
        push    ds
        pop     es                           ; es = ds for the stos runs
        cld
        mov     dword ptr [0xbce2], 0xffffffff ; g_timer_cur_period = none
        mov     edi, 0xbc38                  ; g_timer_active[] (words)
        mov     ecx, 0x11                    ; 17 entries
        mov     eax, 0
        rep stosw word ptr es:[edi], ax      ;   = 0
        mov     edi, 0xbc5a                  ; g_timer_accum[] (dwords)
        mov     ecx, 0x11
        rep stosd dword ptr es:[edi], eax    ;   = 0
        mov     edi, 0xbc9e                  ; g_timer_period[] (dwords)
        mov     ecx, 0x11
        rep stosd dword ptr es:[edi], eax    ;   = 0
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
