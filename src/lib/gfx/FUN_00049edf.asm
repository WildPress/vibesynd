; FUN_00049edf @ 0x49edf  (989 bytes) -- hand-written assembly (fully commented).
;
; FUN_00049edf: present a 16-column x 16-row region of the offscreen buffer to VGA,
; one plane at a time, gated by a countdown byte. Only runs in planar render mode.
;
; The base offset is y*0x500 + 0x10 (a 16-row band). Source is g_screen_buf, dest is
; VGA memory (0x536c). For each of the four planes it selects the plane through the
; Sequencer Map Mask (port 0x3c4 -> 0x0102/0x0202/0x0402/0x0802) and, if the byte at
; [arg0] is > 0, copies 16 columns; each column is an unrolled run of 16 dwords one
; scanline (0x50) apart, then the column pointer steps on by one dword (4 bytes).
; After each plane the pointers rewind 0x40 (16 dwords) to the region's left edge.
; The final plane also decrements the byte at [arg0] once.
;
; The [arg0] byte therefore acts as a whole-region on/off gate that counts down once
; per call -- looks like a timed reveal of a screen patch (a transition/refresh).
;
; Args (stack / cdecl):
;   [ebp+8]   pointer to the gate/countdown byte
;   [ebp+0xc] region y (in 16-row bands)
; Globals:  0x105 render-mode flags   0x5368 g_screen_buf   0x536c VGA base
; Ports:    0x3c4/0x3c5 Sequencer (Map Mask = idx 2)
;
FUN_00049edf:
        push    ebp
        mov     ebp, esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        test    byte ptr [0x105], 2              ; planar mode?
        je      mode_tail                        ;   no -> shared mode tail
        imul    eax, dword ptr [ebp + 0xc], 0x500 ; y * 0x500
        add     eax, 0x10                        ; + 0x10 (region bias)
        mov     esi, dword ptr [0x5368]          ; g_screen_buf
        mov     edi, dword ptr [0x536c]          ; VGA base
        add     esi, eax                         ; source pointer
        add     edi, eax                         ; dest pointer
        mov     dx, 0x3c4                        ; Sequencer index port

; --- plane 0 ---
        mov     ax, 0x102                        ; Map Mask <- plane 0
        out     dx, ax
        mov     ecx, 0x10                        ; 16 columns
        mov     ebx, dword ptr [ebp + 8]         ; ebx = &gate byte
        cmp     byte ptr [ebx], 0                ; gate > 0 ?
        jle     p1_setup                         ;   no -> skip this plane
p0_loop:
        mov     eax, dword ptr [esi]             ; column of 16 rows (each 0x50 apart)
        mov     dword ptr [edi], eax
        mov     eax, dword ptr [esi + 0x50]
        mov     dword ptr [edi + 0x50], eax
        mov     eax, dword ptr [esi + 0xa0]
        mov     dword ptr [edi + 0xa0], eax
        mov     eax, dword ptr [esi + 0xf0]
        mov     dword ptr [edi + 0xf0], eax
        mov     eax, dword ptr [esi + 0x140]
        mov     dword ptr [edi + 0x140], eax
        mov     eax, dword ptr [esi + 0x190]
        mov     dword ptr [edi + 0x190], eax
        mov     eax, dword ptr [esi + 0x1e0]
        mov     dword ptr [edi + 0x1e0], eax
        mov     eax, dword ptr [esi + 0x230]
        mov     dword ptr [edi + 0x230], eax
        mov     eax, dword ptr [esi + 0x280]
        mov     dword ptr [edi + 0x280], eax
        mov     eax, dword ptr [esi + 0x2d0]
        mov     dword ptr [edi + 0x2d0], eax
        mov     eax, dword ptr [esi + 0x320]
        mov     dword ptr [edi + 0x320], eax
        mov     eax, dword ptr [esi + 0x370]
        mov     dword ptr [edi + 0x370], eax
        mov     eax, dword ptr [esi + 0x3c0]
        mov     dword ptr [edi + 0x3c0], eax
        mov     eax, dword ptr [esi + 0x410]
        mov     dword ptr [edi + 0x410], eax
        mov     eax, dword ptr [esi + 0x460]
        mov     dword ptr [edi + 0x460], eax
        mov     eax, dword ptr [esi + 0x4b0]
        mov     dword ptr [edi + 0x4b0], eax
        add     esi, 4                           ; next column
        add     edi, 4
        inc     ebx                              ; walk the gate pointer
        dec     ecx
        jne     p0_loop
        sub     edi, 0x40                        ; rewind to region left edge
        sub     esi, 0x40
