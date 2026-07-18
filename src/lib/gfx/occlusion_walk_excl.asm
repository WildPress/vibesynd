; occlusion_walk_excl @ 000436b2  (4491 bytes) -- hand-written assembly (fully commented).
;
; occlusion_walk_excl: the same isometric scene visibility walker as occlusion_walk, walking the
; same diamond of 36 cells and folding each drawable cell's mask into the table at 0xe144
; -- but with one extra test per cell: two specific cells are excluded from the merge.
; Those two are held in the globals at 0x38952 and 0x38956 (cell-slot pointers); a cell
; equal to either is skipped. They appear to be cells that get drawn separately (for
; example the ones under or around the active agent) and so must not occlude themselves.
;
; esi points at the map cell-pointer array (4-byte cells; row stride 0x200, column stride
; 4). edx is the cell budget. The walk is fully unrolled, one block per cell.
;
; Each cell block (see the fully-commented first block below):
;   1. form the slot offset, subtract g_map_cols (0x5358), reject if not in [0, 0xc000);
;   2. read the one-byte tile/height code; codes <= 4 are skipped;
;   3. index the type->draw-data table at 0x5360 (stride 0x18), fetch a mask-data offset;
;      a zero offset is skipped;
;   4. skip the cell if its slot equals the excluded pair at 0x38952 / 0x38956;
;   5. clear the mask table once on the first survivor (clear_occlusion_mask);
;   6. OR the cell's inverted mask into the table (merge_cell_mask).
;
; After each processed cell, if the folded column index (cx) has reached 0 the walk stops
; early. The tile field steps +0xb..+0 and the mask slot cycles +0xc/+4/+0x14/+0x10/+8/+0.
;
; Returns AX:  AL = 1 if any cell was processed, else 0;  AH = 0 if the walk finished or
; the edx budget ran out, AH = 1 if it stopped early on a column index of 0. [ebp-1] is
; the did-any flag. Globals: 0x5358 g_map_cols, 0x5360 type->draw-data base, 0x38952 /
; 0x38956 excluded-cell pointers, 0xe144 running mask table.
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
; The build uses occlusion_walk_excl.c (a db-transcription of the raw bytes); this .asm is the
; readable companion. See docs/game-vs-library.md.
;
occlusion_walk_excl:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        add     esp, -4                          ; 83c4fc
        mov     byte ptr [ebp - 1], 0            ; c645ff00
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c6c110000

; ----- cell 1: esi+0xc14  tile[+0xb]  mask[+0xc] -----
        mov     ecx, esi                         ; ecx = cell-array cursor (esi)
        add     ecx, 0xc14                       ; point at this cell's map slot (cursor + offset)
        sub     ecx, dword ptr [0x5358]          ; - g_map_cols: fold to a 0-based on-map index
        cmp     ecx, 0xc000                      ; index still inside the map array (< 0xc000)?
        jge     0x4373a                          ; no -> skip this cell, try the next
        cmp     ecx, 0                           ; index >= 0 ?
        jl      0x4373a                          ; no -> skip this cell, try the next
        mov     ebx, dword ptr [esi + 0xc14]     ; ebx = this cell's record pointer
        movzx   ebx, byte ptr [ebx + 0xb]        ; read the tile/height code for this sub-layer
        cmp     ebx, 4                           ; codes 0..4 are empty/floor
        jle     0x4373a                          ; -> nothing to occlude here, skip
        imul    ebx, ebx, 0x18                   ; index the type table (stride 0x18)
        add     ebx, dword ptr [0x5360]          ; + type->draw-data table base (0x5360)
        mov     ebx, dword ptr [ebx + 0xc]       ; fetch this layer's mask-data offset (draw-data slot)
        cmp     ebx, 0                           ; no mask for this layer?
        je      0x4373a                          ; -> skip
        mov     ecx, esi                         ; recompute this cell's slot address
        add     ecx, 0xc14                       ; (same slot offset)
        cmp     ecx, dword ptr [0x38952]         ; is it the first cell drawn separately (excluded)?
        je      0x4373a                          ; -> skip
        cmp     ecx, dword ptr [0x38956]         ; the second excluded cell?
        je      0x4373a                          ; -> skip
        cmp     byte ptr [ebp - 1], 0            ; running mask table already cleared this call?
        jne     0x43721                          ; yes -> don't clear it again
        call    0x46188                          ; one-time: zero the mask table at 0xe144 (clear_occlusion_mask)
        add     byte ptr [ebp - 1], 1            ; mark it cleared (this byte is also return value AL)
        add     ebx, dword ptr [0x5360]          ; mask pointer = offset + draw-data base (0x5360)
        call    0x418ac                          ; OR this cell's inverted mask into the table (merge_cell_mask)
        cmp     cx, 0                            ; column index (low word) still ahead of the view?
        jle     0x44836                          ; no -> stop, return AH=1
        dec     edx                              ; cell budget--
        jl      0x4482f                          ; budget exhausted -> stop, return AH=0

