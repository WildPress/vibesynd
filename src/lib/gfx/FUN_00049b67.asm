; FUN_00049b67 @ 0x49b67  (886 bytes) -- hand-written assembly (fully commented).
;
; FUN_00049b67: copy one animated 16x16-pixel cell from the back buffer to the screen
; buffer, all four planes. Only runs in planar (mode-X) render mode.
;
; The source position scrolls over time. The animation phase is held in two globals,
; g_marker_phase_a (0x10ab4) and g_marker_phase_b (0x10ab8), added to the requested
; cell x/y so the sampled patch drifts frame to frame -- the look of a small looping
; texture (water / a blinking map marker). The source x is masked to 4 bits (& 0xf,
; then *4 bytes) and the source y wraps at 0x19 (25) rows before scaling by 0x500
; (one 16-row band), so the source is a 16-wide, 25-tall tiling patch inside
; g_back_buf. The destination is g_screen_buf at (x*4, y*0x500) + 0x10.
;
; The copy is fully unrolled: 16 rows (offsets 0,0x50,..,0x4b0) x 4 planes (offsets
; 0,0x7d00,0xfa00,0x17700), one dword (4 px/plane -> 16 px across) per row per plane.
;
; Args (stack / cdecl):
;   [ebp+8]   cell x     [ebp+0xc]  cell y
; Globals:
;   0x105    render-mode flags
;   0x10ab4  g_marker_phase_a   0x10ab8  g_marker_phase_b   (animation offsets)
;   0x5370   g_back_buf (source)     0x5368  g_screen_buf (dest)
;
FUN_00049b67:
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
; --- compute animated source pointer in g_back_buf ---
        mov     eax, dword ptr [ebp + 8]         ; eax = cell x
        add     eax, dword ptr [0x10ab8]         ; + g_marker_phase_b
        and     eax, 0xf                         ; wrap x to 16
        shl     eax, 2                            ; * 4 bytes
        mov     ebx, dword ptr [ebp + 0xc]       ; ebx = cell y
        add     ebx, dword ptr [0x10ab4]         ; + g_marker_phase_a
        cmp     ebx, 0x19                        ; y >= 25 ?
        jl      src_y_ok
        sub     ebx, 0x19                        ;   wrap y by 25 rows
src_y_ok:
        imul    ebx, ebx, 0x500                  ; y * 0x500 (one 16-row band)
        add     eax, ebx                         ; source offset
        mov     esi, dword ptr [0x5370]          ; g_back_buf
        add     esi, eax                         ; esi = source pixel
; --- compute destination pointer in g_screen_buf ---
        mov     eax, dword ptr [ebp + 8]         ; eax = cell x
        shl     eax, 2                            ; * 4
        imul    ebx, dword ptr [ebp + 0xc], 0x500 ; y * 0x500
        add     eax, ebx
        add     eax, 0x10                        ; + 0x10 (dest bias)
        mov     edi, dword ptr [0x5368]          ; g_screen_buf
        add     edi, eax                         ; edi = dest pixel
