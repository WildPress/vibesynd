; occlusion_walk @ 00041a44  (3642 bytes) -- hand-written assembly (fully commented).
;
; occlusion_walk: isometric scene visibility walker. It steps through a fixed diamond of
; 36 nearby map cells around a viewpoint, back-to-front, and for every drawable cell it
; folds that cell's occlusion/visibility mask into the running mask table at 0xe144.
; This looks like the pass that works out which stacked tiles hide which, before the
; actual sprites are drawn.
;
; esi points at a map cell-pointer array (each cell is a 4-byte pointer; the walk uses a
; row stride of 0x200 bytes and a column stride of 4). edx is a budget: the maximum
; number of cells to process this call. The walk is fully unrolled, one block per cell.
;
; Each cell block does the same thing (see the fully-commented first block below):
;   1. form the cell's slot offset (esi + a fixed per-cell offset), subtract g_map_cols
;      (0x5358) and reject it if the result is not in [0, 0xc000) -- i.e. off the map;
;   2. read a one-byte tile/height code from the cell record; codes <= 4 are skipped;
;   3. index the type->draw-data table at 0x5360 (stride 0x18) and fetch a mask-data
;      offset from a per-cell slot; a zero offset is skipped;
;   4. the first time a cell survives all that, clear the mask table once (clear_occlusion_mask);
;   5. OR the cell's inverted mask into the table (merge_cell_mask).
;
; After each processed cell it checks the folded column index (cx): if it has dropped to
; 0 or below the walk stops early. The per-cell tile field steps from +0xb down to +0,
; and the draw-data mask slot cycles through +0xc/+4/+0x14/+0x10/+8/+0 -- these pick the
; sub-layer and mask for each step across the diamond.
;
; Returns AX:  AL = 1 if any cell was processed (the mask table was cleared), else 0;
; AH = 0 if the walk finished or the edx budget ran out, AH = 1 if it stopped early on a
; cell whose column index reached 0. Uses one stack byte [ebp-1] as the did-any flag.
; Globals: 0x5358 g_map_cols, 0x5360 type->draw-data base, 0xe144 running mask table.
;
; The cell walk order (slot offset, tile sub-layer field, draw-data mask slot):
;
;   cell  1:  esi+0xc14       tile[+0xb]   mask[+0xc]
;   cell  2:  esi+0xa14       tile[+0xb]   mask[+4]
;   cell  3:  esi+0xa10       tile[+0xb]   mask[+0x14]
;   cell  4:  esi+0xa14       tile[+0xa]   mask[+0]
;   cell  5:  esi+0xa10       tile[+0xa]   mask[+0x10]
;   cell  6:  esi+0x810       tile[+0xa]   mask[+8]
;   cell  7:  esi+0xa10       tile[+9]   mask[+0xc]
;   cell  8:  esi+0x810       tile[+9]   mask[+4]
;   cell  9:  esi+0x80c       tile[+9]   mask[+0x14]
;   cell 10:  esi+0x810       tile[+8]   mask[+0]
;   cell 11:  esi+0x80c       tile[+8]   mask[+0x10]
;   cell 12:  esi+0x60c       tile[+8]   mask[+8]
;   cell 13:  esi+0x80c       tile[+7]   mask[+0xc]
;   cell 14:  esi+0x60c       tile[+7]   mask[+4]
;   cell 15:  esi+0x608       tile[+7]   mask[+0x14]
;   cell 16:  esi+0x60c       tile[+6]   mask[+0]
;   cell 17:  esi+0x608       tile[+6]   mask[+0x10]
;   cell 18:  esi+0x408       tile[+6]   mask[+8]
;   cell 19:  esi+0x608       tile[+5]   mask[+0xc]
;   cell 20:  esi+0x408       tile[+5]   mask[+4]
;   cell 21:  esi+0x404       tile[+5]   mask[+0x14]
;   cell 22:  esi+0x408       tile[+4]   mask[+0]
;   cell 23:  esi+0x404       tile[+4]   mask[+0x10]
;   cell 24:  esi+0x204       tile[+4]   mask[+8]
;   cell 25:  esi+0x404       tile[+3]   mask[+0xc]
;   cell 26:  esi+0x204       tile[+3]   mask[+4]
;   cell 27:  esi+0x200       tile[+3]   mask[+0x14]
;   cell 28:  esi+0x204       tile[+2]   mask[+0]
;   cell 29:  esi+0x200       tile[+2]   mask[+0x10]
;   cell 30:  esi+0           tile[+2]   mask[+8]
;   cell 31:  esi+0x200       tile[+1]   mask[+0xc]
;   cell 32:  esi+0           tile[+1]   mask[+4]
;   cell 33:  esi-4           tile[+1]   mask[+0x14]
;   cell 34:  esi+0           tile[+0]   mask[+0]
;   cell 35:  esi-4           tile[+0]   mask[+0x10]
;   cell 36:  esi+0xfffffdfc  tile[+0]   mask[+8]
;
; The build uses occlusion_walk.c (a db-transcription of the raw bytes); this .asm is the
; readable companion. See docs/game-vs-library.md.
;
occlusion_walk:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        add     esp, -4                          ; 83c4fc
        mov     byte ptr [ebp - 1], 0            ; c645ff00
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c1b0e0000