; ----- cell 2: esi+0xa14  tile[+0xb]  mask[+4] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x437b8                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x437b8                          ; 7c5c
        mov     ebx, dword ptr [esi + 0xa14]     ; 8b9e140a0000
        movzx   ebx, byte ptr [ebx + 0xb]        ; 0fb65b0b
        cmp     ebx, 4                           ; 83fb04
        jle     0x437b8                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x437b8                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x437b8                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x437b8                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4379f                          ; 7505
        call    0x46188                          ; e8e9290000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8fee0ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e7e100000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c70100000

; ----- cell 3: esi+0xa10  tile[+0xb]  mask[+0x14] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43836                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43836                          ; 7c5c
        mov     ebx, dword ptr [esi + 0xa10]     ; 8b9e100a0000
        movzx   ebx, byte ptr [ebx + 0xb]        ; 0fb65b0b
        cmp     ebx, 4                           ; 83fb04
        jle     0x43836                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x43836                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43836                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43836                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4381d                          ; 7505
        call    0x46188                          ; e86b290000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e880e0ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e00100000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cf20f0000

; ----- cell 4: esi+0xa14  tile[+0xa]  mask[+0] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x438b3                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x438b3                          ; 7c5b
        mov     ebx, dword ptr [esi + 0xa14]     ; 8b9e140a0000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x438b3                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x438b3                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x438b3                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x438b3                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4389a                          ; 7505
        call    0x46188                          ; e8ee280000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e803e0ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e830f0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c750f0000

; ----- cell 5: esi+0xa10  tile[+0xa]  mask[+0x10] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43931                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43931                          ; 7c5c
        mov     ebx, dword ptr [esi + 0xa10]     ; 8b9e100a0000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x43931                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x43931                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43931                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43931                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43918                          ; 7505
        call    0x46188                          ; e870280000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e885dfffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e050f0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cf70e0000

; ----- cell 6: esi+0x810  tile[+0xa]  mask[+8] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x439af                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x439af                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x439af                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x439af                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x439af                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x439af                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43996                          ; 7505
        call    0x46188                          ; e8f2270000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e807dfffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e870e0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c790e0000

; ----- cell 7: esi+0xa10  tile[+9]  mask[+0xc] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43a2d                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43a2d                          ; 7c5c
        mov     ebx, dword ptr [esi + 0xa10]     ; 8b9e100a0000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x43a2d                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x43a2d                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43a2d                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43a2d                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43a14                          ; 7505
        call    0x46188                          ; e874270000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e889deffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e090e0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cfb0d0000

; ----- cell 8: esi+0x810  tile[+9]  mask[+4] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43aab                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43aab                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x43aab                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x43aab                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43aab                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43aab                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43a92                          ; 7505
        call    0x46188                          ; e8f6260000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e80bdeffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e8b0d0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c7d0d0000

; ----- cell 9: esi+0x80c  tile[+9]  mask[+0x14] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43b29                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43b29                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x80c]     ; 8b9e0c080000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x43b29                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x43b29                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43b29                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43b29                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43b10                          ; 7505
        call    0x46188                          ; e878260000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e88dddffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e0d0d0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cff0c0000

; ----- cell 10: esi+0x810  tile[+8]  mask[+0] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43ba6                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x43ba6                          ; 7c5b
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x43ba6                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x43ba6                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43ba6                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43ba6                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43b8d                          ; 7505
        call    0x46188                          ; e8fb250000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e810ddffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e900c0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c820c0000

; ----- cell 11: esi+0x80c  tile[+8]  mask[+0x10] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43c24                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43c24                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x80c]     ; 8b9e0c080000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x43c24                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x43c24                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43c24                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43c24                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43c0b                          ; 7505
        call    0x46188                          ; e87d250000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e892dcffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e120c0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c040c0000

; ----- cell 12: esi+0x60c  tile[+8]  mask[+8] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43ca2                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43ca2                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x43ca2                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x43ca2                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43ca2                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43ca2                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43c89                          ; 7505
        call    0x46188                          ; e8ff240000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e814dcffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e940b0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c860b0000

