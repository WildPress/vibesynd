; reset_gfx_write_mode @ 0x49922  (25 bytes) -- hand-written assembly (fully commented).
;
; reset_gfx_write_mode: reset the VGA Graphics Controller to plain write mode. It writes the
; Bit Mask register (index 8) back to 0xff (all eight bits writable) and the Mode
; register (index 5) back to 0. This undoes any set/reset or special write mode a
; previous draw left in place, so ordinary CPU byte writes behave normally again.
;
; The same two GC writes appear inline at the head of the fill/present routines in
; this cluster (e.g. clear_vga_screen, FUN_0004a5a8); this is the standalone version.
;
; No args. Preserves ax and dx. Ports: 0x3ce VGA GC index, 0x3cf GC data
; (accessed here as a 16-bit OUT: AL=index, AH=data).
;
reset_gfx_write_mode:
        push    ax
        push    dx
        mov     dx, 0x3ce                        ; VGA Graphics Controller index port
        mov     ax, 0xff08                       ; index 8 (Bit Mask) <- 0xff : all bits writable
        out     dx, ax
        mov     ax, 5                            ; index 5 (Mode) <- 0 : plain write mode
        out     dx, ax
        pop     dx
        pop     ax
        ret