; ----- cell 1: esi+0xc14  tile[+0xb]  mask[+0xc] -----
        mov     ecx, esi                         ; ecx = cell-array cursor (esi)
        add     ecx, 0xc14                       ; point at this cell's map slot (cursor + offset)
        sub     ecx, dword ptr [0x5358]          ; - g_map_cols: fold to a 0-based on-map index
        cmp     ecx, 0xc000                      ; index still inside the map array (< 0xc000)?
        jge     0x41ab4                          ; no -> skip this cell, try the next
        cmp     ecx, 0                           ; index >= 0 ?
        jl      0x41ab4                          ; no -> skip this cell, try the next
        mov     ebx, dword ptr [esi + 0xc14]     ; ebx = this cell's record pointer
        movzx   ebx, byte ptr [ebx + 0xb]        ; read the tile/height code for this sub-layer
        cmp     ebx, 4                           ; codes 0..4 are empty/floor
        jle     0x41ab4                          ; -> nothing to occlude here, skip
        imul    ebx, ebx, 0x18                   ; index the type table (stride 0x18)
        add     ebx, dword ptr [0x5360]          ; + type->draw-data table base (0x5360)
        mov     ebx, dword ptr [ebx + 0xc]       ; fetch this layer's mask-data offset (draw-data slot)
        cmp     ebx, 0                           ; no mask for this layer?
        je      0x41ab4                          ; -> skip
        cmp     byte ptr [ebp - 1], 0            ; running mask table already cleared this call?
        jne     0x41a9b                          ; yes -> don't clear it again
        call    0x46188                          ; one-time: zero the mask table at 0xe144 (clear_occlusion_mask)
        add     byte ptr [ebp - 1], 1            ; mark it cleared (this byte is also return value AL)
        add     ebx, dword ptr [0x5360]          ; mask pointer = offset + draw-data base (0x5360)
        call    0x418ac                          ; OR this cell's inverted mask into the table (merge_cell_mask)
        cmp     cx, 0                            ; column index (low word) still ahead of the view?
        jle     0x42877                          ; no -> stop, return AH=1
        dec     edx                              ; cell budget--
        jl      0x42870                          ; budget exhausted -> stop, return AH=0

; ----- cell 2: esi+0xa14  tile[+0xb]  mask[+4] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x41b1a                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x41b1a                          ; 7c44
        mov     ebx, dword ptr [esi + 0xa14]     ; 8b9e140a0000
        movzx   ebx, byte ptr [ebx + 0xb]        ; 0fb65b0b
        cmp     ebx, 4                           ; 83fb04
        jle     0x41b1a                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x41b1a                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x41b01                          ; 7505
        call    0x46188                          ; e887460000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e89cfdffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e5d0d0000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c4f0d0000

; ----- cell 3: esi+0xa10  tile[+0xb]  mask[+0x14] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x41b80                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x41b80                          ; 7c44
        mov     ebx, dword ptr [esi + 0xa10]     ; 8b9e100a0000
        movzx   ebx, byte ptr [ebx + 0xb]        ; 0fb65b0b
        cmp     ebx, 4                           ; 83fb04
        jle     0x41b80                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x41b80                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x41b67                          ; 7505
        call    0x46188                          ; e821460000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e836fdffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8ef70c0000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8ce90c0000

