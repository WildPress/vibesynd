; install_timer_isr @ 0x393e0  (79 bytes) -- hand-written assembly (fully commented).
;
; install_timer_isr: hook the PC timer interrupt (IRQ0 -> INT 8) so the sound driver's
; own ISR is driven by every tick. It first reads and saves the current INT 8 handler
; (so it can be restored later), then installs the driver's ISR (at image offset
; 0x2bb76, in the code segment) as the new INT 8. Interrupts are disabled across the
; swap. Uses real-mode DOS services through the DOS extender.
;
; No args, no return value.
; DOS calls (INT 21h):
;   AH=0x35, AL=8  Get Interrupt Vector 8  -> ES:EBX = current handler (seg:off)
;   AH=0x25, AL=8  Set Interrupt Vector 8  <- DS:EDX = new handler (seg:off)
; Globals:
;   0xbce6  saved old INT 8 handler offset (ebx)
;   0xbcea  saved old INT 8 handler segment (es)
;   0xbc34  driver context pointer, set to 0x2bc2b (data the new ISR uses)
; New ISR: cs:0x2bb76.

install_timer_isr:
        push    esi
        push    edi
        pushfd                               ; save flags (incl. IF)
        cli                                  ; no ticks during the vector swap
        mov     eax, 8                       ; interrupt number 8 (IRQ0 timer)...
        mov     ah, 0x35                     ; ...AH=35h Get Interrupt Vector
        push    es
        int     0x21                         ; -> ES:EBX = current INT 8 handler
        mov     dx, es                       ; dx = old handler segment
        pop     es
        mov     dword ptr [0xbce6], ebx      ; save old handler offset
        mov     dword ptr [0xbcea], edx      ; save old handler segment
        mov     ebx, 0x2bc2b                 ; driver context pointer
        mov     dword ptr [0xbc34], ebx      ;   stash where the ISR will read it
        mov     eax, 8                       ; interrupt number 8...
        mov     edx, 0x2bb76                 ; ...new handler offset = driver ISR
        mov     bx, cs                       ; new handler segment = current code segment
        mov     ah, 0x25                     ; AH=25h Set Interrupt Vector
        push    ds
        mov     ds, bx                       ; DS:EDX must point at the new handler
        int     0x21                         ; install driver ISR as INT 8
        pop     ds
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
        ret
