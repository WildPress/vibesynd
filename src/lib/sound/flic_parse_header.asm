; flic_parse_header @ 0x39ee2  (71 bytes) -- hand-written assembly (fully commented).
;
; flic_parse_header: read and parse the rest of a FLIC/FLC file header. This lives in the
; sound module but is part of the animation player driven by flic_play (the magic
; word 0xAF12 identifies an Autodesk FLC file). The caller has already read the leading
; 6 bytes of the header (a 4-byte size + 2-byte type/magic); this routine slurps the
; remaining (size-6) bytes into g_back_buf and pulls out the three fields the player
; needs: frame count, width and height.
;
; No args. Globals:
;   0xbddc  chunk size (dword; low word used) of the FLC main header
;   0xbdd0  g_flic_handle  open file handle
;   0x5370  g_back_buf     scratch buffer the header body is read into
;   0xbdd6  frame count   (word)  <- header[0]
;   0xbdd8  width         (word)  <- header[2]
;   0xbdda  height        (word)  <- header[4]
; Calls: FUN_0003a7c4(handle, buf, count) -- read `count` bytes from the file.

flic_parse_header:
        movzx   eax, word ptr [0xbddc]       ; eax = FLC header size (low word)
        sub     eax, 6                        ; minus the 6 bytes already read
        push    eax                          ; count
        push    dword ptr [0x5370]           ; buffer = g_back_buf
        push    dword ptr [0xbdd0]           ; handle = g_flic_handle
        call    0x3a7c4                      ; -> FUN_0003a7c4: read the header body
        add     esp, 0xc
        mov     edi, dword ptr [0x5370]      ; edi -> g_back_buf (header body)
        mov     ax, word ptr [edi]           ; header[0] = frame count
        mov     word ptr [0xbdd6], ax
        add     edi, 2
        mov     ax, word ptr [edi]           ; header[2] = width
        mov     word ptr [0xbdd8], ax
        add     edi, 2
        mov     ax, word ptr [edi]           ; header[4] = height
        mov     word ptr [0xbdda], ax
        ret
