; FUN_0004cd3e @ 0004cd3e  (164 bytes) -- hand-written assembly (fully commented).
;
; glyph_blit_narrow: the up-to-8-pixel-wide sibling of glyph_blit_wide (FUN_0004cc77).
; Same four-plane 16-colour offscreen layout and same per-plane set/clear-by-colour
; scheme, but each glyph row is a single byte and the shifted result is written as a
; 16-bit word (a byte plus its shift can only reach into one neighbouring byte).
;
; Each row byte is read from [edi], zero-extended into dx and rotated right by (x & 7)
; so a glyph that straddles a byte boundary still lands correctly across the word.
; Then for each of the four planes it ORs the pixels in where the colour's bit is set
; and ANDs them out where it is clear. The loop runs once per row, stepping 0x50 down.
;
; Registers in:  ax = x        bx = y        cx = row count (height)
;                dx = colour/plane-select (0..15)     edi = glyph row data (bytes)
; Globals: 0x5368 g_screen_buf (plane 0 base). The build uses FUN_0004cd3e.c
; (db-transcription); this is the readable companion.
;
FUN_0004cd3e:
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
row_loop:                                        ;            <- (0x4cd63)
        mov     dx, 0                            ; 66ba0000
        mov     dl, byte ptr [edi]               ; 8a17       -- dl = next 8-bit glyph row
        inc     edi                              ; 47         -- advance glyph pointer
        ror     dx, cl                           ; 66d3ca     -- shift row to its x bit position (within a word)
; plane 0
        test    si, 1                            ; 66f7c60100 -- colour bit 0 set?
        je      0x4cd79                          ; 7405       -- no -> clear these pixels in plane 0
        or      word ptr [ebx], dx               ; 660913     -- yes -> set glyph pixels in plane 0
        jmp     0x4cd82                          ; eb09
        not     dx                               ; 66f7d2     ; (0x4cd79)
        and     word ptr [ebx], dx               ; 662113     -- clear glyph pixels in plane 0
        not     dx                               ; 66f7d2     -- restore dx for the next plane
; plane 1  (+0x7d00)
        test    si, 2                            ; 66f7c60200 ; (0x4cd82) colour bit 1 set?
        je      0x4cd92                          ; 7409
        or      word ptr [ebx + 0x7d00], dx      ; 660993007d0000  -- set in plane 1
        jmp     0x4cd9f                          ; eb0d
        not     dx                               ; 66f7d2     ; (0x4cd92)
        and     word ptr [ebx + 0x7d00], dx      ; 662193007d0000  -- clear in plane 1
        not     dx                               ; 66f7d2
; plane 2  (+0xfa00)
        test    si, 4                            ; 66f7c60400 ; (0x4cd9f) colour bit 2 set?
        je      0x4cdaf                          ; 7409
        or      word ptr [ebx + 0xfa00], dx      ; 66099300fa0000  -- set in plane 2
        jmp     0x4cdbc                          ; eb0d
        not     dx                               ; 66f7d2     ; (0x4cdaf)
        and     word ptr [ebx + 0xfa00], dx      ; 66219300fa0000  -- clear in plane 2
        not     dx                               ; 66f7d2
; plane 3  (+0x17700)
        test    si, 8                            ; 66f7c60800 ; (0x4cdbc) colour bit 3 set?
        je      0x4cdcc                          ; 7409
        or      word ptr [ebx + 0x17700], dx     ; 66099300770100  -- set in plane 3
        jmp     0x4cdd9                          ; eb0d
        not     dx                               ; 66f7d2     ; (0x4cdcc)
        and     word ptr [ebx + 0x17700], dx     ; 66219300770100  -- clear in plane 3
        not     dx                               ; 66f7d2
        add     ebx, 0x50                        ; 83c350     ; (0x4cdd9) next screen row
        dec     ax                               ; 6648       -- one row done
        jne     0x4cd63                          ; 7583       -- more rows -> row_loop
        pop     esi                              ; 5e
        ret                                      ; c3
