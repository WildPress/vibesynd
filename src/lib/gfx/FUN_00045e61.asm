; FUN_00045e61 @ 00045e61  (151 bytes) -- hand-written assembly (fully commented).
;
; Same tile-address-and-triangle dispatch as FUN_0004107b, but for a 2-unit tile grid
; and gated by the render mode. It maps (x, y) to a cell offset and an iso-triangle
; selector, scales two block-size stack arguments the way the planar blitters expect,
; then tail-jumps to one of four quadrant draw handlers.
;
; Coordinate maths (note the /2 and &1 in place of FUN_0004107b's /256 and &255, so a
; "tile" here is 2 units):
;   tile column = x >> 1,  tile row = y >> 1,  index = row*128 + column
;   esi = index * 4 + [0x5358]                 (cell-array base at 0x5358)
;   xf = x & 1,  yf = y & 1
;   selector = 0; if xf+yf >= 2 selector += 8; if xf >= yf selector += 4  -> {0,4,8,12}
;
; When render-mode bit 1 (0x105) is set it also rewrites two stack args in place into
; the planar block form used by blit_block: arg[+0x18] *= 0x500 (one 16-row band),
; arg[+0x14] <<= 2 (dword -> byte x). It then dispatches through the 4-entry table at
; 0x38794. If bit 1 is clear it branches to the shared epilogue at 0x45f6e (register
; restore + return), which lies past the bytes shown here.
;
; Args (stack / cdecl):  [ebp+8] x   [ebp+0xc] y   [ebp+0x14], [ebp+0x18] block size
; Reads:  0x105 render-mode flags   0x5358 cell-array base   0x38794 handler table
; Saves every register on entry (the shared epilogue pops them).
;
; The build uses FUN_00045e61.c (a db-transcription of the raw bytes); this .asm is
; the readable companion. See docs/game-vs-library.md.
;
FUN_00045e61:
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
        je      0x45f6e                          ;   no -> shared epilogue (restore + ret)
        imul    ebx, dword ptr [ebp + 0x18], 0x500 ; arg[+0x18] *= 0x500 (16-row band)
        mov     dword ptr [ebp + 0x18], ebx      ; store it back
        shl     dword ptr [ebp + 0x14], 2        ; arg[+0x14] <<= 2 (dword -> byte x)
        add     esi, dword ptr [0x5358]          ; esi = tile*4 + cell-array base (0x5358)
        jmp     dword ptr [eax + 0x38794]        ; tail-jump to quadrant handler [selector]

; The bytes below are NOT a straight-line continuation. They decode (little-endian)
; to a 4-entry jump table -- pointers 0x000387b1, 0x000387ec, 0x000387a4, 0x000387ac,
; the quadrant handlers in the 0x387xx block -- followed by two short re-entry
; fragments (`add esi,0x200 / jmp 0x45f34 / add esi,4`). Kept verbatim (with raw
; bytes) so the listing stays a faithful disassembly of the region.
        mov     cl, 0x87                         ; b187
        add     eax, dword ptr [eax]             ; 0300
        in      al, dx                           ; ec
        xchg    dword ptr [ebx], eax             ; 8703
        add     byte ptr [edi + eax*4 - 0x7853fffd], ah ; 00a4870300ac87
        add     eax, dword ptr [eax]             ; 0300
        add     esi, 0x200                       ; 81c600020000
        jmp     0x45f34                          ; eb40
        add     esi, 4                           ; 83c604