; ----- cell 13: esi+0x80c  tile[+7]  mask[+0xc] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43d20                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43d20                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x80c]     ; 8b9e0c080000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x43d20                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x43d20                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43d20                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43d20                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43d07                          ; 7505
        call    0x46188                          ; e881240000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e896dbffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e160b0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c080b0000

; ----- cell 14: esi+0x60c  tile[+7]  mask[+4] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43d9e                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43d9e                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x43d9e                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x43d9e                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43d9e                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43d9e                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43d85                          ; 7505
        call    0x46188                          ; e803240000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e818dbffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e980a0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c8a0a0000

; ----- cell 15: esi+0x608  tile[+7]  mask[+0x14] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43e1c                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43e1c                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x608]     ; 8b9e08060000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x43e1c                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x43e1c                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43e1c                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43e1c                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43e03                          ; 7505
        call    0x46188                          ; e885230000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e89adaffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e1a0a0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c0c0a0000

; ----- cell 16: esi+0x60c  tile[+6]  mask[+0] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43e99                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x43e99                          ; 7c5b
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x43e99                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x43e99                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43e99                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43e99                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43e80                          ; 7505
        call    0x46188                          ; e808230000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e81ddaffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e9d090000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c8f090000

; ----- cell 17: esi+0x608  tile[+6]  mask[+0x10] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43f17                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43f17                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x608]     ; 8b9e08060000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x43f17                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x43f17                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43f17                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43f17                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43efe                          ; 7505
        call    0x46188                          ; e88a220000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e89fd9ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e1f090000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c11090000

; ----- cell 18: esi+0x408  tile[+6]  mask[+8] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43f95                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43f95                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x43f95                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x43f95                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43f95                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43f95                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43f7c                          ; 7505
        call    0x46188                          ; e80c220000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e821d9ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8ea1080000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c93080000

; ----- cell 19: esi+0x608  tile[+5]  mask[+0xc] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44013                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44013                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x608]     ; 8b9e08060000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x44013                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x44013                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44013                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44013                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43ffa                          ; 7505
        call    0x46188                          ; e88e210000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8a3d8ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e23080000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c15080000

; ----- cell 20: esi+0x408  tile[+5]  mask[+4] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44091                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44091                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x44091                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x44091                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44091                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44091                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44078                          ; 7505
        call    0x46188                          ; e810210000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e825d8ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8ea5070000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c97070000

; ----- cell 21: esi+0x404  tile[+5]  mask[+0x14] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4410f                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x4410f                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x404]     ; 8b9e04040000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x4410f                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x4410f                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4410f                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4410f                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x440f6                          ; 7505
        call    0x46188                          ; e892200000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8a7d7ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e27070000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c19070000

; ----- cell 22: esi+0x408  tile[+4]  mask[+0] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4418c                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x4418c                          ; 7c5b
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x4418c                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x4418c                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4418c                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4418c                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44173                          ; 7505
        call    0x46188                          ; e815200000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e82ad7ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8eaa060000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c9c060000

; ----- cell 23: esi+0x404  tile[+4]  mask[+0x10] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4420a                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x4420a                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x404]     ; 8b9e04040000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x4420a                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x4420a                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4420a                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4420a                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x441f1                          ; 7505
        call    0x46188                          ; e8971f0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8acd6ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e2c060000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c1e060000

; ----- cell 24: esi+0x204  tile[+4]  mask[+8] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44288                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44288                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x44288                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x44288                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44288                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44288                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4426f                          ; 7505
        call    0x46188                          ; e8191f0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e82ed6ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8eae050000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8ca0050000

; ----- cell 25: esi+0x404  tile[+3]  mask[+0xc] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44306                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44306                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x404]     ; 8b9e04040000
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x44306                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x44306                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44306                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44306                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x442ed                          ; 7505
        call    0x46188                          ; e89b1e0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8b0d5ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e30050000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c22050000

; ----- cell 26: esi+0x204  tile[+3]  mask[+4] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44384                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44384                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x44384                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x44384                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44384                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44384                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4436b                          ; 7505
        call    0x46188                          ; e81d1e0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e832d5ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8eb2040000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8ca4040000

; ----- cell 27: esi+0x200  tile[+3]  mask[+0x14] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44402                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44402                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x200]     ; 8b9e00020000
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x44402                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x44402                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44402                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44402                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x443e9                          ; 7505
        call    0x46188                          ; e89f1d0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8b4d4ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e34040000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c26040000

