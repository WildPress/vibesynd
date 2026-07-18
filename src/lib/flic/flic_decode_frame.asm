; flic_decode_frame @ 0x39e42  (160 bytes) -- hand-written assembly (fully commented).
;
; flic_decode_frame: decode one FLIC/FLC frame by walking its sub-chunks. A FLIC frame chunk
; (magic 0xF1FA, handled by flic_play) contains a count of sub-chunks followed by
; that many {size, type, data} records. This routine reads the count, then for each
; sub-chunk reads its 6-byte header and dispatches on the standard FLIC chunk type:
;
;   type 7  (FLI_SS2 / word-oriented delta) -> FUN_0003a033
;   type 4  (FLI_COLOR256 / 256-colour map) -> FUN_00039f69
;   type 15 (FLI_BRUN / byte run-length)    -> FUN_0003a10c
;   anything else (skip/unknown)            -> FUN_00039f49
;
; No args. Globals:
;   0xbde0  sub-chunk count remaining in this frame (word)
;   0xbdd0  g_flic_handle  open file handle
;   0x5370  g_back_buf     buffer the first read (the frame's own header word) lands in
;   0xbddc  sub-chunk size (dword)
;   0xbdd4  sub-chunk type (word)
; Calls: FUN_0003a7c4(handle, buf, count) -- read `count` bytes from the file.

flic_decode_frame:
        push    2                            ; read 2 bytes: the frame's sub-chunk count
        push    0xbde0                        ;   into 0xbde0
        push    dword ptr [0xbdd0]           ;   from g_flic_handle
        call    0x3a7c4                      ; -> FUN_0003a7c4
        add     esp, 0xc
        push    8                            ; read 8 more bytes (rest of the frame header)
        push    dword ptr [0x5370]           ;   into g_back_buf (discarded)
        push    dword ptr [0xbdd0]
        call    0x3a7c4                      ; -> FUN_0003a7c4
        add     esp, 0xc
subchunk_loop:
        mov     ax, word ptr [0xbde0]        ; sub-chunks left?
        cmp     ax, 0
        jne     have_subchunk
        jmp     done                         ;   none -> frame complete
have_subchunk:
        sub     word ptr [0xbde0], 1         ; consume one sub-chunk
        push    4                            ; read the 4-byte sub-chunk size...
        push    0xbddc                        ;   into 0xbddc
        push    dword ptr [0xbdd0]
        call    0x3a7c4                      ; -> FUN_0003a7c4
        add     esp, 0xc
        push    2                            ; read the 2-byte sub-chunk type...
        push    0xbdd4                        ;   into 0xbdd4
        push    dword ptr [0xbdd0]
        call    0x3a7c4                      ; -> FUN_0003a7c4
        add     esp, 0xc
        mov     ax, word ptr [0xbdd4]        ; ax = chunk type
        cmp     ax, 7                        ; FLI_SS2 (delta)?
        jne     chk_type4
        call    0x3a033                      ; -> FUN_0003a033: apply SS2 delta
        jmp     subchunk_loop
chk_type4:
        cmp     ax, 4                        ; FLI_COLOR256?
        jne     chk_type15
        call    0x39f69                      ; -> FUN_00039f69: load 256-colour palette
        jmp     subchunk_loop
chk_type15:
        cmp     ax, 0xf                      ; FLI_BRUN (byte run-length full frame)?
        jne     unknown_type
        call    0x3a10c                      ; -> FUN_0003a10c: decode BRUN
        jmp     subchunk_loop
unknown_type:
        call    0x39f49                      ; -> FUN_00039f49: skip unrecognised chunk
        jmp     subchunk_loop
done:
        ret