p1_setup:
; --- plane 1 ---
        mov     ax, 0x202                        ; Map Mask <- plane 1
        out     dx, ax
        mov     ecx, 0x10
        mov     ebx, dword ptr [ebp + 8]
        cmp     byte ptr [ebx], 0
        jle     p2_setup
p1_loop:
        mov     eax, dword ptr [esi + 0x7d00]    ; plane-1 slice (+0x7d00)
        mov     dword ptr [edi], eax
        mov     eax, dword ptr [esi + 0x7d50]
        mov     dword ptr [edi + 0x50], eax
        mov     eax, dword ptr [esi + 0x7da0]
        mov     dword ptr [edi + 0xa0], eax
        mov     eax, dword ptr [esi + 0x7df0]
        mov     dword ptr [edi + 0xf0], eax
        mov     eax, dword ptr [esi + 0x7e40]
        mov     dword ptr [edi + 0x140], eax
        mov     eax, dword ptr [esi + 0x7e90]
        mov     dword ptr [edi + 0x190], eax
        mov     eax, dword ptr [esi + 0x7ee0]
        mov     dword ptr [edi + 0x1e0], eax
        mov     eax, dword ptr [esi + 0x7f30]
        mov     dword ptr [edi + 0x230], eax
        mov     eax, dword ptr [esi + 0x7f80]
        mov     dword ptr [edi + 0x280], eax
        mov     eax, dword ptr [esi + 0x7fd0]
        mov     dword ptr [edi + 0x2d0], eax
        mov     eax, dword ptr [esi + 0x8020]
        mov     dword ptr [edi + 0x320], eax
        mov     eax, dword ptr [esi + 0x8070]
        mov     dword ptr [edi + 0x370], eax
        mov     eax, dword ptr [esi + 0x80c0]
        mov     dword ptr [edi + 0x3c0], eax
        mov     eax, dword ptr [esi + 0x8110]
        mov     dword ptr [edi + 0x410], eax
        mov     eax, dword ptr [esi + 0x8160]
        mov     dword ptr [edi + 0x460], eax
        mov     eax, dword ptr [esi + 0x81b0]
        mov     dword ptr [edi + 0x4b0], eax
        add     esi, 4
        add     edi, 4
        inc     ebx
        dec     ecx
        jne     p1_loop
        sub     edi, 0x40
        sub     esi, 0x40
p2_setup:
; --- plane 2 ---
        mov     ax, 0x402                        ; Map Mask <- plane 2
        out     dx, ax
        mov     ecx, 0x10
        mov     ebx, dword ptr [ebp + 8]
        cmp     byte ptr [ebx], 0
        jle     p3_setup
