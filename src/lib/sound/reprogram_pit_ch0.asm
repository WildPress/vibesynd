; reprogram_pit_ch0 @ 0x39467  (46 bytes) -- hand-written assembly (fully commented).
;
; reprogram_pit_ch0: load a new reload count into the 8253/8254 Programmable Interval
; Timer, channel 0 (the system tick, wired to IRQ0). Writing a smaller divisor makes
; the timer -- and therefore the music/voice ISR -- fire faster. Runs with interrupts
; disabled around the port writes so a tick can't land between the low and high bytes.
;
; Args (cdecl): [ebp+8] = 16-bit reload count / divisor (the PIT counts it down to 0).
; Ports:  0x43  PIT mode/command register
;         0x40  PIT channel-0 data (low byte then high byte)
; Globals: 0xbcf6  g_pit_divisor -- remembers the last value programmed.
;
; The two `jmp $+2` are the traditional I/O settle delays between consecutive OUTs to
; the PIT (they just flush the prefetch queue, giving the chip time to latch).

reprogram_pit_ch0:
        push    ebp
        mov     ebp, esp
        push    esi
        push    edi
        pushfd                               ; save flags (incl. IF)
        cli                                  ; no ticks during the reprogram
        mov     al, 0x36                     ; PIT: channel 0, lobyte/hibyte, mode 3 (square wave), binary
        out     0x43, al                     ; write command register
        mov     eax, dword ptr [ebp + 8]     ; eax = new divisor (al=low, ah=high)
        mov     dword ptr [0xbcf6], eax       ; g_pit_divisor = divisor
        jmp     write_lo                     ; I/O settle delay
write_lo:
        out     0x40, al                     ; channel-0 reload, low byte
        mov     al, ah                       ; move high byte into al
        jmp     write_hi                     ; I/O settle delay
write_hi:
        out     0x40, al                     ; channel-0 reload, high byte
        push    ebp                          ; --- restore caller's interrupt state, then flags ---
        mov     ebp, esp
        test    byte ptr [ebp + 5], 2        ; saved EFLAGS bit 9 (IF) set on entry?
        cli
        je      flags_done                   ;   IF was clear -> keep interrupts off
        sti                                  ;   IF was set   -> re-enable
flags_done:
        pop     ebp
        popfd
        pop     edi
        pop     esi
        leave
        ret
