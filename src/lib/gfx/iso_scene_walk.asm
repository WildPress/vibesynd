; iso_scene_walk @ 0x4287e  (3636 bytes) -- hand-written assembly (fully commented).
;
; iso_scene_walk: the isometric scene/render walker. It visits the map cells around
; the current view centre, and for each cell that holds a visible object it dispatches
; a draw. This is not a pixel blitter. It decides *what* to draw and in what order,
; then hands each object to the visibility-mask merge merge_cell_mask.
;
; Registers in (a private, non-C convention -- another sign this is hand asm):
;   esi = base of the cell pointer-table. It is an array of object-record pointers,
;         one per map cell, laid out so that a step of 0x200 bytes (128 dword slots)
;         moves one row and a step of 4 bytes moves one column. The walker reads it at
;         fixed byte offsets that trace a diamond around the centre.
;   edx = cell countdown. Decremented once per cell; when it goes negative the walk
;         stops (normal completion). The caller sets how many cells to visit.
;   ebx = scratch. Across one cell it holds, in turn: the object-record pointer, then
;         the record's type byte, then the type-table index, then the draw-data value.
;   ecx = scratch screen column. Low 16 bits (cx) double as the early-stop signal.
;   [ebp-1] = one-shot "setup done" flag, also the low byte of the return value.
;
; Globals:
;   0x5358  g_map_cols   subtracted from each cell's computed column to place it
;                        relative to the visible strip.
;   0x5360  type-table base. A record's type indexes a 0x18-byte (24-byte) entry here
;           (type*0x18 + base); the entry holds six dword draw-data fields at offsets
;           0, 4, 8, 0xc, 0x10, 0x14. Each cell reads one of the six. The fetched field
;           is itself table-relative, so it is re-based (+0x5360) into a pointer before
;           the draw call.
;   0xe144  (reached inside the callees) the 16-slot visibility-mask accumulator.
;
; Callees:
;   clear_occlusion_mask  one-shot per-call setup. Zeroes the 16-slot mask accumulator at
;                 0xe144. Guarded by [ebp-1] so it runs at most once, on the first
;                 object actually drawn this call.
;   merge_cell_mask  the 16x-unrolled visibility-mask OR-merge. Given a mask block in ebx
;                 it folds that object's coverage into the accumulator.
;
; The per-cell test is the same each time:
;   1. column = [esi]+offset ; column -= g_map_cols
;   2. reject the cell if column >= 0xc000 or column < 0 (off the visible strip)
;   3. rec = [esi+offset]    ; type = rec[byte offset]  (a per-cell height byte)
;   4. reject if type <= 4   (empty / non-drawable tiles)
;   5. entry = type*0x18 + type-table base ; data = entry[draw-slot]
;   6. reject if data == 0   (this layer has nothing for this type)
;   7. first drawn object only: call the one-shot setup, set the flag
;   8. ebx = data + type-table base ; call the mask merge
;   9. if cx <= 0 after the merge, stop early
;
; The body below is that same test unrolled ~36 times, once per cell, each with its own
; cell offset, type-byte offset, and draw-slot. The offsets step through neighbouring
; cells in an isometric diamond and the sequence walks from the far cells inward toward
; the centre, so nearer objects are merged last. The first two or three iterations are
; commented line by line; the rest carry a one-line banner noting the offset so the walk
; pattern is visible without repeating 36 identical blocks.
;
; The two shared exits are labelled: .walk_done (all cells visited, or the last cell
; rejected) returns AH=0, and .walk_early_out (cx fell to <=0) returns AH=1. Either way
; AL is the [ebp-1] flag: 1 if at least one object was drawn, 0 if none. The build uses
; iso_scene_walk.c (the raw bytes as a db-transcription); this .asm is the readable
; companion. See docs/game-vs-library.md and docs/blitter.md.

