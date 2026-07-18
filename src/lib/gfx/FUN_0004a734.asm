; FUN_0004a734 @ 0x4a734  (356 bytes) -- hand-written assembly (fully commented).
;
; draw_sprite_rle: draw a run-length-encoded, masked (transparent) sprite into the
; 320x200 frame at edi, with clipping. This is the routine that stamps characters,
; vehicles, and objects, skipping their transparent pixels so they layer correctly.
;
; Registers in (a private, non-C convention -- another sign this is hand asm):
;   edi = destination buffer base       esi = sprite data (RLE stream)
;   ebx = x        ecx = y              dl = width      dh = height
; The four are halved on entry (>>1), so the caller passes doubled coordinates.
;
; Sprite encoding. The stream is a series of run bytes, row by row:
;   run == row-marker  -> end of this row, move to the next
;   run <  0           -> a transparent run: skip |run| pixels (leave the dest as is)
;   run >  0           -> an opaque run: copy the next `run` source bytes to the dest
; The transparent skip uses a neat trick: eax is pre-loaded with 0xffffffff, and
; `lodsb` only replaces the low byte, so a negative run byte becomes a negative
; dword and `sub edi, eax` advances the destination by exactly |run|.
;
; The routine has a fully-visible fast path and four edge-clipped variants (top,
; bottom, left, right), all running the same RLE loop with the edges trimmed. Row
; pitch is 0x140 (320). ebp holds the current row's start so each row can restart
; cleanly. Saves nothing itself; callers preserve what they need.

FUN_0004a734:
        sar     ebx, 1              ; x  >>= 1
        sar     ecx, 1              ; y  >>= 1
        shr     dl, 1               ; width  >>= 1
        shr     dh, 1               ; height >>= 1

; --- clip decision: pick the fast path, a clipped variant, or reject ---
        cmp     cx, 0xc8            ; y >= 200 ?
        jge     0x4a897             ;   -> fully below screen, done
        cmp     cx, 0
        jl      0x4a86b             ;   y < 0        -> top-clip variant
        mov     al, dh
        cbw
        add     ax, cx             ; y + height
        cmp     ax, 0xc8
        jge     0x4a889            ;   past bottom   -> bottom-clip variant
        cmp     bx, 0x140          ; x >= 320 ?
        jge     0x4a897            ;   -> fully right of screen, done
        cmp     bx, 0
        jl      0x4a7bf            ;   x < 0        -> left-clip variant
        mov     al, dl
        cbw
        add     ax, bx             ; x + width
        cmp     ax, 0x140
        jge     0x4a81d            ;   past right    -> right-clip variant

; --- fully-visible fast path ---
        add     edi, ebx           ; dest = base + x
        movsx   eax, cx
        mov     bh, dh             ; bh = row counter (height)
        imul    eax, eax, 0x140    ;   + y * 320
        add     edi, eax           ; dest = pixel address
        movzx   ecx, cl            ; clear ecx high bytes (rep movsb count = cl)
        mov     eax, 0xffffffff    ; preload for the sign-extend-on-lodsb trick
        mov     ebp, edi           ; ebp = start of this row
row:                               ; (0x4a79d)
        lodsb                      ; al = next run byte
        cmp     al, ch             ; row marker?
        je      next_row
        js      skip_run           ; negative -> transparent run
        mov     cl, al             ; opaque run length
        rep movsb                  ; copy `al` pixels
        jmp     row
skip_run:                          ; (0x4a7aa)
        sub     edi, eax           ; eax is negative here -> advance |run| pixels
        jmp     row
next_row:                          ; (0x4a7ae)
        add     ebp, 0x140         ; step to the next screen row
        mov     edi, ebp
        dec     bh
        jne     row
        jmp     0x4a897            ; done

; --- clipped variants ------------------------------------------------------------
; Each mirrors the fast-path RLE loop but trims one edge. They compute the visible
; span, adjust the destination pointer and the run counters at the clipped side,
; and then decode the same run/skip/row-marker stream. Left/right also split runs
; that straddle the screen edge; top/bottom skip whole rows of the stream (top-clip
; recomputes the starting height, bottom-clip caps it). The full decoded logic is
; left in instruction form below; the fast path above is the readable model for it.

; left-clip variant  (entered at 0x4a7bf)
        movzx   eax, dl
        add     eax, ebx
        js      0x4a897
        mov     eax, ecx
        mov     ch, dh
        imul    eax, eax, 0x140
        add     edi, eax
        mov     ebp, edi
        mov     cl, bl
        movzx   ebx, bl
        lodsb
        cmp     al, 0
        je      0x4a80d
        js      0x4a7f2
        mov     ah, al
        lodsb
        inc     bl
        jle     0x4a7ec
        stosb
        dec     ah
        jg      0x4a7e6
        jmp     0x4a7dd
        mov     bh, bl
        sub     bl, al
        jle     0x4a7dd
        cmp     bh, 0
        jge     0x4a804
        xor     bh, bh
        add     di, bx
        jmp     0x4a809
        movsx   eax, al
        sub     edi, eax
        xor     bh, bh
        jmp     0x4a7dd
        add     ebp, 0x140
        mov     edi, ebp
        mov     bl, cl
        dec     ch
        jne     0x4a7dd
        jmp     0x4a897

; right-clip variant  (entered at 0x4a81d)
        add     edi, ebx
        neg     ebx
        add     ebx, 0x140
        mov     eax, ecx
        mov     ecx, edx
        mov     cl, bl
        xor     edx, edx
        imul    eax, eax, 0x140
        add     edi, eax
        mov     ebp, edi
        lodsb
        cmp     al, 0
        je      0x4a85b
        js      0x4a84e
        mov     ah, al
        lodsb
        dec     bl
        js      0x4a848
        stosb
        dec     ah
        jg      0x4a842
        jmp     0x4a839
        add     bl, al
        jle     0x4a839
        cbw
        movsx   eax, ax
        sub     edi, eax
        jmp     0x4a839
        add     ebp, 0x140
        mov     edi, ebp
        mov     bl, cl
        dec     ch
        jne     0x4a839
        jmp     0x4a897

; bottom-clip: cap the visible height, then rejoin the loop
        movzx   eax, dh
        add     eax, ecx
        jle     0x4a897
        mov     dh, al
        xor     ah, ah
        lodsb
        cmp     al, 0
        je      0x4a881
        js      0x4a876
        add     esi, eax
        jmp     0x4a876
        inc     ecx
        jne     0x4a876
        jmp     0x4a762

; top-clip: skip clipped rows by recomputing the starting height
        mov     eax, 0xc8
        sub     eax, ecx
        mov     dh, al
        jmp     0x4a762

        ret                        ; (0x4a897) done
