; iso_cell_dispatch @ 0x47a7e  (140 bytes) -- hand-written assembly (fully commented).
;
; iso_cell_dispatch: isometric tile-cell dispatch. Given a packed screen coordinate it
; works out which map cell the point lands in, and which quarter of that diamond-
; shaped cell, then jumps to one of four handlers through a jump table.
;
; The argument is a fixed-point coordinate: the high byte of each 16-bit value is the
; whole tile index, the low byte is the fractional position inside the tile (0..255).
; A tile row is 128 cells wide (shl bx,7), so tileY*128 + tileX is the linear cell
; number; times 4 indexes a dword-per-cell table based at g_map_cols (0x5358).
;
; The quarter is chosen from the two fractional bytes (fx = cx, fy = dx): an
; isometric cell is a diamond, and the two tests (fx+fy vs 256, then fx vs fy) split
; it into the four triangles. The result 0/4/8/12 selects the handler in the table at
; 0x3a3a7. (Only runs when render-mode bit 1 is set; other modes jump to a shared
; exit that lives outside this listing.)
;
; Args (stack / cdecl):
;   [ebp+8]    packed x  (high byte tileX, low byte fractional x)
;   [ebp+0xc]  packed y  (high byte tileY, low byte fractional y)
; Globals:
;   0x105      render-mode flags
;   0x5358     g_map_cols -- dword-per-cell table
;   0x3a3a7    4-entry jump table of tile-quarter handlers
; The four dword table entries follow the code and the disassembler decodes them as
; stray instructions; they are data, not executable, and are marked below.
;
iso_cell_dispatch:
        push    ebp
        mov     ebp, esp
        push    ebx                              ; save caller regs whole (asm convention)
        push    ecx
        push    edx
        push    esi
        push    edi
        test    byte ptr [0x105], 2              ; planar render mode?
        je      0x49862                          ; no -> shared render-mode exit (outside this listing)
        mov     ax, word ptr [ebp + 8]           ; ax = packed x
        mov     bx, word ptr [ebp + 0xc]         ; bx = packed y
        mov     cx, ax                           ; keep a copy of packed x
        mov     dx, bx                           ; keep a copy of packed y
        sar     ax, 8                            ; ax = tileX  (high byte)
        sar     bx, 8                            ; bx = tileY  (high byte)
        shl     bx, 7                            ; tileY * 128 (cells per row)
        mov     si, ax                           ; si = tileX
        add     si, bx                           ;    + tileY*128  -> linear cell number
        and     esi, 0xffff                      ; keep it a 16-bit index
        shl     esi, 2                            ; * 4  -> dword offset into the cell table
        and     cx, 0xff                          ; cx = fractional x (fx, low byte)
        and     dx, 0xff                          ; dx = fractional y (fy, low byte)
        mov     bx, dx                           ; bx = fy
        add     bx, cx                           ;    + fx  -> fx+fy
        mov     eax, 0                           ; eax = handler index, start at 0
        cmp     bx, 0x100                        ; fx+fy >= 256 ? (lower half of diamond)
        jl      skip_add8
        add     eax, 8                           ;   yes -> +8
skip_add8:
        cmp     cx, dx                           ; fx < fy ? (left/right of diamond)
        jl      skip_add4
        add     eax, 4                           ;   fx >= fy -> +4
skip_add4:
        add     esi, dword ptr [0x5358]          ; esi = &g_map_cols[cell]
        jmp     dword ptr [eax + 0x3a3a7]        ; dispatch to the chosen tile-quarter handler

; ---- jump table data at 0x3a3a7 (four dword handler addresses); the disassembler
; ---- mis-decodes these bytes as instructions -- they are NOT executed here.
        lds     esp, ptr [ebx - 0x4d88fffd]      ; (data)
        add     eax, dword ptr [eax]             ; (data)
        mov     bh, 0xa3                         ; (data)
        add     eax, dword ptr [eax]             ; (data)
        ret     0x3a3                            ; (data)
        add     byte ptr [ecx + 0x200c6], al     ; (data)
        add     cl, ch                           ; (data)
        mov     ch, 0xe                          ; (data)
        add     byte ptr [eax], al               ; (data)