iso_scene_walk:
        push    ebp
        mov     ebp, esp
        add     esp, -4                          ; make room for the one local byte
        mov     byte ptr [ebp - 1], 0            ; setup-done flag = 0 (nothing drawn yet)

; --- cell 01: offset 0xa18, type byte [rec+0xb], draw-slot [entry+0] ---
        dec     edx                              ; one fewer cell to process
        jl      .walk_done                       ; countdown exhausted -> done (AH=0)
        mov     ecx, esi                          ; ecx = cell-table base
        add     ecx, 0xa18                        ;   + this cell's offset
        sub     ecx, dword ptr [0x5358]          ; column -= g_map_cols   0x5358=g_map_cols
        cmp     ecx, 0xc000                       ; column past the right edge?
        jge     0x428ed                           ;   yes -> skip this cell (go to cell 02)
        cmp     ecx, 0                             ; column left of the strip?
        jl      0x428ed                           ;   yes -> skip this cell
        mov     ebx, dword ptr [esi + 0xa18]      ; ebx = object record for this cell
        movzx   ebx, byte ptr [ebx + 0xb]         ; ebx = record type byte (this height)
        cmp     ebx, 4                             ; type <= 4 is empty / non-drawable
        jle     0x428ed                           ;   -> skip this cell
        imul    ebx, ebx, 0x18                    ; type * 0x18 (24-byte type entry)
        add     ebx, dword ptr [0x5360]           ;   + type-table base -> &entry
        mov     ebx, dword ptr [ebx]              ; ebx = entry.draw-slot0 (draw-data value)
        cmp     ebx, 0                             ; nothing to draw for this type/layer?
        je      0x428ed                           ;   -> skip this cell
        cmp     byte ptr [ebp - 1], 0             ; has the one-shot setup run yet?
        jne     0x428d4                           ;   already done -> skip the setup call
        call    0x46188                           ; first drawn object: -> clear_occlusion_mask (zero the mask accumulator)
        add     byte ptr [ebp - 1], 1             ; mark setup done (and "something drew")
        add     ebx, dword ptr [0x5360]           ; re-base the draw-data value to a pointer
        call    0x418ac                           ; merge this object's mask -> merge_cell_mask
        cmp     cx, 0                              ; merge signalled stop?
        jle     .walk_early_out                    ;   cx <= 0 -> stop early (AH=1)

