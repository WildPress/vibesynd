; uninstall_timer_isr @ 0003942f  (56 bytes) -- hand-written assembly (fully commented).
;
; uninstall_timer_isr: uninstall the sound timer ISR -- restore the original INT 8 (IRQ0
; timer) handler that install_timer_isr saved away. This is the teardown counterpart
; to install_timer_isr, called when the last active sequence is released (see
; free_seq_slot). It runs inside a cli critical section and restores the caller's
; interrupt-enable state on the way out.
;
; No args, no return value.
;
; Globals:  0xbcee  a "pending"/dirty flag, forced to -1 (matches recompute)
;           0xbce6  saved original INT 8 handler offset  (from install_timer_isr)
;           0xbcea  saved original INT 8 handler selector (from install_timer_isr)
; DOS call: INT 21h AH=25h = Set Interrupt Vector (vector 8 = ds:edx)
;
uninstall_timer_isr:
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c    save caller's FLAGS (IF state)
        cli                                      ; fa    enter critical section
        mov     dword ptr [0xbcee], 0xffffffff   ; c705eebc0000ffffffff  reset pending flag to -1
        mov     eax, 8                            ; b808000000   AL = vector number 8 (IRQ0/timer)
        mov     edx, dword ptr [0xbce6]           ; 8b15e6bc0000 edx = saved handler offset
        mov     ebx, dword ptr [0xbcea]           ; 8b1deabc0000 ebx = saved handler selector
        mov     ah, 0x25                          ; b425         AH = 25h (Set Interrupt Vector)
        push    ds                                ; 1e           save ds
        mov     ds, bx                            ; 668edb       ds = saved selector (ds:edx = handler)
        int     0x21                              ; cd21         DOS: install saved vector as INT 8
        pop     ds                                ; 1f           restore ds
; --- critical-section exit: restore caller's IF from the saved FLAGS, then popfd ---
        push    ebp                               ; 55
        mov     ebp, esp                          ; 8bec
        test    byte ptr [ebp + 5], 2             ; f6450502     IF bit of saved FLAGS set?
        cli                                       ; fa
        je      0x39462                           ; 7401         if not, skip sti (leave ints off)
        sti                                       ; fb           re-enable interrupts as on entry
        pop     ebp                               ; 5d
        popfd                                     ; 9d           restore full FLAGS
        pop     edi                               ; 5f
        pop     esi                               ; 5e
        ret                                       ; c3
