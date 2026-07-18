; glyph_stamp_fixed @ 0004ce87  (421 bytes) -- hand-written assembly (fully commented).
;
; glyph_stamp_fixed: stamp one fixed-size font glyph into the four-plane offscreen
; buffer. Called by the fixed-width text renderer draw_text_fixed.
;
; The glyph is byte-aligned and 8 pixels wide (one byte per plane per row). Its source
; data at edx is stored plane-interleaved: for each row it holds four consecutive
; bytes -- the plane-0, plane-1, plane-2 and plane-3 pixel bytes -- so a row is copied
; by four reads and four writes to the planes at +0, +0x7d00, +0xfa00 and +0x17700 of
; the destination. After each row the destination steps 0x50 (80) bytes to the next
; planar row. The whole thing is fully unrolled for 12 rows (no loop counter), which,
; with the plane offsets baked into the instructions, is why this is hand assembly.
;
; Registers in:  edx = glyph source (plane-interleaved bytes, advanced by 48)
;                edi = destination screen address (plane 0)
; Globals: none directly (offsets are the plane spacing of g_screen_buf).
; The build uses glyph_stamp_fixed.c (db-transcription); this is the readable companion.
;
glyph_stamp_fixed:
        mov     al, byte ptr [edx]               ; 8a02       -- al = next source byte (plane 0)
        inc     edx                              ; 42
        mov     byte ptr [edi], al               ; 8807         -- write to plane 0
        mov     al, byte ptr [edx]               ; 8a02       -- al = next source byte (plane 1)
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x7d00], al      ; 8887007d0000 -- write to plane 1
        mov     al, byte ptr [edx]               ; 8a02       -- al = next source byte (plane 2)
        inc     edx                              ; 42
        mov     byte ptr [edi + 0xfa00], al      ; 888700fa0000 -- write to plane 2
        mov     al, byte ptr [edx]               ; 8a02       -- al = next source byte (plane 3)
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x17700], al     ; 888700770100 -- write to plane 3
        add     edi, 0x50                        ; 83c750     -- next planar row (row 1 of 12 done)
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi], al               ; 8807
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x7d00], al      ; 8887007d0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0xfa00], al      ; 888700fa0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x17700], al     ; 888700770100
        add     edi, 0x50                        ; 83c750     -- next planar row (row 2)
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi], al               ; 8807
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x7d00], al      ; 8887007d0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0xfa00], al      ; 888700fa0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x17700], al     ; 888700770100
        add     edi, 0x50                        ; 83c750     -- next planar row (row 3)
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi], al               ; 8807
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x7d00], al      ; 8887007d0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0xfa00], al      ; 888700fa0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x17700], al     ; 888700770100
        add     edi, 0x50                        ; 83c750     -- next planar row (row 4)
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi], al               ; 8807
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x7d00], al      ; 8887007d0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0xfa00], al      ; 888700fa0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x17700], al     ; 888700770100
        add     edi, 0x50                        ; 83c750     -- next planar row (row 5)
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi], al               ; 8807
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x7d00], al      ; 8887007d0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0xfa00], al      ; 888700fa0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x17700], al     ; 888700770100
        add     edi, 0x50                        ; 83c750     -- next planar row (row 6)
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi], al               ; 8807
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x7d00], al      ; 8887007d0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0xfa00], al      ; 888700fa0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x17700], al     ; 888700770100
        add     edi, 0x50                        ; 83c750     -- next planar row (row 7)
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi], al               ; 8807
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x7d00], al      ; 8887007d0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0xfa00], al      ; 888700fa0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x17700], al     ; 888700770100
        add     edi, 0x50                        ; 83c750     -- next planar row (row 8)
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi], al               ; 8807
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x7d00], al      ; 8887007d0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0xfa00], al      ; 888700fa0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x17700], al     ; 888700770100
        add     edi, 0x50                        ; 83c750     -- next planar row (row 9)
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi], al               ; 8807
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x7d00], al      ; 8887007d0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0xfa00], al      ; 888700fa0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x17700], al     ; 888700770100
        add     edi, 0x50                        ; 83c750     -- next planar row (row 10)
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi], al               ; 8807
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x7d00], al      ; 8887007d0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0xfa00], al      ; 888700fa0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x17700], al     ; 888700770100
        add     edi, 0x50                        ; 83c750     -- next planar row (row 11)
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi], al               ; 8807
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x7d00], al      ; 8887007d0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0xfa00], al      ; 888700fa0000
        mov     al, byte ptr [edx]               ; 8a02
        inc     edx                              ; 42
        mov     byte ptr [edi + 0x17700], al     ; 888700770100
        add     edi, 0x50                        ; 83c750     -- next planar row (row 12)
        ret                                      ; c3
