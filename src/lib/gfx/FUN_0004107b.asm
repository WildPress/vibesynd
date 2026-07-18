; FUN_0004107b @ 0004107b  (95 bytes) -- hand-written assembly (fully commented).
;
; Turn a fixed-point (x, y) into a cell/tile address and an iso-triangle selector,
; then tail-jump to one of four draw handlers. Called by the region walker
; FUN_00040f4e once per strip.
;
; The coordinates come in as 16-bit fixed-point with 8 fractional bits (256 units to
; a tile). The integer parts pick the tile; the fractional parts (xf, yf) say which of
; the four triangles of the iso diamond the point sits in:
;
;       index 0  : xf + yf <  256  and  xf <  yf
;       index 4  : xf + yf <  256  and  xf >= yf
;       index 8  : xf + yf >= 256  and  xf <  yf
;       index 12 : xf + yf >= 256  and  xf >= yf
;
; The selected handler is [index + 0x33992] (a 4-entry jump table of code pointers).
; esi is handed to the handler as the cell/tile offset: tile_index*4 plus the base
; value at 0x5358 (labelled g_map_cols; used here as the cell-array base).
;
; Args (stack / cdecl):
;   [ebp+8]   x   (16-bit fixed-point, 8 fractional bits)
;   [ebp+0xc] y
; Reads:   0x5358  cell-array base   0x33992  4-entry handler jump table
;
; No epilogue of its own: it jumps into the handler, which returns to the caller.
; The build uses FUN_0004107b.c (a db-transcription of the raw bytes); this .asm is
; the readable companion. See docs/game-vs-library.md.
;
FUN_0004107b:
        push    ebp
        mov     ebp, esp
        mov     ax, word ptr [ebp + 8]           ; ax = x
        mov     bx, word ptr [ebp + 0xc]         ; bx = y
        mov     cx, ax                           ; keep a copy of x
        mov     dx, bx                           ; keep a copy of y
        sar     ax, 8                            ; ax = x >> 8   (tile column)
        sar     bx, 8                            ; bx = y >> 8   (tile row)
        shl     bx, 7                            ; bx = row * 128 (128 tiles per row)
        mov     si, ax                           ; si  = column
        add     si, bx                           ; si  = row*128 + column  (tile index)
        and     esi, 0xffff                      ; keep the 16-bit index
        shl     esi, 2                            ; esi = tile index * 4 (dword stride)
        and     cx, 0xff                          ; cx = xf (x fraction, 0..255)
        and     dx, 0xff                          ; dx = yf (y fraction, 0..255)
        mov     bx, dx                           ; bx = yf
        add     bx, cx                           ; bx = xf + yf
        mov     eax, 0                           ; selector = 0
        cmp     bx, 0x100                        ; xf + yf >= 256 ?
        jl      lower_half                       ;   no  -> stay in the near half
        add     eax, 8                           ;   yes -> +8 (far half)
lower_half:
        cmp     cx, dx                           ; xf < yf ?
        jl      have_index                       ;   yes -> leave selector as is
        add     eax, 4                           ;   no  -> +4 (right triangle)
have_index:
        add     esi, dword ptr [0x5358]          ; esi = tile*4 + cell-array base (0x5358)
        jmp     dword ptr [eax + 0x33992]        ; tail-jump to handler [selector]
