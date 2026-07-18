; FUN_0004a492 @ 0x4a492  (226 bytes) -- hand-written assembly (fully commented).
;
; vga_planar_present: copy the finished offscreen frame to VGA display memory in
; the "unchained" mode-X planar layout.
;
; VGA planar memory has four bit-planes that all share the same addresses. A byte
; you write lands in whichever planes are currently enabled by the Sequencer's
; Map Mask register (index 2, reached through I/O port 0x3c4 for the index and
; 0x3c5 for the data). So to push a whole frame you enable one plane, copy that
; plane's share of the pixels, switch to the next plane, and repeat four times.
;
; The offscreen buffer g_screen_buf holds the four planes back to back, each
; 0x7d00 (32000) bytes apart. g_536c is the base of VGA video memory. For each
; plane the routine sets that plane's mask (0x01, 0x02, 0x04, 0x08), then copies
; 400 rows of 16 bytes from the plane's slice of the offscreen buffer into video
; memory, stepping 0x40 bytes down each side per row.
;
; Runs only when render-mode flag bit 1 (at 0x105) is set. Saves every register.
;
; Globals:  0x5368  g_screen_buf   offscreen frame, four planes x 0x7d00
;           0x536c  VGA video-memory base
;           0x105   render-mode flags
; Ports:    0x3c4   VGA Sequencer address/data (Map Mask is index 2)

FUN_0004a492:
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        test    byte [0x105], 2
        je      done                ; only present the frame in this render mode
        mov     dx, 0x3c4           ; dx = VGA Sequencer index port

; --- plane 0 ---
        mov     esi, [0x5368]       ; source = offscreen plane-0 slice (g_screen_buf + 0)
        mov     edi, [0x536c]       ; dest   = VGA memory base
        mov     ax, 0x102           ; Sequencer[2] (Map Mask) <- 0x01: enable plane 0 only
        out     dx, ax
        mov     ebx, 0x190          ; 400 rows
        mov     ecx, 4              ; 4 dwords = 16 bytes copied per row
p0row:  rep movsd                   ; copy one row into plane 0
        add     esi, 0x40           ; next source row (row pitch 0x40)
        add     edi, 0x40           ; next dest row
        dec     ebx
        jne     p0row

; --- plane 1 ---
        mov     esi, [0x5368]
        add     esi, 0x7d00         ; source = plane-1 slice (+0x7d00)
        mov     edi, [0x536c]
        mov     ax, 0x202           ; Map Mask <- 0x02: enable plane 1
        out     dx, ax
        mov     ebx, 0x190
        mov     ecx, 4
p1row:  rep movsd
        add     esi, 0x40
        add     edi, 0x40
        dec     ebx
        jne     p1row

; --- plane 2 ---
        mov     esi, [0x5368]
        add     esi, 0xfa00         ; source = plane-2 slice (+2*0x7d00)
        mov     edi, [0x536c]
        mov     ax, 0x402           ; Map Mask <- 0x04: enable plane 2
        out     dx, ax
        mov     ebx, 0x190
        mov     ecx, 4
p2row:  rep movsd
        add     esi, 0x40
        add     edi, 0x40
        dec     ebx
        jne     p2row

; --- plane 3 ---
        mov     esi, [0x5368]
        mov     edi, [0x536c]
        add     esi, 0x17700        ; source = plane-3 slice (+3*0x7d00)
        mov     ax, 0x802           ; Map Mask <- 0x08: enable plane 3
        out     dx, ax
        mov     ebx, 0x190
        mov     ecx, 4
p3row:  rep movsd
        add     esi, 0x40
        add     edi, 0x40
        dec     ebx
        jne     p3row
        jmp     done

; The tail mirrors the render-mode dispatch other draw routines use; in this one
; the non-planar modes do nothing extra and fall straight through to the epilogue.
        test    byte [0x105], 4
        je      $+4
        jmp     done
        test    byte [0x105], 1
        je      done

done:
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        ret
