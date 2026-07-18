; program_pit_period_us @ 00039495  (49 bytes) -- hand-written assembly (fully commented).
;
; program_pit_period_us: convert a period in microseconds to an 8253/8254 PIT channel-0 reload
; count and program it, via reprogram_pit_ch0. This is the helper recompute_timer_period
; calls once it has found the smallest requested period across all sequences.
;
; The scale is count = period_us * 10000 / 8380 = period_us * 1.19331, i.e. the PIT
; input frequency 1.19318 MHz expressed as counts per microsecond. Periods at or above
; 0xd68d us (54925 us, the longest a 16-bit reload can express) collapse to reload 0,
; which reprogram_pit_ch0 writes as the maximum (65536) count.
;
; Args (stack, cdecl):  [ebp+8] = period in microseconds
; Calls:  0x39467 reprogram_pit_ch0(reload_count)
;
program_pit_period_us:
        push    ebp                              ; 55
        mov     ebp, esp                          ; 8bec
        push    esi                               ; 56
        push    edi                               ; 57
        mov     eax, 0                             ; b800000000   default reload = 0 (max period)
        cmp     dword ptr [ebp + 8], 0xd68d        ; 817d088dd60000  period >= 54925 us ?
        jae     0x394b9                            ; 7311         yes -> use reload 0 (.program)
        mov     eax, dword ptr [ebp + 8]           ; 8b4508       eax = period_us
        mov     ebx, 0x20bc                         ; bbbc200000  ebx = 8380 (divisor)
        mov     ecx, 0x2710                         ; b910270000  ecx = 10000 (multiplier)
        mul     ecx                                 ; f7e1        edx:eax = period_us * 10000
        div     ebx                                 ; f7f3        eax = period_us * 10000 / 8380 (PIT count)
; .program (0x394b9):
        push    eax                                 ; 50          arg = reload count
        call    0x39467                             ; e8a8ffffff -> reprogram_pit_ch0(count)
        add     esp, 4                              ; 83c404
        pop     edi                                 ; 5f
        pop     esi                                 ; 5e
        leave                                       ; c9
        ret                                         ; c3