; --- unrolled 16-row x 4-plane copy (plane offsets 0,0x7d00,0xfa00,0x17700) ---
        mov     eax, dword ptr [esi]             ; row 0
        mov     dword ptr [edi], eax
        mov     eax, dword ptr [esi + 0x7d00]
        mov     dword ptr [edi + 0x7d00], eax
        mov     eax, dword ptr [esi + 0xfa00]
        mov     dword ptr [edi + 0xfa00], eax
        mov     eax, dword ptr [esi + 0x17700]
        mov     dword ptr [edi + 0x17700], eax
        mov     eax, dword ptr [esi + 0x50]      ; row 1
        mov     dword ptr [edi + 0x50], eax
        mov     eax, dword ptr [esi + 0x7d50]
        mov     dword ptr [edi + 0x7d50], eax
        mov     eax, dword ptr [esi + 0xfa50]
        mov     dword ptr [edi + 0xfa50], eax
        mov     eax, dword ptr [esi + 0x17750]
        mov     dword ptr [edi + 0x17750], eax
        mov     eax, dword ptr [esi + 0xa0]      ; row 2
        mov     dword ptr [edi + 0xa0], eax
        mov     eax, dword ptr [esi + 0x7da0]
        mov     dword ptr [edi + 0x7da0], eax
        mov     eax, dword ptr [esi + 0xfaa0]
        mov     dword ptr [edi + 0xfaa0], eax
        mov     eax, dword ptr [esi + 0x177a0]
        mov     dword ptr [edi + 0x177a0], eax
        mov     eax, dword ptr [esi + 0xf0]      ; row 3
        mov     dword ptr [edi + 0xf0], eax
        mov     eax, dword ptr [esi + 0x7df0]
        mov     dword ptr [edi + 0x7df0], eax
        mov     eax, dword ptr [esi + 0xfaf0]
        mov     dword ptr [edi + 0xfaf0], eax
        mov     eax, dword ptr [esi + 0x177f0]
        mov     dword ptr [edi + 0x177f0], eax
        mov     eax, dword ptr [esi + 0x140]     ; row 4
        mov     dword ptr [edi + 0x140], eax
        mov     eax, dword ptr [esi + 0x7e40]
        mov     dword ptr [edi + 0x7e40], eax
        mov     eax, dword ptr [esi + 0xfb40]
        mov     dword ptr [edi + 0xfb40], eax
        mov     eax, dword ptr [esi + 0x17840]
        mov     dword ptr [edi + 0x17840], eax
        mov     eax, dword ptr [esi + 0x190]     ; row 5
        mov     dword ptr [edi + 0x190], eax
        mov     eax, dword ptr [esi + 0x7e90]
        mov     dword ptr [edi + 0x7e90], eax
        mov     eax, dword ptr [esi + 0xfb90]
        mov     dword ptr [edi + 0xfb90], eax
        mov     eax, dword ptr [esi + 0x17890]
        mov     dword ptr [edi + 0x17890], eax
        mov     eax, dword ptr [esi + 0x1e0]     ; row 6
        mov     dword ptr [edi + 0x1e0], eax
        mov     eax, dword ptr [esi + 0x7ee0]
        mov     dword ptr [edi + 0x7ee0], eax
        mov     eax, dword ptr [esi + 0xfbe0]
        mov     dword ptr [edi + 0xfbe0], eax
        mov     eax, dword ptr [esi + 0x178e0]
        mov     dword ptr [edi + 0x178e0], eax
        mov     eax, dword ptr [esi + 0x230]     ; row 7
        mov     dword ptr [edi + 0x230], eax
        mov     eax, dword ptr [esi + 0x7f30]
        mov     dword ptr [edi + 0x7f30], eax
        mov     eax, dword ptr [esi + 0xfc30]
        mov     dword ptr [edi + 0xfc30], eax
        mov     eax, dword ptr [esi + 0x17930]
        mov     dword ptr [edi + 0x17930], eax
        mov     eax, dword ptr [esi + 0x280]     ; row 8
        mov     dword ptr [edi + 0x280], eax
        mov     eax, dword ptr [esi + 0x7f80]
        mov     dword ptr [edi + 0x7f80], eax
        mov     eax, dword ptr [esi + 0xfc80]
        mov     dword ptr [edi + 0xfc80], eax
        mov     eax, dword ptr [esi + 0x17980]
        mov     dword ptr [edi + 0x17980], eax
        mov     eax, dword ptr [esi + 0x2d0]     ; row 9
        mov     dword ptr [edi + 0x2d0], eax
        mov     eax, dword ptr [esi + 0x7fd0]
        mov     dword ptr [edi + 0x7fd0], eax
        mov     eax, dword ptr [esi + 0xfcd0]
        mov     dword ptr [edi + 0xfcd0], eax
        mov     eax, dword ptr [esi + 0x179d0]
        mov     dword ptr [edi + 0x179d0], eax
        mov     eax, dword ptr [esi + 0x320]     ; row 10
        mov     dword ptr [edi + 0x320], eax
        mov     eax, dword ptr [esi + 0x8020]
        mov     dword ptr [edi + 0x8020], eax
        mov     eax, dword ptr [esi + 0xfd20]
        mov     dword ptr [edi + 0xfd20], eax
        mov     eax, dword ptr [esi + 0x17a20]
        mov     dword ptr [edi + 0x17a20], eax
        mov     eax, dword ptr [esi + 0x370]     ; row 11
        mov     dword ptr [edi + 0x370], eax
        mov     eax, dword ptr [esi + 0x8070]
        mov     dword ptr [edi + 0x8070], eax
        mov     eax, dword ptr [esi + 0xfd70]
        mov     dword ptr [edi + 0xfd70], eax
        mov     eax, dword ptr [esi + 0x17a70]
        mov     dword ptr [edi + 0x17a70], eax
        mov     eax, dword ptr [esi + 0x3c0]     ; row 12
        mov     dword ptr [edi + 0x3c0], eax
        mov     eax, dword ptr [esi + 0x80c0]
        mov     dword ptr [edi + 0x80c0], eax
        mov     eax, dword ptr [esi + 0xfdc0]
        mov     dword ptr [edi + 0xfdc0], eax
        mov     eax, dword ptr [esi + 0x17ac0]
        mov     dword ptr [edi + 0x17ac0], eax
        mov     eax, dword ptr [esi + 0x410]     ; row 13
        mov     dword ptr [edi + 0x410], eax
        mov     eax, dword ptr [esi + 0x8110]
        mov     dword ptr [edi + 0x8110], eax
        mov     eax, dword ptr [esi + 0xfe10]
        mov     dword ptr [edi + 0xfe10], eax
        mov     eax, dword ptr [esi + 0x17b10]
        mov     dword ptr [edi + 0x17b10], eax
        mov     eax, dword ptr [esi + 0x460]     ; row 14
        mov     dword ptr [edi + 0x460], eax
        mov     eax, dword ptr [esi + 0x8160]
        mov     dword ptr [edi + 0x8160], eax
        mov     eax, dword ptr [esi + 0xfe60]
        mov     dword ptr [edi + 0xfe60], eax
        mov     eax, dword ptr [esi + 0x17b60]
        mov     dword ptr [edi + 0x17b60], eax
        mov     eax, dword ptr [esi + 0x4b0]     ; row 15
        mov     dword ptr [edi + 0x4b0], eax
        mov     eax, dword ptr [esi + 0x81b0]
        mov     dword ptr [edi + 0x81b0], eax
        mov     eax, dword ptr [esi + 0xfeb0]
        mov     dword ptr [edi + 0xfeb0], eax
        mov     eax, dword ptr [esi + 0x17bb0]
        mov     dword ptr [edi + 0x17bb0], eax
        pop     edi                              ; planar path returns here
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        ret

; --- shared render-mode tail (non-planar modes do nothing extra) ---
        jmp     done                             ; (unreached: prior ret exits the planar path)
mode_tail:
        test    byte ptr [0x105], 4              ; discard-layer mode?
        je      try_bit0
        jmp     done
try_bit0:
        test    byte ptr [0x105], 1              ; 8bpp mode?
        je      done
done:
        pop     edi                              ; epilogue (shared with sibling routines)
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
