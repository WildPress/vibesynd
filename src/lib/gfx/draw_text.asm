; draw_text @ 0004cb68  (227 bytes) -- hand-written assembly (fully commented).
;
; draw_text: render a null-terminated string in a proportional (variable-width) font
; into the offscreen buffer, advancing a pen position character by character and
; handling carriage-return (0x0d) as a new line.
;
; Font layout. [ebp+0xc] points at a table of 5-byte glyph descriptors indexed by
; character code:
;     word[0]  offset (relative to the font base) of this glyph's pixel rows
;     byte[2]  advance width, in pixels
;     byte[3]  row count (glyph height); a value <= 0 means "draw nothing"
;     byte[4]  vertical offset added to the pen y before drawing
; A word[0] of 0 (e.g. space) also draws nothing but still advances the pen.
;
; For each character the pen x is read, the glyph descriptor fetched, x advanced by
; (width + 1) for the inter-character gap, and the glyph drawn: glyphs wider than 8
; pixels go through the wide dispatcher (glyph_dispatch_wide), narrower ones through the
; narrow dispatcher (glyph_dispatch_narrow), which in turn select the per-mode renderer. The
; colour ([ebp+0x18]) is passed through in dx. A 0x0d resets x to the line start
; (saved at [ebp-2]) and advances y by the line height (0xc planar / 6 half-res). In
; the half-resolution mode (render bit 1 clear) the incoming x and y are halved first.
;
; Args (stack / cdecl):  [ebp+8] string   [ebp+0xc] font table   [ebp+0x10] x (word)
;                        [ebp+0x14] y (word)   [ebp+0x18] colour (word)
; Globals/ports: 0x105 render-mode flags. Callers include the number-drawing
; wrappers draw_hex (hex) and draw_dec (decimal).
; The build uses draw_text.c (db-transcription); this is the readable companion.
;
draw_text:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        add     esp, -4                          ; 83c4fc     -- local: [ebp-2] saved line-start x
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    edi                              ; 57
        push    esi                              ; 56
        test    byte ptr [0x105], 2              ; f6050501000002  -- planar mode?
        jne     have_string                      ; 750d       -- yes -> keep coords as-is
        test    byte ptr [0x105], 4              ; f6050501000004  -- other draw mode?
        je      mode_tail                        ; 0f84b4000000  -- bit 2 clear -> mode dispatch tail (else fall through, coords halved below)
have_string:                                     ;            <- (0x4cb89)
        mov     esi, dword ptr [ebp + 8]         ; 8b7508     -- esi = string pointer
        test    byte ptr [0x105], 2              ; f6050501000002  -- planar mode?
        jne     coords_ready                     ; 7508       -- yes -> full-res coords
        shr     word ptr [ebp + 0x10], 1         ; 66d16d10   -- half-res: x /= 2
        shr     word ptr [ebp + 0x14], 1         ; 66d16d14   -- half-res: y /= 2
coords_ready:                                    ;            <- (0x4cb9d)
        mov     ax, word ptr [ebp + 0x10]        ; 668b4510   -- ax = pen x
        mov     word ptr [ebp - 2], ax           ; 668945fe   -- remember line-start x for newline
        mov     ax, word ptr [ebp + 0x18]        ; 668b4518   -- normalise colour arg into place
        mov     word ptr [ebp + 0x18], ax        ; 66894518
next_char:                                       ;            <- (0x4cbad) main loop
        movzx   edi, byte ptr [esi]              ; 0fb63e     -- edi = next character
        inc     esi                              ; 46
        cmp     edi, 0                           ; 83ff00
        je      done                             ; 0f848c000000  -- 0 terminator -> finished
        cmp     edi, 0xd                          ; 83ff0d
        je      newline                          ; 745a       -- carriage return -> new line
        imul    edi, edi, 5                      ; 6bff05     -- edi = char * 5 (descriptor stride)
        add     edi, dword ptr [ebp + 0xc]       ; 037d0c     -- + font table -> glyph descriptor
        mov     ax, word ptr [ebp + 0x10]        ; 668b4510   -- ax = pen x
        mov     bx, word ptr [ebp + 0x14]        ; 668b5d14   -- bx = pen y
        movzx   cx, byte ptr [edi + 4]           ; 660fb64f04 -- cx = descriptor[4] (y offset)
        add     bx, cx                           ; 6603d9     -- draw y = pen y + y offset
        movzx   cx, byte ptr [edi + 3]           ; 660fb64f03 -- cx = descriptor[3] (row count / height)
        movzx   dx, byte ptr [edi + 2]           ; 660fb65702 -- dx = descriptor[2] (advance width)
        add     word ptr [ebp + 0x10], dx        ; 66015510   -- pen x += width
        add     word ptr [ebp + 0x10], 1         ; 6683451001 -- pen x += 1 (inter-character gap)
        movzx   edi, word ptr [edi]              ; 0fb73f     -- edi = descriptor[0] (glyph pixel offset)
        cmp     edi, 0                           ; 83ff00
        jle     cont                             ; 7e27       -- no pixels (e.g. space) -> skip drawing
        cmp     cx, 0                            ; 6683f900
        jle     cont                             ; 7e21       -- non-positive height -> skip drawing
        add     edi, dword ptr [ebp + 0xc]       ; 037d0c     -- edi = font base + offset -> glyph rows
        sar     dx, 3                            ; 66c1fa03   -- dx = width / 8
        cmp     dx, 0                             ; 6683fa00
        je      draw_narrow                      ; 740b       -- width < 8 -> narrow renderer
        mov     dx, word ptr [ebp + 0x18]        ; 668b5518   -- dx = colour
        call    0x4cc4d                          ; e841000000  -- glyph_dispatch_wide: draw wide glyph
        jmp     cont                             ; eb09
draw_narrow:                                     ;            <- (0x4cc0e)
        mov     dx, word ptr [ebp + 0x18]        ; 668b5518   -- dx = colour
        call    0x4cd19                          ; e802010000  -- glyph_dispatch_narrow: draw narrow glyph
cont:                                            ;            <- (0x4cc17)
        jmp     next_char                        ; eb94       -- next character
newline:                                         ;            <- (0x4cc19)
        mov     ax, word ptr [ebp - 2]           ; 668b45fe   -- restore x to the line start
        mov     word ptr [ebp + 0x10], ax        ; 66894510
        test    byte ptr [0x105], 2              ; f6050501000002  -- planar mode?
        je      nl_halfres                       ; 7407
        add     word ptr [ebp + 0x14], 0xc       ; 668345140c -- planar: y += 12 (line height)
        jmp     nl_done                          ; eb05
nl_halfres:                                      ;            <- (0x4cc31)
        add     word ptr [ebp + 0x14], 6         ; 6683451406 -- half-res: y += 6 (line height)
nl_done:                                         ;            <- (0x4cc36)
        jmp     next_char                        ; e972ffffff -- continue with the next character
        jmp     done                             ; eb09       ; (0x4cc3b) unreached tail from the mode dispatch
mode_tail:                                       ;            <- (0x4cc3d) render-mode dispatch (bit0 path)
        test    byte ptr [0x105], 1              ; f6050501000001
        je      done                             ; 7400
done:                                            ;            <- (0x4cc46)
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
; (transcription ends here; the matching frame teardown -- leave/ret -- follows just
;  past the transcribed range and is not shown in this listing.)