; ----- cell 4: esi+0xa14  tile[+0xa]  mask[+0] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x41be5                          ; 7d48
        cmp     ecx, 0                           ; 83f900
        jl      0x41be5                          ; 7c43
        mov     ebx, dword ptr [esi + 0xa14]     ; 8b9e140a0000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x41be5                          ; 7e34
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x41be5                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x41bcc                          ; 7505
        call    0x46188                          ; e8bc450000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8d1fcffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e920c0000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c840c0000

; ----- cell 5: esi+0xa10  tile[+0xa]  mask[+0x10] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x41c4b                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x41c4b                          ; 7c44
        mov     ebx, dword ptr [esi + 0xa10]     ; 8b9e100a0000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x41c4b                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x41c4b                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x41c32                          ; 7505
        call    0x46188                          ; e856450000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e86bfcffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e2c0c0000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c1e0c0000

; ----- cell 6: esi+0x810  tile[+0xa]  mask[+8] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x41cb1                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x41cb1                          ; 7c44
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x41cb1                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x41cb1                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x41c98                          ; 7505
        call    0x46188                          ; e8f0440000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e805fcffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8ec60b0000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8cb80b0000

; ----- cell 7: esi+0xa10  tile[+9]  mask[+0xc] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x41d17                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x41d17                          ; 7c44
        mov     ebx, dword ptr [esi + 0xa10]     ; 8b9e100a0000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x41d17                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x41d17                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x41cfe                          ; 7505
        call    0x46188                          ; e88a440000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e89ffbffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e600b0000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c520b0000

; ----- cell 8: esi+0x810  tile[+9]  mask[+4] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x41d7d                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x41d7d                          ; 7c44
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x41d7d                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x41d7d                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x41d64                          ; 7505
        call    0x46188                          ; e824440000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e839fbffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8efa0a0000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8cec0a0000

; ----- cell 9: esi+0x80c  tile[+9]  mask[+0x14] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x41de3                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x41de3                          ; 7c44
        mov     ebx, dword ptr [esi + 0x80c]     ; 8b9e0c080000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x41de3                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x41de3                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x41dca                          ; 7505
        call    0x46188                          ; e8be430000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8d3faffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e940a0000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c860a0000

; ----- cell 10: esi+0x810  tile[+8]  mask[+0] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x41e48                          ; 7d48
        cmp     ecx, 0                           ; 83f900
        jl      0x41e48                          ; 7c43
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x41e48                          ; 7e34
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x41e48                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x41e2f                          ; 7505
        call    0x46188                          ; e859430000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e86efaffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e2f0a0000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c210a0000

; ----- cell 11: esi+0x80c  tile[+8]  mask[+0x10] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x41eae                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x41eae                          ; 7c44
        mov     ebx, dword ptr [esi + 0x80c]     ; 8b9e0c080000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x41eae                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x41eae                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x41e95                          ; 7505
        call    0x46188                          ; e8f3420000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e808faffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8ec9090000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8cbb090000

; ----- cell 12: esi+0x60c  tile[+8]  mask[+8] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x41f14                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x41f14                          ; 7c44
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x41f14                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x41f14                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x41efb                          ; 7505
        call    0x46188                          ; e88d420000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8a2f9ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e63090000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c55090000

; ----- cell 13: esi+0x80c  tile[+7]  mask[+0xc] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x41f7a                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x41f7a                          ; 7c44
        mov     ebx, dword ptr [esi + 0x80c]     ; 8b9e0c080000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x41f7a                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x41f7a                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x41f61                          ; 7505
        call    0x46188                          ; e827420000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e83cf9ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8efd080000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8cef080000

; ----- cell 14: esi+0x60c  tile[+7]  mask[+4] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x41fe0                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x41fe0                          ; 7c44
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x41fe0                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x41fe0                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x41fc7                          ; 7505
        call    0x46188                          ; e8c1410000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8d6f8ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e97080000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c89080000