; ----- cell 28: esi+0x204  tile[+2]  mask[+0] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4447f                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x4447f                          ; 7c5b
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x4447f                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x4447f                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4447f                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4447f                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44466                          ; 7505
        call    0x46188                          ; e8221d0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e837d4ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8eb7030000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8ca9030000

; ----- cell 29: esi+0x200  tile[+2]  mask[+0x10] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x444fd                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x444fd                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x200]     ; 8b9e00020000
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x444fd                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x444fd                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x444fd                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x444fd                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x444e4                          ; 7505
        call    0x46188                          ; e8a41c0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8b9d3ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e39030000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c2b030000

; ----- cell 30: esi+0  tile[+2]  mask[+8] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44571                          ; 7d5a
        cmp     ecx, 0                           ; 83f900
        jl      0x44571                          ; 7c55
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x44571                          ; 7e4a
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x44571                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44571                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44571                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44558                          ; 7505
        call    0x46188                          ; e8301c0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e845d3ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8ec5020000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cb7020000

; ----- cell 31: esi+0x200  tile[+1]  mask[+0xc] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x445ef                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x445ef                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x200]     ; 8b9e00020000
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x445ef                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x445ef                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x445ef                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x445ef                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x445d6                          ; 7505
        call    0x46188                          ; e8b21b0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8c7d2ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e47020000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c39020000

; ----- cell 32: esi+0  tile[+1]  mask[+4] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44663                          ; 7d5a
        cmp     ecx, 0                           ; 83f900
        jl      0x44663                          ; 7c55
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x44663                          ; 7e4a
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x44663                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44663                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44663                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4464a                          ; 7505
        call    0x46188                          ; e83e1b0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e853d2ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8ed3010000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cc5010000

; ----- cell 33: esi-4  tile[+1]  mask[+0x14] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, -4                          ; 83c1fc
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x446d8                          ; 7d5b
        cmp     ecx, 0                           ; 83f900
        jl      0x446d8                          ; 7c56
        mov     ebx, dword ptr [esi - 4]         ; 8b5efc
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x446d8                          ; 7e4a
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x446d8                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, -4                          ; 83c1fc
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x446d8                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x446d8                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x446bf                          ; 7505
        call    0x46188                          ; e8c91a0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8ded1ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e5e010000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c50010000

; ----- cell 34: esi+0  tile[+0]  mask[+0] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4474a                          ; 7d58
        cmp     ecx, 0                           ; 83f900
        jl      0x4474a                          ; 7c53
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x4474a                          ; 7e49
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x4474a                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4474a                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4474a                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44731                          ; 7505
        call    0x46188                          ; e8571a0000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e86cd1ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8eec000000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cde000000

; ----- cell 35: esi-4  tile[+0]  mask[+0x10] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, -4                          ; 83c1fc
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x447ba                          ; 7d56
        cmp     ecx, 0                           ; 83f900
        jl      0x447ba                          ; 7c51
        mov     ebx, dword ptr [esi - 4]         ; 8b5efc
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x447ba                          ; 7e46
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x447ba                          ; 7435
        mov     ecx, esi                         ; 8bce
        add     ecx, -4                          ; 83c1fc
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x447ba                          ; 7428
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x447ba                          ; 7420
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x447a5                          ; 7505
        call    0x46188                          ; e8e3190000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8f8d0ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 7e7c
        dec     edx                              ; 4a
        jl      0x4482f                          ; 7c72

; ----- cell 36: esi+0xfffffdfc  tile[+0]  mask[+8] -----
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffdfc                  ; 81c1fcfdffff
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4482f                          ; 7d5c
        cmp     ecx, 0                           ; 83f900
        jl      0x4482f                          ; 7c57
        mov     ebx, dword ptr [esi - 0x204]     ; 8b9efcfdffff
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x4482f                          ; 7e49
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x4482f                          ; 7438
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffdfc                  ; 81c1fcfdffff
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4482f                          ; 7428
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4482f                          ; 7420
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4481a                          ; 7505
        call    0x46188                          ; e86e190000     -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e883d0ffff     -> merge_cell_mask
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 7e07

; ===== exit =====
        mov     al, byte ptr [ebp - 1]           ; AL = did-any-cell flag
        mov     ah, 0                            ; AH=0: walk finished / budget ran out
        leave                                    ; c9
        ret                                      ; c3
        mov     al, byte ptr [ebp - 1]           ; AL = did-any-cell flag
        mov     ah, 1                            ; AH=1: stopped early (column index <= 0)
        leave                                    ; c9
        ret                                      ; c3
