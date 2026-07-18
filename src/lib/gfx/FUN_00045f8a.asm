; FUN_00045f8a @ 00045f8a  (151 bytes) -- hand-written assembly (fully commented).
;
; The twin of FUN_00045e61: the same 2-unit tile-address-and-triangle dispatch, with
; its own handler table at 0x388bd and its own shared epilogue at 0x4607e. It maps
; (x, y) to a cell offset and an iso-triangle selector, scales two block-size stack
; arguments into planar form, then tail-jumps to one of four quadrant draw handlers.
;
; Coordinate maths (a "tile" is 2 units here):
;   tile column = x >> 1,  tile row = y >> 1,  index = row*128 + column
;   esi = index * 4 + [0x5358]                 (cell-array base at 0x5358)
;   xf = x & 1,  yf = y & 1
;   selector = 0; if xf+yf >= 2 selector += 8; if xf >= yf selector += 4  -> {0,4,8,12}
;
; When render-mode bit 1 (0x105) is set it rewrites two stack args in place into the
; planar block form used by blit_block: arg[+0x18] *= 0x500 (one 16-row band),
; arg[+0x14] <<= 2 (dword -> byte x). It then dispatches through the 4-entry table at
; 0x388bd. If bit 1 is clear it branches to the shared epilogue at 0x4607e (register
; restore + return), which lies past the bytes shown here.
;
; Args (stack / cdecl):  [ebp+8] x   [ebp+0xc] y   [ebp+0x14], [ebp+0x18] block size
; Reads:  0x105 render-mode flags   0x5358 cell-array base   0x388bd handler table
; Saves every register on entry (the shared epilogue pops them).
;
; The build uses FUN_00045f8a.c (a db-transcription of the raw bytes); this .asm is
; the readable companion. See docs/game-vs-library.md.
;
FUN_00045f8a:
        push    ebp
        mov     ebp, esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        mov     ax, word ptr [ebp + 8]           ; ax = x
        mov     bx, word ptr [ebp + 0xc]         ; bx = y
        mov     cx, ax                           ; copy of x
        mov     dx, bx                           ; copy of y
        sar     ax, 1                            ; ax = x >> 1 (tile column)
        sar     bx, 1                            ; bx = y >> 1 (tile row)
        shl     bx, 7                            ; bx = row * 128
        mov     si, ax                           ; si = column
        add     si, bx                           ; si = row*128 + column (tile index)
        and     esi, 0xffff                      ; keep 16-bit index
        shl     esi, 2                            ; esi = index * 4 (dword stride)
        and     cx, 1                            ; cx = xf (x & 1)
        and     dx, 1                            ; dx = yf (y & 1)
        mov     bx, dx                           ; bx = yf
        add     bx, cx                           ; bx = xf + yf
        mov     eax, 0                           ; selector = 0
        cmp     bx, 2                            ; xf + yf >= 2 ?
        jl      lower_half                       ;   no
        add     eax, 8                           ;   yes -> +8
lower_half:
        cmp     cx, dx                           ; xf < yf ?
        jl      have_index                       ;   yes
        add     eax, 4                           ;   no -> +4
have_index:
        test    byte ptr [0x105], 2              ; render-mode bit 1 set?
        je      0x4607e                          ;   no -> shared epilogue (restore + ret)
        imul    ebx, dword ptr [ebp + 0x18], 0x500 ; arg[+0x18] *= 0x500 (16-row band)
        mov     dword ptr [ebp + 0x18], ebx      ; store it back
        shl     dword ptr [ebp + 0x14], 2        ; arg[+0x14] <<= 2 (dword -> byte x)
        add     esi, dword ptr [0x5358]          ; esi = tile*4 + cell-array base (0x5358)
        jmp     dword ptr [eax + 0x388bd]        ; tail-jump to quadrant handler [selector]

; The bytes below are NOT a straight-line continuation. They decode (little-endian)
; to a 4-entry jump table of code pointers into the 0x388xx quadrant-handler block,
; followed by two short re-entry fragments (`add esi,0x200 / jmp 0x4604c / add esi,4`).
; Kept verbatim (with raw bytes) so the listing stays a faithful disassembly.
        fimul   dword ptr [eax - 0x76fbfffd]     ; da8803000489
        add     eax, dword ptr [eax]             ; 0300
        int     0x88                             ; cd88
        add     eax, dword ptr [eax]             ; 0300
        aad     0x88                             ; d588
        add     eax, dword ptr [eax]             ; 0300
        add     esi, 0x200                       ; 81c600020000
        jmp     0x4604c                          ; eb2f
        add     esi, 4                           ; 83c604