; ----- cell 15: esi+0x608  tile[+7]  mask[+0x14] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42046                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42046                          ; 7c44
        mov     ebx, dword ptr [esi + 0x608]     ; 8b9e08060000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x42046                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x42046                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4202d                          ; 7505
        call    0x46188                          ; e85b410000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e870f8ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e31080000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c23080000

; ----- cell 16: esi+0x60c  tile[+6]  mask[+0] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x420ab                          ; 7d48
        cmp     ecx, 0                           ; 83f900
        jl      0x420ab                          ; 7c43
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x420ab                          ; 7e34
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x420ab                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42092                          ; 7505
        call    0x46188                          ; e8f6400000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e80bf8ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8ecc070000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8cbe070000

; ----- cell 17: esi+0x608  tile[+6]  mask[+0x10] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42111                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42111                          ; 7c44
        mov     ebx, dword ptr [esi + 0x608]     ; 8b9e08060000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x42111                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x42111                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x420f8                          ; 7505
        call    0x46188                          ; e890400000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8a5f7ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e66070000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c58070000

; ----- cell 18: esi+0x408  tile[+6]  mask[+8] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42177                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42177                          ; 7c44
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x42177                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x42177                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4215e                          ; 7505
        call    0x46188                          ; e82a400000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e83ff7ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e00070000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8cf2060000

; ----- cell 19: esi+0x608  tile[+5]  mask[+0xc] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x421dd                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x421dd                          ; 7c44
        mov     ebx, dword ptr [esi + 0x608]     ; 8b9e08060000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x421dd                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x421dd                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x421c4                          ; 7505
        call    0x46188                          ; e8c43f0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8d9f6ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e9a060000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c8c060000

; ----- cell 20: esi+0x408  tile[+5]  mask[+4] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42243                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42243                          ; 7c44
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x42243                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x42243                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4222a                          ; 7505
        call    0x46188                          ; e85e3f0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e873f6ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e34060000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c26060000

; ----- cell 21: esi+0x404  tile[+5]  mask[+0x14] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x422a9                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x422a9                          ; 7c44
        mov     ebx, dword ptr [esi + 0x404]     ; 8b9e04040000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x422a9                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x422a9                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42290                          ; 7505
        call    0x46188                          ; e8f83e0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e80df6ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8ece050000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8cc0050000

; ----- cell 22: esi+0x408  tile[+4]  mask[+0] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4230e                          ; 7d48
        cmp     ecx, 0                           ; 83f900
        jl      0x4230e                          ; 7c43
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x4230e                          ; 7e34
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x4230e                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x422f5                          ; 7505
        call    0x46188                          ; e8933e0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8a8f5ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e69050000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c5b050000

; ----- cell 23: esi+0x404  tile[+4]  mask[+0x10] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42374                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42374                          ; 7c44
        mov     ebx, dword ptr [esi + 0x404]     ; 8b9e04040000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x42374                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x42374                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4235b                          ; 7505
        call    0x46188                          ; e82d3e0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e842f5ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e03050000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8cf5040000

; ----- cell 24: esi+0x204  tile[+4]  mask[+8] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x423da                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x423da                          ; 7c44
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x423da                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x423da                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x423c1                          ; 7505
        call    0x46188                          ; e8c73d0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8dcf4ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e9d040000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c8f040000

; ----- cell 25: esi+0x404  tile[+3]  mask[+0xc] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42440                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42440                          ; 7c44
        mov     ebx, dword ptr [esi + 0x404]     ; 8b9e04040000
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x42440                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x42440                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42427                          ; 7505
        call    0x46188                          ; e8613d0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e876f4ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e37040000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c29040000

; ----- cell 26: esi+0x204  tile[+3]  mask[+4] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x424a6                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x424a6                          ; 7c44
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x424a6                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x424a6                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4248d                          ; 7505
        call    0x46188                          ; e8fb3c0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e810f4ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8ed1030000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8cc3030000

; ----- cell 27: esi+0x200  tile[+3]  mask[+0x14] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4250c                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x4250c                          ; 7c44
        mov     ebx, dword ptr [esi + 0x200]     ; 8b9e00020000
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x4250c                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x4250c                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x424f3                          ; 7505
        call    0x46188                          ; e8953c0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8aaf3ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e6b030000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c5d030000

