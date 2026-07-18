; FUN_0004cde3 @ 0004cde3  (162 bytes) -- hand-written assembly (fully commented).
;
; draw_text_fixed: render a string in a fixed-width 8-pixel bitmap font into the
; planar offscreen buffer, up to a maximum character count, handling carriage-return
; (0x0d) as a new line. This is the byte-aligned, monospaced counterpart to the
; proportional draw_text (FUN_0004cb68).
;
; The font pixel data lives at [0x10abc]; [ebp+0x14] selects a font page 0x2800 bytes
; apart. Each glyph occupies a 64-byte cell reached as (char - 0x20) * 64, so codes
; below space are not drawable. The glyph itself is stamped by FUN_0004ce87.
;
; Screen addressing uses the planar layout (row pitch 0x50, plane 0 at g_screen_buf).
; The pen is kept byte-aligned: each character advances the cursor by one byte (8
; pixels) and the sub-byte part of x (cl = x & 7) is computed but not used, since the
; glyph blit is byte-aligned. A newline advances the line start by 0x280 bytes and
; resets the cursor to it. Only runs when render bit 1 (planar) is set; the other
; modes fall through to the epilogue.
;
; Locals:  [ebp-0x10] glyph-data base (font page)   [ebp-0xc] line-start screen addr
;          [ebp-8] current cursor screen addr
; Args (stack / cdecl):  [ebp+8] string   [ebp+0xc] x (word)   [ebp+0x10] y (word)
;                        [ebp+0x14] font page (word)   [ebp+0x18] max chars (word)
; Globals: 0x10abc font pixel data base, 0x5368 g_screen_buf, 0x105 render-mode flags.
; The build uses FUN_0004cde3.c (db-transcription); this is the readable companion.
;
; Note: the glyph blit FUN_0004ce87 writes 12 rows, while a newline here advances the
; line start by only 0x280 (8 planar rows); the exact intended line spacing is not
; fully pinned down by this routine alone.
;
FUN_0004cde3:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        add     esp, -0x18                       ; 83c4e8     -- reserve locals
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    edi                              ; 57
        push    esi                              ; 56
        mov     esi, dword ptr [ebp + 8]         ; 8b7508     -- esi = string pointer
        mov     eax, dword ptr [0x10abc]         ; a1bc0a0100 -- eax = font pixel-data base
        mov     dword ptr [ebp - 0x10], eax      ; 8945f0     -- store glyph-data base
        test    byte ptr [0x105], 2              ; f6050501000002  -- planar mode?
        je      0x4ce6c                          ; 746a       -- no -> mode dispatch tail (-> mode_tail)
        movzx   eax, word ptr [ebp + 0xc]        ; 0fb7450c   -- eax = x
        movzx   ebx, word ptr [ebp + 0x10]       ; 0fb75d10   -- ebx = y
        mov     cl, al                           ; 8ac8
        and     cl, 7                            ; 80e107     -- cl = x & 7 (computed; blit is byte-aligned)
        shr     eax, 3                           ; c1e803     -- eax = x / 8 (byte within the row)
        imul    ebx, ebx, 0x50                   ; 6bdb50     -- ebx = y * 0x50 (row pitch)
        add     ebx, eax                         ; 03d8       -- ebx = y*0x50 + x/8
        add     ebx, dword ptr [0x5368]          ; 031d68530000  -- + g_screen_buf -> screen address
        mov     dword ptr [ebp - 0xc], ebx       ; 895df4     -- line-start screen address
        mov     dword ptr [ebp - 8], ebx         ; 895df8     -- cursor screen address
        movzx   eax, word ptr [ebp + 0x14]       ; 0fb74514   -- eax = font page index
        imul    eax, eax, 0x2800                 ; 69c000280000  -- * 0x2800 (bytes per font page)
        add     dword ptr [ebp - 0x10], eax      ; 0145f0     -- glyph base += page offset
next_char:                                       ;            <- (0x4ce30) main loop
        movzx   edx, byte ptr [esi]              ; 0fb616     -- edx = next character
        inc     esi                              ; 46
        cmp     edx, 0                           ; 83fa00
        je      0x4ce80                          ; 7447       -- 0 terminator -> finished (-> done)
        cmp     edx, 0xd                          ; 83fa0d
        je      0x4ce5a                          ; 741c       -- carriage return -> new line (-> newline)
        sub     edx, 0x20                        ; 83ea20     -- glyph index = char - ' '
        shl     edx, 6                           ; c1e206     -- * 64 (bytes per glyph cell)
        add     edx, dword ptr [ebp - 0x10]      ; 0355f0     -- + glyph base -> glyph pixels
        mov     edi, dword ptr [ebp - 8]         ; 8b7df8     -- edi = cursor screen address
        call    0x4ce87                          ; e838000000  -- FUN_0004ce87: stamp the glyph (4 planes)
        inc     dword ptr [ebp - 8]              ; ff45f8     -- cursor += 1 byte (8 pixels)
advance:                                         ;            <- (0x4ce52)
        dec     word ptr [ebp + 0x18]            ; 66ff4d18   -- one fewer of the max character count
        jne     0x4ce30                          ; 75d8       -- more to draw -> next_char
        jmp     0x4ce80                          ; eb26       -- count exhausted -> done
newline:                                         ;            <- (0x4ce5a)
        mov     eax, dword ptr [ebp - 0xc]       ; 8b45f4     -- eax = line start
        add     eax, 0x280                       ; 0580020000 -- next line (advance 0x280 bytes)
        mov     dword ptr [ebp - 0xc], eax       ; 8945f4     -- update line start
        mov     dword ptr [ebp - 8], eax         ; 8945f8     -- cursor back to line start
        jmp     0x4ce52                          ; ebe8       -- count the newline too (-> advance)
        jmp     0x4ce80                          ; eb14       ; (0x4ce6a) unreached tail from the mode dispatch
mode_tail:                                       ;            <- (0x4ce6c) render-mode dispatch
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x4ce77                          ; 7402       -- (-> try_bit0)
        jmp     0x4ce80                          ; eb09       -- (-> done)
try_bit0:                                        ;            <- (0x4ce77)
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x4ce80                          ; 7400       -- (-> done)
done:                                            ;            <- (0x4ce80)
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
; (transcription ends here; the matching frame teardown -- leave/ret -- follows just
;  past the transcribed range and is not shown in this listing.)
