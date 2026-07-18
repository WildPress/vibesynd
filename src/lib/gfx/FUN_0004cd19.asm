; FUN_0004cd19 @ 0004cd19  (37 bytes) -- hand-written assembly (fully commented).
;
; glyph_dispatch_narrow: the narrow-glyph counterpart of FUN_0004cc4d. Reads the
; render-mode flags at 0x105 and:
;   bit 1 -> planar 16-colour offscreen buffer: call glyph_blit_narrow (FUN_0004cd3e)
;   bit 2 -> nothing to draw, just return
;   bit 0 -> just return
; Glyph registers (ax=x, bx=y, cx=height, dx=colour, edi=glyph data) pass through.
;
; Globals/ports: 0x105 render-mode flags. The build uses FUN_0004cd19.c
; (db-transcription); this is the readable companion.
;
FUN_0004cd19:
        test    byte ptr [0x105], 2              ; f6050501000002  -- planar mode?
        je      0x4cd29                          ; 7407       -- no -> try next mode (-> try_bit2)
        call    0x4cd3e                          ; e817000000  -- FUN_0004cd3e: draw narrow glyph (planar)
        jmp     0x4cd3d                          ; eb14       -- done (-> ret)
try_bit2:                                        ;            <- (0x4cd29)
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x4cd34                          ; 7402       -- (-> try_bit0)
        jmp     0x4cd3d                          ; eb09       -- nothing to draw -> done (-> ret)
try_bit0:                                        ;            <- (0x4cd34)
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x4cd3d                          ; 7400       -- (-> ret)
ret:                                             ;            <- (0x4cd3d)
        ret                                      ; c3
