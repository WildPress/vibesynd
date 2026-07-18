; glyph_dispatch_wide @ 0004cc4d  (42 bytes) -- hand-written assembly (fully commented).
;
; glyph_dispatch_wide: pick the wide-glyph renderer for the current render mode. The
; render-mode flags at 0x105 select which target buffer is live:
;   bit 1 -> planar 16-colour offscreen buffer: call glyph_blit_wide (glyph_blit_wide)
;   bit 2 -> call FUN_0004cd18 (a 1-byte no-op/ret stub -- nothing to draw in that mode)
;   bit 0 -> just return
; The glyph registers (ax=x, bx=y, cx=height, dx=colour, edi=glyph data) are passed
; straight through to whichever renderer is chosen.
;
; Globals/ports: 0x105 render-mode flags. The build uses glyph_dispatch_wide.c
; (db-transcription); this is the readable companion.
;
glyph_dispatch_wide:
        test    byte ptr [0x105], 2              ; f6050501000002  -- planar mode?
        je      0x4cc5d                          ; 7407       -- no -> try next mode (-> try_bit2)
        call    0x4cc77                          ; e81c000000  -- glyph_blit_wide: draw wide glyph (planar)
        jmp     0x4cc76                          ; eb19       -- done (-> ret)
try_bit2:                                        ;            <- (0x4cc5d)
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x4cc6d                          ; 7407       -- no -> try next mode (-> try_bit0)
        call    0x4cd18                          ; e8ad000000  -- FUN_0004cd18: no-op stub for this mode
        jmp     0x4cc76                          ; eb09       -- done (-> ret)
try_bit0:                                        ;            <- (0x4cc6d)
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x4cc76                          ; 7400       -- (-> ret)
ret:                                             ;            <- (0x4cc76)
        ret                                      ; c3