; ----- cell 28: esi+0x204  tile[+2]  mask[+0] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42571                          ; 7d48
        cmp     ecx, 0                           ; 83f900
        jl      0x42571                          ; 7c43
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x42571                          ; 7e34
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x42571                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42558                          ; 7505
        call    0x46188                          ; e8303c0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e845f3ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e06030000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8cf8020000

; ----- cell 29: esi+0x200  tile[+2]  mask[+0x10] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x425d7                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x425d7                          ; 7c44
        mov     ebx, dword ptr [esi + 0x200]     ; 8b9e00020000
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x425d7                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x425d7                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x425be                          ; 7505
        call    0x46188                          ; e8ca3b0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8dff2ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8ea0020000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c92020000

; ----- cell 30: esi+0  tile[+2]  mask[+8] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42636                          ; 7d45
        cmp     ecx, 0                           ; 83f900
        jl      0x42636                          ; 7c40
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x42636                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x42636                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4261d                          ; 7505
        call    0x46188                          ; e86b3b0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e880f2ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e41020000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c33020000

; ----- cell 31: esi+0x200  tile[+1]  mask[+0xc] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4269c                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x4269c                          ; 7c44
        mov     ebx, dword ptr [esi + 0x200]     ; 8b9e00020000
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x4269c                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x4269c                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42683                          ; 7505
        call    0x46188                          ; e8053b0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e81af2ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8edb010000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8ccd010000

; ----- cell 32: esi+0  tile[+1]  mask[+4] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x426fb                          ; 7d45
        cmp     ecx, 0                           ; 83f900
        jl      0x426fb                          ; 7c40
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x426fb                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x426fb                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x426e2                          ; 7505
        call    0x46188                          ; e8a63a0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8bbf1ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e7c010000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c6e010000

; ----- cell 33: esi-4  tile[+1]  mask[+0x14] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, -4                          ; 83c1fc
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4275b                          ; 7d46
        cmp     ecx, 0                           ; 83f900
        jl      0x4275b                          ; 7c41
        mov     ebx, dword ptr [esi - 4]         ; 8b5efc
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x4275b                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x4275b                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42742                          ; 7505
        call    0x46188                          ; e8463a0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e85bf1ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8e1c010000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8c0e010000

; ----- cell 34: esi+0  tile[+0]  mask[+0] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x427b8                          ; 7d43
        cmp     ecx, 0                           ; 83f900
        jl      0x427b8                          ; 7c3e
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x427b8                          ; 7e34
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x427b8                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4279f                          ; 7505
        call    0x46188                          ; e8e9390000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8fef0ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 0f8ebf000000
        dec     edx                              ; 4a
        jl      0x42870                          ; 0f8cb1000000

; ----- cell 35: esi-4  tile[+0]  mask[+0x10] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, -4                          ; 83c1fc
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42813                          ; 7d41
        cmp     ecx, 0                           ; 83f900
        jl      0x42813                          ; 7c3c
        mov     ebx, dword ptr [esi - 4]         ; 8b5efc
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x42813                          ; 7e31
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x42813                          ; 7420
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x427fe                          ; 7505
        call    0x46188                          ; e88a390000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e89ff0ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 7e64
        dec     edx                              ; 4a
        jl      0x42870                          ; 7c5a

; ----- cell 36: esi+0xfffffdfc  tile[+0]  mask[+8] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffdfc                  ; 81c1fcfdffff
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42870                          ; 7d44
        cmp     ecx, 0                           ; 83f900
        jl      0x42870                          ; 7c3f
        mov     ebx, dword ptr [esi - 0x204]     ; 8b9efcfdffff
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x42870                          ; 7e31
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x42870                          ; 7420
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4285b                          ; 7505
        call    0x46188                          ; e82d390000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e842f0ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x42877                          ; 7e07

; ===== exit =====
        mov     al, byte ptr [ebp - 1]           ; AL = did-any-cell flag
        mov     ah, 0                            ; AH=0: walk finished / budget ran out
        leave                                    ; c9
        ret                                      ; c3
        mov     al, byte ptr [ebp - 1]           ; AL = did-any-cell flag
        mov     ah, 1                            ; AH=1: stopped early (column index <= 0)
        leave                                    ; c9
        ret                                      ; c3
