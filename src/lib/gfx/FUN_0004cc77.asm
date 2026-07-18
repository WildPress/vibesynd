; FUN_0004cc77 @ 0004cc77  (161 bytes) -- hand-written assembly (fully commented).
;
; glyph_blit_wide: paint one glyph column up to 16 pixels wide into the four-plane
; offscreen buffer, in a chosen 4-bit colour, shifted to an arbitrary x bit position.
;
; The offscreen buffer is the planar 16-colour layout: four planes 0x7d00 bytes
; apart, each plane an 0x50-byte-per-row (80) bitmap; a byte holds 8 horizontal
; pixels and the four planes at one address give a pixel its 4 colour bits. To draw
; a colour, for each plane you OR the glyph bits in where that plane's colour bit is
; 1, and AND them out where it is 0. That per-plane set/clear choice is what si (the
; colour, 0..15) selects below.
;
; Each glyph row is a 16-bit mask read from [edi]. It is rotated right by (x & 7) and
; byte-reordered into a 32-bit value so a row shifted across a byte boundary still
; lands correctly, then written to a dword (32 px) at the destination. The loop runs
; once per row and steps 0x50 bytes down.
;
; Registers in:  ax = x        bx = y        cx = row count (height)
;                dx = colour/plane-select (0..15)     edi = glyph row data (words)
; Globals: 0x5368 g_screen_buf (plane 0 base). The build uses FUN_0004cc77.c
; (db-transcription); this is the readable companion.
;
FUN_0004cc77:
        push    esi                              ; 56
        mov     si, dx                           ; 668bf2     -- si = colour / per-plane set-mask
        imul    bx, bx, 0x50                     ; 666bdb50   -- bx = y * 0x50 (row pitch)
        push    ax                               ; 6650       -- save x
        sar     ax, 3                            ; 66c1f803   -- ax = x / 8 (byte within the row)
        add     bx, ax                           ; 6603d8     -- bx = y*0x50 + x/8
        and     ebx, 0xffff                      ; 81e3ffff0000  -- keep offset 16-bit
        add     ebx, dword ptr [0x5368]          ; 031d68530000  -- + g_screen_buf -> plane-0 dest address
        pop     ax                               ; 6658       -- restore x
        and     ax, 7                            ; 6683e007   -- ax = x & 7 (bit offset within the byte)
        xchg    cx, ax                           ; 6691       -- cl = shift (x&7), ax = row count (height)
row_loop:                                        ;            <- (0x4cc9c)
        mov     edx, 0                           ; ba00000000
        mov     dx, word ptr [edi]               ; 668b17     -- dx = next 16-bit glyph row
        add     edi, 2                           ; 83c702     -- advance glyph pointer
        ror     edx, cl                          ; d3ca       -- shift row to its x bit position
        xchg    dh, dl                           ; 86d6       -- byte-reorder so the shifted mask ...
        rol     edx, 0x10                        ; c1c210     --   ... lays out correctly across ...
        xchg    dh, dl                           ; 86d6       --   ... up to four destination bytes ...
        rol     edx, 0x10                        ; c1c210     --   ... (little-endian dword span)
; plane 0
        test    si, 1                            ; 66f7c60100 -- colour bit 0 set?
        je      0x4ccbe                          ; 7404       -- no -> clear these pixels in plane 0
        or      dword ptr [ebx], edx             ; 0913       -- yes -> set glyph pixels in plane 0
        jmp     0x4ccc4                          ; eb06
        not     edx                              ; f7d2       ; (0x4ccbe)
        and     dword ptr [ebx], edx             ; 2113       -- clear glyph pixels in plane 0
        not     edx                              ; f7d2       -- restore edx for the next plane
; plane 1  (+0x7d00)
        test    si, 2                            ; 66f7c60200 ; (0x4ccc4) colour bit 1 set?
        je      0x4ccd3                          ; 7408
        or      dword ptr [ebx + 0x7d00], edx    ; 0993007d0000  -- set in plane 1
        jmp     0x4ccdd                          ; eb0a
        not     edx                              ; f7d2       ; (0x4ccd3)
        and     dword ptr [ebx + 0x7d00], edx    ; 2193007d0000  -- clear in plane 1
        not     edx                              ; f7d2
; plane 2  (+0xfa00)
        test    si, 4                            ; 66f7c60400 ; (0x4ccdd) colour bit 2 set?
        je      0x4ccec                          ; 7408
        or      dword ptr [ebx + 0xfa00], edx    ; 099300fa0000  -- set in plane 2
        jmp     0x4ccf6                          ; eb0a
        not     edx                              ; f7d2       ; (0x4ccec)
        and     dword ptr [ebx + 0xfa00], edx    ; 219300fa0000  -- clear in plane 2
        not     edx                              ; f7d2
; plane 3  (+0x17700)
        test    si, 8                            ; 66f7c60800 ; (0x4ccf6) colour bit 3 set?
        je      0x4cd05                          ; 7408
        or      dword ptr [ebx + 0x17700], edx   ; 099300770100  -- set in plane 3
        jmp     0x4cd0f                          ; eb0a
        not     edx                              ; f7d2       ; (0x4cd05)
        and     dword ptr [ebx + 0x17700], edx   ; 219300770100  -- clear in plane 3
        not     edx                              ; f7d2
        add     ebx, 0x50                        ; 83c350     ; (0x4cd0f) next screen row
        dec     ax                               ; 6648       -- one row done
        jne     0x4cc9c                          ; 7586       -- more rows -> row_loop
        pop     esi                              ; 5e
        ret                                      ; c3
