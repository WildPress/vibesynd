; FUN_00040f4e @ 00040f4e  (301 bytes) -- hand-written assembly (fully commented).
;
; Rectangle/region draw setup. Given a bounding box in fixed-point, it works out how
; many tile columns the region spans and then walks that row of columns, dispatching
; each one through the tile router FUN_0004107b. It appears to draw one diagonal run
; of columns, handing the column count's companion "rows" value to each dispatched
; draw as a height, so the net effect is a filled rectangular block of tiles.
;
; Only runs when render-mode bit 1 (0x105) is set; otherwise it falls through the
; usual bit2/bit0 tail to the epilogue. Saves every register.
;
; The two axes arrive at different fixed-point scales: the args at [ebp+0x1c]/[ebp+0x24]
; are shifted right by 5, and [ebp+0x20]/[ebp+0x28] by 4, to get tile counts. From
; those it forms:
;   rows = (arg24>>5) - (arg1c>>5) + 1      -> local [ebp-0xa]
;   cols = (arg28>>4) - (arg20>>4) + 1      -> local [ebp-0xc]
;   [ebp-4] = (arg1c>>5) * 4                -> x base (dword)
;   [ebp-8] = (arg20>>4) * 0x500            -> planar band base (dword), 16-row band
;
; The column loop keeps a running screen (x, y) in the word slots [ebp+8]/[ebp+0x10],
; stepping both by 0x80 (half a tile) each column and the band offset [ebp-8] by 0x500.
; A per-column byte is read from the table at 0xa7dc (indexed by the down-counting edi)
; and passed along. Columns whose band offset is out of range [0, 0x7d00) are skipped
; (off the planar buffer). Each drawn column calls FUN_0004107b with, on the stack:
; x, y, rows(height), the table byte, the x base, the band offset, and edi.
;
; Args (stack / cdecl, mixed byte/word):
;   [ebp+8]  x start (byte)      [ebp+0x10] y start (byte)
;   [ebp+0xc] x step             [ebp+0x14] y step
;   [ebp+0x18] a span/height     [ebp+0x1c],[ebp+0x24] box in >>5 units (rows axis)
;   [ebp+0x20],[ebp+0x28] box in >>4 units (cols axis)
; Locals:  [ebp-4] x base   [ebp-8] band base   [ebp-0xa] rows   [ebp-0xc] cols
; Reads:   0x105 render flags   0xa7dc per-column byte table   calls FUN_0004107b
;
; The build uses FUN_00040f4e.c (a db-transcription of the raw bytes); this .asm is
; the readable companion. See docs/game-vs-library.md.
;
FUN_00040f4e:
        push    ebp
        mov     ebp, esp
        add     esp, -0xc                        ; reserve 12 bytes of locals
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        test    byte ptr [0x105], 2              ; render-mode bit 1 set?
        je      tail_modes                       ;   no -> bit2/bit0 tail dispatch
        mov     ax, word ptr [ebp + 0x1c]
        sar     ax, 5                            ; ax = arg1c >> 5 (top row)
        mov     cx, word ptr [ebp + 0x24]
        sar     cx, 5                            ; cx = arg24 >> 5 (bottom row)
        mov     bx, word ptr [ebp + 0x20]
        sar     bx, 4                            ; bx = arg20 >> 4 (left col)
        mov     dx, word ptr [ebp + 0x28]
        sar     dx, 4                            ; dx = arg28 >> 4 (right col)
        sub     cx, ax
        inc     cx                               ; cx = row count
        mov     word ptr [ebp - 0xa], cx         ; rows -> [ebp-0xa]
        sub     dx, bx
        inc     dx                               ; dx = column count
        mov     word ptr [ebp - 0xc], dx         ; cols -> [ebp-0xc]
        movsx   eax, ax                          ; sign-extend top row
        movsx   ebx, bx                          ; sign-extend left col
        shl     eax, 2                            ; eax = top row * 4
        imul    ebx, ebx, 0x500                  ; ebx = left col * 0x500 (band offset)
        mov     dword ptr [ebp - 4], eax         ; x base -> [ebp-4]
        mov     dword ptr [ebp - 8], ebx         ; band base -> [ebp-8]
        mov     bx, word ptr [ebp - 0xc]         ; bx = cols
        movzx   edi, bx
        inc     edi                              ; edi = cols + 1 (table cursor)
        mov     ax, word ptr [ebp + 0x18]
        shl     bx, 7                            ; bx = cols * 128
        add     bx, ax                           ;   + arg18
        sub     bx, 0x100                        ;   - 256  -> wrap-back distance
        movzx   cx, byte ptr [ebp + 8]           ; cx = x start
        movzx   dx, byte ptr [ebp + 0x10]        ; dx = y start
        add     cx, dx                           ; x + y
        cmp     cx, 0x100                        ; would the run cross the tile seam?
        jl      no_wrap                          ;   no
        dec     edi                              ; wrap: one fewer column
no_wrap:
        sub     word ptr [ebp + 8], bx           ; pull x back by the wrap distance
        sub     word ptr [ebp + 0x10], bx        ; pull y back by the wrap distance
        add     ax, 0x7f
        sar     ax, 7                            ; ax = (arg18 + 127) / 128 (tiles)
        shl     ax, 4                            ; ax *= 16
        add     di, ax                           ; bump the table cursor
        mov     ax, word ptr [ebp + 0xc]         ; ax = x step
        mov     bx, word ptr [ebp + 0x14]        ; bx = y step
        add     word ptr [ebp + 8], ax           ; prime x by one step
        add     word ptr [ebp + 0x10], bx        ; prime y by one step
column_loop:
        movzx   ax, byte ptr [edi + 0xa7dc]      ; per-column byte from table 0xa7dc[edi]
        dec     edi                              ; step the table cursor back
        cmp     dword ptr [ebp - 8], 0           ; band offset below the buffer?
        jl      next_column                      ;   yes -> skip this column's draw
        cmp     dword ptr [ebp - 8], 0x7d00      ; band offset past the plane?
        jge     epilogue                         ;   yes -> done
        push    edi                              ; --- args for FUN_0004107b / its handler ---
        push    dword ptr [ebp - 8]              ; band offset
        push    dword ptr [ebp - 4]              ; x base
        sub     esp, 2
        push    ax                               ; table byte
        sub     esp, 2
        push    word ptr [ebp - 0xa]             ; rows (height)
        sub     esp, 2
        push    word ptr [ebp + 0x10]            ; y  (-> callee [ebp+0xc])
        sub     esp, 2
        push    word ptr [ebp + 8]               ; x  (-> callee [ebp+8])
        call    0x4107b                          ; -> FUN_0004107b (route to quadrant draw)
        add     esp, 0x18                        ; drop the pushed args
        pop     edi                              ; restore table cursor
next_column:
        add     word ptr [ebp + 8], 0x80         ; x += half a tile
        add     word ptr [ebp + 0x10], 0x80      ; y += half a tile
        add     dword ptr [ebp - 8], 0x500       ; band offset += one 16-row band
        dec     word ptr [ebp - 0xc]             ; cols--
        jg      column_loop                      ;   more columns -> loop
        jmp     epilogue

tail_modes:                                      ; render bit1 clear: match the other draws
        test    byte ptr [0x105], 4              ; bit 2 set -> discard
        je      mode_bit0
        jmp     epilogue
mode_bit0:
        test    byte ptr [0x105], 1              ; bit 0 set -> (nothing extra here)
        je      epilogue
epilogue:
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        ret