p2_loop:
        mov     eax, dword ptr [esi + 0xfa00]    ; plane-2 slice (+0xfa00)
        mov     dword ptr [edi], eax
        mov     eax, dword ptr [esi + 0xfa50]
        mov     dword ptr [edi + 0x50], eax
        mov     eax, dword ptr [esi + 0xfaa0]
        mov     dword ptr [edi + 0xa0], eax
        mov     eax, dword ptr [esi + 0xfaf0]
        mov     dword ptr [edi + 0xf0], eax
        mov     eax, dword ptr [esi + 0xfb40]
        mov     dword ptr [edi + 0x140], eax
        mov     eax, dword ptr [esi + 0xfb90]
        mov     dword ptr [edi + 0x190], eax
        mov     eax, dword ptr [esi + 0xfbe0]
        mov     dword ptr [edi + 0x1e0], eax
        mov     eax, dword ptr [esi + 0xfc30]
        mov     dword ptr [edi + 0x230], eax
        mov     eax, dword ptr [esi + 0xfc80]
        mov     dword ptr [edi + 0x280], eax
        mov     eax, dword ptr [esi + 0xfcd0]
        mov     dword ptr [edi + 0x2d0], eax
        mov     eax, dword ptr [esi + 0xfd20]
        mov     dword ptr [edi + 0x320], eax
        mov     eax, dword ptr [esi + 0xfd70]
        mov     dword ptr [edi + 0x370], eax
        mov     eax, dword ptr [esi + 0xfdc0]
        mov     dword ptr [edi + 0x3c0], eax
        mov     eax, dword ptr [esi + 0xfe10]
        mov     dword ptr [edi + 0x410], eax
        mov     eax, dword ptr [esi + 0xfe60]
        mov     dword ptr [edi + 0x460], eax
        mov     eax, dword ptr [esi + 0xfeb0]
        mov     dword ptr [edi + 0x4b0], eax
        add     esi, 4
        add     edi, 4
        inc     ebx
        dec     ecx
        jne     p2_loop
        sub     edi, 0x40
        sub     esi, 0x40
p3_setup:
; --- plane 3 ---
        mov     ax, 0x802                        ; Map Mask <- plane 3
        out     dx, ax
        mov     ecx, 0x10
        mov     ebx, dword ptr [ebp + 8]
        cmp     byte ptr [ebx], 0
        jle     after_planes
        dec     byte ptr [ebx]                   ; count the gate down once per call
p3_loop:
        mov     eax, dword ptr [esi + 0x17700]   ; plane-3 slice (+0x17700)
        mov     dword ptr [edi], eax
        mov     eax, dword ptr [esi + 0x17750]
        mov     dword ptr [edi + 0x50], eax
        mov     eax, dword ptr [esi + 0x177a0]
        mov     dword ptr [edi + 0xa0], eax
        mov     eax, dword ptr [esi + 0x177f0]
        mov     dword ptr [edi + 0xf0], eax
        mov     eax, dword ptr [esi + 0x17840]
        mov     dword ptr [edi + 0x140], eax
        mov     eax, dword ptr [esi + 0x17890]
        mov     dword ptr [edi + 0x190], eax
        mov     eax, dword ptr [esi + 0x178e0]
        mov     dword ptr [edi + 0x1e0], eax
        mov     eax, dword ptr [esi + 0x17930]
        mov     dword ptr [edi + 0x230], eax
        mov     eax, dword ptr [esi + 0x17980]
        mov     dword ptr [edi + 0x280], eax
        mov     eax, dword ptr [esi + 0x179d0]
        mov     dword ptr [edi + 0x2d0], eax
        mov     eax, dword ptr [esi + 0x17a20]
        mov     dword ptr [edi + 0x320], eax
        mov     eax, dword ptr [esi + 0x17a70]
        mov     dword ptr [edi + 0x370], eax
        mov     eax, dword ptr [esi + 0x17ac0]
        mov     dword ptr [edi + 0x3c0], eax
        mov     eax, dword ptr [esi + 0x17b10]
        mov     dword ptr [edi + 0x410], eax
        mov     eax, dword ptr [esi + 0x17b60]
        mov     dword ptr [edi + 0x460], eax
        mov     eax, dword ptr [esi + 0x17bb0]
        mov     dword ptr [edi + 0x4b0], eax
        add     esi, 4
        add     edi, 4
        inc     ebx
        dec     ecx
        jne     p3_loop
        sub     edi, 0x40
        sub     esi, 0x40
after_planes:
        jmp     done

; --- shared render-mode tail (non-planar modes do nothing extra) ---
mode_tail:
        test    byte ptr [0x105], 4              ; discard-layer mode?
        je      try_bit0
        jmp     done
try_bit0:
        test    byte ptr [0x105], 1              ; 8bpp mode?
        je      done
done:
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        ret
