; plot_point @ 0x40236  (170 bytes) -- hand-written assembly (fully commented).
;
; plot_point: draw one point into the active overlay buffer, with viewport clipping.
; Two target buffers are supported, chosen by the render-mode flag byte at 0x105:
;   bit 1 set -> a 1-bit-per-pixel mask/overlay layer, 640 pixels wide. The
;                per-point operation (set / clear / toggle the bit) is chosen by
;                arg3 through a jump table, so the same routine draws into a mask
;                in several modes.
;   bit 0 set -> an 8-bit-per-pixel half-resolution buffer, 320 wide. arg3 is the
;                colour byte, written straight to the pixel.
;   bit 2 set -> discard the point (drawing is suppressed for this layer).
;
; Args (stack / cdecl):
;   [ebp+8]    x
;   [ebp+0xc]  y
;   [ebp+0x10] value   -- operation code for the 1bpp layer, colour for the 8bpp
; Globals:
;   0x5380 / 0x5384    viewport origin x / y (added after the clip test)
;   0x5388 / 0x538c    clip width / height
;   0x5368             g_screen_buf, the active buffer base
;   0x105              render-mode flags
;   0x32b9f            1bpp plot-operation jump table, indexed by value*4
;
; It saves and restores every register, including EAX/ECX/EDX, which a C compiler
; treats as scratch and never preserves. That save-everything shape is why this is
; hand asm and not compiler output. The build uses plot_point.c (the raw bytes as
; a db-transcription); this .asm is the readable companion. See docs/game-vs-library.md.

plot_point:
        push    ebp
        mov     ebp, esp
        push    eax                 ; save the caller's registers whole (asm convention)
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        mov     ecx, [ebp+8]        ; ecx = x
        mov     eax, [ebp+0xc]      ; eax = y
        cmp     ecx, 0
        jl      done                ; clip: drop if x < 0
        cmp     eax, 0
        jl      done                ; clip: drop if y < 0
        cmp     ecx, [0x5388]
        jge     done                ; clip: drop if x >= width
        cmp     eax, [0x538c]
        jge     done                ; clip: drop if y >= height
        add     ecx, [0x5380]       ; x += viewport origin x
        add     eax, [0x5384]       ; y += viewport origin y
        test    byte [0x105], 2
        je      try_8bpp            ; mode bit1 clear -> this isn't the 1bpp layer

; --- 1-bit-per-pixel mask layer ---
        mov     edx, [ebp+0x10]     ; edx = operation code (jump-table index)
        imul    esi, eax, 0x280     ; esi = y * 640          (bit index: 640 bits/row)
        add     esi, ecx            ;      + x               -> bit index
        shr     esi, 3              ;      / 8               -> byte index
        add     esi, [0x5368]       ;      + g_screen_buf    -> byte address
        and     cl, 7               ; cl = x & 7             -> bit within the byte
        mov     ax, 0x80
        shr     ax, cl              ; ax = set-mask   (0x80 >> (x & 7))
        mov     bx, ax
        not     bx                  ; bx = clear-mask (~set-mask)
        call    FUN_000402e0        ; jmp [edx*4 + 0x32b9f]: apply the chosen op to [esi]
        jmp     done

try_8bpp:
        test    byte [0x105], 4
        je      check_8bpp
        jmp     done                ; mode bit2 set -> discard the point

check_8bpp:
        test    byte [0x105], 1
        je      done                ; mode bit0 clear -> nothing to draw here

; --- 8-bit-per-pixel half-resolution buffer ---
        mov     eax, [ebp+8]
        sar     eax, 1              ; x / 2
        mov     ebx, [ebp+0xc]
        sar     ebx, 1              ; y / 2
        imul    ebx, ebx, 0x140     ; (y/2) * 320
        add     eax, ebx            ;   + x/2                -> byte offset
        add     eax, [0x5368]       ;   + g_screen_buf       -> pixel address
        mov     ebx, [ebp+0x10]     ; bl = colour byte
        mov     byte [eax], bl      ; write the pixel

done:
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        ret