; --- cell 02: offset 0xa14, type byte [rec+0xb], draw-slot [entry+0x10] ---
        dec     edx                              ; one fewer cell to process
        jl      .walk_done                       ; countdown exhausted -> done
        mov     ecx, esi                          ; ecx = cell-table base
        add     ecx, 0xa14                        ;   + this cell's offset
        sub     ecx, dword ptr [0x5358]          ; column -= g_map_cols   0x5358=g_map_cols
        cmp     ecx, 0xc000                       ; past the right edge?
        jge     0x42953                           ;   -> skip to cell 03
        cmp     ecx, 0                             ; left of the strip?
        jl      0x42953                           ;   -> skip to cell 03
        mov     ebx, dword ptr [esi + 0xa14]      ; ebx = object record for this cell
        movzx   ebx, byte ptr [ebx + 0xb]         ; ebx = record type byte
        cmp     ebx, 4                             ; empty / non-drawable?
        jle     0x42953                           ;   -> skip to cell 03
        imul    ebx, ebx, 0x18                    ; type * 0x18
        add     ebx, dword ptr [0x5360]           ;   + type-table base -> &entry
        mov     ebx, dword ptr [ebx + 0x10]       ; ebx = entry.draw-slot4 (this cell's slot)
        cmp     ebx, 0                             ; nothing for this layer?
        je      0x42953                           ;   -> skip to cell 03
        cmp     byte ptr [ebp - 1], 0             ; one-shot setup done?
        jne     0x4293a                           ;   -> skip setup call
        call    0x46188                           ; -> clear_occlusion_mask (one-shot setup)
        add     byte ptr [ebp - 1], 1             ; mark setup done
        add     ebx, dword ptr [0x5360]           ; re-base draw-data to a pointer
        call    0x418ac                           ; -> merge_cell_mask (mask merge)
        cmp     cx, 0                              ; stop signal?
        jle     .walk_early_out                    ;   -> stop early

; --- cell 03: offset 0x814, type byte [rec+0xb], draw-slot [entry+8] ---
        dec     edx                              ; one fewer cell to process
        jl      .walk_done                       ; countdown exhausted -> done
        mov     ecx, esi                          ; ecx = cell-table base
        add     ecx, 0x814                        ;   + this cell's offset
        sub     ecx, dword ptr [0x5358]          ; column -= g_map_cols   0x5358=g_map_cols
        cmp     ecx, 0xc000                       ; past the right edge?
        jge     0x429b9                           ;   -> skip to cell 04
        cmp     ecx, 0                             ; left of the strip?
        jl      0x429b9                           ;   -> skip to cell 04
        mov     ebx, dword ptr [esi + 0x814]      ; ebx = object record for this cell
        movzx   ebx, byte ptr [ebx + 0xb]         ; ebx = record type byte
        cmp     ebx, 4                             ; empty / non-drawable?
        jle     0x429b9                           ;   -> skip to cell 04
        imul    ebx, ebx, 0x18                    ; type * 0x18
        add     ebx, dword ptr [0x5360]           ;   + type-table base -> &entry
        mov     ebx, dword ptr [ebx + 8]          ; ebx = entry.draw-slot2
        cmp     ebx, 0                             ; nothing for this layer?
        je      0x429b9                           ;   -> skip to cell 04
        cmp     byte ptr [ebp - 1], 0             ; one-shot setup done?
        jne     0x429a0                           ;   -> skip setup call
        call    0x46188                           ; -> clear_occlusion_mask (one-shot setup)
        add     byte ptr [ebp - 1], 1             ; mark setup done
        add     ebx, dword ptr [0x5360]           ; re-base draw-data to a pointer
        call    0x418ac                           ; -> merge_cell_mask (mask merge)
        cmp     cx, 0                              ; stop signal?
        jle     .walk_early_out                    ;   -> stop early

; From here the block is identical bar the three per-cell constants (cell offset,
; type-byte offset, draw-slot). Each block: dec/jl countdown, compute+bounds-check the
; column, load the record and its type byte, reject type<=4, index the type table, fetch
; the draw-slot, reject null, run the one-shot setup once, merge the mask, early-out on
; cx<=0. Only the banner is given for each.

; --- cell 04: offset 0xa14, type byte [rec+0xa], draw-slot [entry+0xc] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0xa14
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42a1f
        cmp     ecx, 0
        jl      0x42a1f
        mov     ebx, dword ptr [esi + 0xa14]
        movzx   ebx, byte ptr [ebx + 0xa]
        cmp     ebx, 4
        jle     0x42a1f
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0xc]
        cmp     ebx, 0
        je      0x42a1f
        cmp     byte ptr [ebp - 1], 0
        jne     0x42a06
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 05: offset 0x814, type byte [rec+0xa], draw-slot [entry+4] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x814
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42a85
        cmp     ecx, 0
        jl      0x42a85
        mov     ebx, dword ptr [esi + 0x814]
        movzx   ebx, byte ptr [ebx + 0xa]
        cmp     ebx, 4
        jle     0x42a85
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 4]
        cmp     ebx, 0
        je      0x42a85
        cmp     byte ptr [ebp - 1], 0
        jne     0x42a6c
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 06: offset 0x810, type byte [rec+0xa], draw-slot [entry+0x14] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x810
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42aeb
        cmp     ecx, 0
        jl      0x42aeb
        mov     ebx, dword ptr [esi + 0x810]
        movzx   ebx, byte ptr [ebx + 0xa]
        cmp     ebx, 4
        jle     0x42aeb
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0x14]
        cmp     ebx, 0
        je      0x42aeb
        cmp     byte ptr [ebp - 1], 0
        jne     0x42ad2
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 07: offset 0x814, type byte [rec+9], draw-slot [entry+0] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x814
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42b50
        cmp     ecx, 0
        jl      0x42b50
        mov     ebx, dword ptr [esi + 0x814]
        movzx   ebx, byte ptr [ebx + 9]
        cmp     ebx, 4
        jle     0x42b50
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx]
        cmp     ebx, 0
        je      0x42b50
        cmp     byte ptr [ebp - 1], 0
        jne     0x42b37
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 08: offset 0x810, type byte [rec+9], draw-slot [entry+0x10] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x810
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42bb6
        cmp     ecx, 0
        jl      0x42bb6
        mov     ebx, dword ptr [esi + 0x810]
        movzx   ebx, byte ptr [ebx + 9]
        cmp     ebx, 4
        jle     0x42bb6
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0x10]
        cmp     ebx, 0
        je      0x42bb6
        cmp     byte ptr [ebp - 1], 0
        jne     0x42b9d
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 09: offset 0x610, type byte [rec+9], draw-slot [entry+8] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x610
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42c1c
        cmp     ecx, 0
        jl      0x42c1c
        mov     ebx, dword ptr [esi + 0x610]
        movzx   ebx, byte ptr [ebx + 9]
        cmp     ebx, 4
        jle     0x42c1c
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 8]
        cmp     ebx, 0
        je      0x42c1c
        cmp     byte ptr [ebp - 1], 0
        jne     0x42c03
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 10: offset 0x810, type byte [rec+8], draw-slot [entry+0xc] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x810
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42c82
        cmp     ecx, 0
        jl      0x42c82
        mov     ebx, dword ptr [esi + 0x810]
        movzx   ebx, byte ptr [ebx + 8]
        cmp     ebx, 4
        jle     0x42c82
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0xc]
        cmp     ebx, 0
        je      0x42c82
        cmp     byte ptr [ebp - 1], 0
        jne     0x42c69
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 11: offset 0x610, type byte [rec+8], draw-slot [entry+4] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x610
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42ce8
        cmp     ecx, 0
        jl      0x42ce8
        mov     ebx, dword ptr [esi + 0x610]
        movzx   ebx, byte ptr [ebx + 8]
        cmp     ebx, 4
        jle     0x42ce8
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 4]
        cmp     ebx, 0
        je      0x42ce8
        cmp     byte ptr [ebp - 1], 0
        jne     0x42ccf
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 12: offset 0x60c, type byte [rec+8], draw-slot [entry+0x14] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x60c
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42d4e
        cmp     ecx, 0
        jl      0x42d4e
        mov     ebx, dword ptr [esi + 0x60c]
        movzx   ebx, byte ptr [ebx + 8]
        cmp     ebx, 4
        jle     0x42d4e
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0x14]
        cmp     ebx, 0
        je      0x42d4e
        cmp     byte ptr [ebp - 1], 0
        jne     0x42d35
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 13: offset 0x610, type byte [rec+7], draw-slot [entry+0] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x610
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42db3
        cmp     ecx, 0
        jl      0x42db3
        mov     ebx, dword ptr [esi + 0x610]
        movzx   ebx, byte ptr [ebx + 7]
        cmp     ebx, 4
        jle     0x42db3
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx]
        cmp     ebx, 0
        je      0x42db3
        cmp     byte ptr [ebp - 1], 0
        jne     0x42d9a
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 14: offset 0x60c, type byte [rec+7], draw-slot [entry+0x10] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x60c
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42e19
        cmp     ecx, 0
        jl      0x42e19
        mov     ebx, dword ptr [esi + 0x60c]
        movzx   ebx, byte ptr [ebx + 7]
        cmp     ebx, 4
        jle     0x42e19
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0x10]
        cmp     ebx, 0
        je      0x42e19
        cmp     byte ptr [ebp - 1], 0
        jne     0x42e00
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 15: offset 0x40c, type byte [rec+7], draw-slot [entry+8] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x40c
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42e7f
        cmp     ecx, 0
        jl      0x42e7f
        mov     ebx, dword ptr [esi + 0x40c]
        movzx   ebx, byte ptr [ebx + 7]
        cmp     ebx, 4
        jle     0x42e7f
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 8]
        cmp     ebx, 0
        je      0x42e7f
        cmp     byte ptr [ebp - 1], 0
        jne     0x42e66
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 16: offset 0x60c, type byte [rec+6], draw-slot [entry+0xc] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x60c
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42ee5
        cmp     ecx, 0
        jl      0x42ee5
        mov     ebx, dword ptr [esi + 0x60c]
        movzx   ebx, byte ptr [ebx + 6]
        cmp     ebx, 4
        jle     0x42ee5
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0xc]
        cmp     ebx, 0
        je      0x42ee5
        cmp     byte ptr [ebp - 1], 0
        jne     0x42ecc
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 17: offset 0x40c, type byte [rec+6], draw-slot [entry+4] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x40c
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42f4b
        cmp     ecx, 0
        jl      0x42f4b
        mov     ebx, dword ptr [esi + 0x40c]
        movzx   ebx, byte ptr [ebx + 6]
        cmp     ebx, 4
        jle     0x42f4b
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 4]
        cmp     ebx, 0
        je      0x42f4b
        cmp     byte ptr [ebp - 1], 0
        jne     0x42f32
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 18: offset 0x408, type byte [rec+6], draw-slot [entry+0x14] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x408
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x42fb1
        cmp     ecx, 0
        jl      0x42fb1
        mov     ebx, dword ptr [esi + 0x408]
        movzx   ebx, byte ptr [ebx + 6]
        cmp     ebx, 4
        jle     0x42fb1
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0x14]
        cmp     ebx, 0
        je      0x42fb1
        cmp     byte ptr [ebp - 1], 0
        jne     0x42f98
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 19: offset 0x40c, type byte [rec+5], draw-slot [entry+0] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x40c
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x43016
        cmp     ecx, 0
        jl      0x43016
        mov     ebx, dword ptr [esi + 0x40c]
        movzx   ebx, byte ptr [ebx + 5]
        cmp     ebx, 4
        jle     0x43016
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx]
        cmp     ebx, 0
        je      0x43016
        cmp     byte ptr [ebp - 1], 0
        jne     0x42ffd
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 20: offset 0x408, type byte [rec+5], draw-slot [entry+0x10] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x408
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x4307c
        cmp     ecx, 0
        jl      0x4307c
        mov     ebx, dword ptr [esi + 0x408]
        movzx   ebx, byte ptr [ebx + 5]
        cmp     ebx, 4
        jle     0x4307c
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0x10]
        cmp     ebx, 0
        je      0x4307c
        cmp     byte ptr [ebp - 1], 0
        jne     0x43063
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 21: offset 0x208, type byte [rec+5], draw-slot [entry+8] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x208
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x430e2
        cmp     ecx, 0
        jl      0x430e2
        mov     ebx, dword ptr [esi + 0x208]
        movzx   ebx, byte ptr [ebx + 5]
        cmp     ebx, 4
        jle     0x430e2
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 8]
        cmp     ebx, 0
        je      0x430e2
        cmp     byte ptr [ebp - 1], 0
        jne     0x430c9
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 22: offset 0x408, type byte [rec+4], draw-slot [entry+0xc] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x408
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x43148
        cmp     ecx, 0
        jl      0x43148
        mov     ebx, dword ptr [esi + 0x408]
        movzx   ebx, byte ptr [ebx + 4]
        cmp     ebx, 4
        jle     0x43148
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0xc]
        cmp     ebx, 0
        je      0x43148
        cmp     byte ptr [ebp - 1], 0
        jne     0x4312f
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 23: offset 0x208, type byte [rec+4], draw-slot [entry+4] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x208
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x431ae
        cmp     ecx, 0
        jl      0x431ae
        mov     ebx, dword ptr [esi + 0x208]
        movzx   ebx, byte ptr [ebx + 4]
        cmp     ebx, 4
        jle     0x431ae
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 4]
        cmp     ebx, 0
        je      0x431ae
        cmp     byte ptr [ebp - 1], 0
        jne     0x43195
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 24: offset 0x204, type byte [rec+4], draw-slot [entry+0x14] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x204
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x43214
        cmp     ecx, 0
        jl      0x43214
        mov     ebx, dword ptr [esi + 0x204]
        movzx   ebx, byte ptr [ebx + 4]
        cmp     ebx, 4
        jle     0x43214
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0x14]
        cmp     ebx, 0
        je      0x43214
        cmp     byte ptr [ebp - 1], 0
        jne     0x431fb
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 25: offset 0x208, type byte [rec+3], draw-slot [entry+0] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x208
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x43279
        cmp     ecx, 0
        jl      0x43279
        mov     ebx, dword ptr [esi + 0x208]
        movzx   ebx, byte ptr [ebx + 3]
        cmp     ebx, 4
        jle     0x43279
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx]
        cmp     ebx, 0
        je      0x43279
        cmp     byte ptr [ebp - 1], 0
        jne     0x43260
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 26: offset 0x204, type byte [rec+3], draw-slot [entry+0x10] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x204
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x432df
        cmp     ecx, 0
        jl      0x432df
        mov     ebx, dword ptr [esi + 0x204]
        movzx   ebx, byte ptr [ebx + 3]
        cmp     ebx, 4
        jle     0x432df
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0x10]
        cmp     ebx, 0
        je      0x432df
        cmp     byte ptr [ebp - 1], 0
        jne     0x432c6
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 27: offset 0x004, type byte [rec+3], draw-slot [entry+8] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 4
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x4333f
        cmp     ecx, 0
        jl      0x4333f
        mov     ebx, dword ptr [esi + 4]
        movzx   ebx, byte ptr [ebx + 3]
        cmp     ebx, 4
        jle     0x4333f
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 8]
        cmp     ebx, 0
        je      0x4333f
        cmp     byte ptr [ebp - 1], 0
        jne     0x43326
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 28: offset 0x204, type byte [rec+2], draw-slot [entry+0xc] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0x204
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x433a5
        cmp     ecx, 0
        jl      0x433a5
        mov     ebx, dword ptr [esi + 0x204]
        movzx   ebx, byte ptr [ebx + 2]
        cmp     ebx, 4
        jle     0x433a5
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0xc]
        cmp     ebx, 0
        je      0x433a5
        cmp     byte ptr [ebp - 1], 0
        jne     0x4338c
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 29: offset 0x004, type byte [rec+2], draw-slot [entry+4] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 4
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x43405
        cmp     ecx, 0
        jl      0x43405
        mov     ebx, dword ptr [esi + 4]
        movzx   ebx, byte ptr [ebx + 2]
        cmp     ebx, 4
        jle     0x43405
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 4]
        cmp     ebx, 0
        je      0x43405
        cmp     byte ptr [ebp - 1], 0
        jne     0x433ec
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 30: offset 0x000, type byte [rec+2], draw-slot [entry+0x14] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x43464
        cmp     ecx, 0
        jl      0x43464
        mov     ebx, dword ptr [esi]
        movzx   ebx, byte ptr [ebx + 2]
        cmp     ebx, 4
        jle     0x43464
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0x14]
        cmp     ebx, 0
        je      0x43464
        cmp     byte ptr [ebp - 1], 0
        jne     0x4344b
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 31: offset 0x004, type byte [rec+1], draw-slot [entry+0] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 4
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x434c3
        cmp     ecx, 0
        jl      0x434c3
        mov     ebx, dword ptr [esi + 4]
        movzx   ebx, byte ptr [ebx + 1]
        cmp     ebx, 4
        jle     0x434c3
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx]
        cmp     ebx, 0
        je      0x434c3
        cmp     byte ptr [ebp - 1], 0
        jne     0x434aa
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 32: offset 0x000, type byte [rec+1], draw-slot [entry+0x10] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x43522
        cmp     ecx, 0
        jl      0x43522
        mov     ebx, dword ptr [esi]
        movzx   ebx, byte ptr [ebx + 1]
        cmp     ebx, 4
        jle     0x43522
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0x10]
        cmp     ebx, 0
        je      0x43522
        cmp     byte ptr [ebp - 1], 0
        jne     0x43509
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 33: offset -0x200, type byte [rec+1], draw-slot [entry+8] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0xfffffe00                   ; esi - 0x200 (one row back)
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x43588
        cmp     ecx, 0
        jl      0x43588
        mov     ebx, dword ptr [esi - 0x200]
        movzx   ebx, byte ptr [ebx + 1]
        cmp     ebx, 4
        jle     0x43588
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 8]
        cmp     ebx, 0
        je      0x43588
        cmp     byte ptr [ebp - 1], 0
        jne     0x4356f
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 34: offset 0x000, type byte [rec+0], draw-slot [entry+0xc] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x435e6
        cmp     ecx, 0
        jl      0x435e6
        mov     ebx, dword ptr [esi]
        movzx   ebx, byte ptr [ebx]
        cmp     ebx, 4
        jle     0x435e6
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0xc]
        cmp     ebx, 0
        je      0x435e6
        cmp     byte ptr [ebp - 1], 0
        jne     0x435cd
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 35: offset -0x200, type byte [rec+0], draw-slot [entry+4] ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0xfffffe00                   ; esi - 0x200 (one row back)
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     0x43647
        cmp     ecx, 0
        jl      0x43647
        mov     ebx, dword ptr [esi - 0x200]
        movzx   ebx, byte ptr [ebx]
        cmp     ebx, 4
        jle     0x43647
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 4]
        cmp     ebx, 0
        je      0x43647
        cmp     byte ptr [ebp - 1], 0
        jne     0x43632
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- cell 36: offset -0x204, type byte [rec+0], draw-slot [entry+0x14] (last cell) ---
        dec     edx
        jl      .walk_done
        mov     ecx, esi
        add     ecx, 0xfffffdfc                   ; esi - 0x204 (row back, one column left)
        sub     ecx, dword ptr [0x5358]          ; 0x5358=g_map_cols
        cmp     ecx, 0xc000
        jge     .walk_done                         ; last cell: skip target is the exit itself
        cmp     ecx, 0
        jl      .walk_done
        mov     ebx, dword ptr [esi - 0x204]
        movzx   ebx, byte ptr [ebx]
        cmp     ebx, 4
        jle     .walk_done
        imul    ebx, ebx, 0x18
        add     ebx, dword ptr [0x5360]
        mov     ebx, dword ptr [ebx + 0x14]
        cmp     ebx, 0
        je      .walk_done
        cmp     byte ptr [ebp - 1], 0
        jne     0x4368f
        call    0x46188                           ; -> clear_occlusion_mask
        add     byte ptr [ebp - 1], 1
        add     ebx, dword ptr [0x5360]
        call    0x418ac                           ; -> merge_cell_mask
        cmp     cx, 0
        jle     .walk_early_out

; --- exits ------------------------------------------------------------------------
.walk_done:                                        ; (0x436a4) all cells visited (or last rejected)
        mov     al, byte ptr [ebp - 1]            ; AL = "something drew" flag
        mov     ah, 0                              ; AH = 0: finished the whole walk
        leave
        ret
.walk_early_out:                                   ; (0x436ab) a merge signalled cx <= 0
        mov     al, byte ptr [ebp - 1]            ; AL = "something drew" flag
        mov     ah, 1                              ; AH = 1: stopped early
        leave
        ret
