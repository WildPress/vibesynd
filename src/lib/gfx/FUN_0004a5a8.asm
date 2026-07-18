; FUN_0004a5a8 @ 0x4a5a8  (145 bytes) -- hand-written assembly (fully commented).
;
; FUN_0004a5a8: present the whole offscreen frame to VGA using string moves. This is
; the planar-present implementation called by the dispatcher FUN_0004a574.
;
; It first resets the Graphics Controller to plain write mode (Bit Mask 0xff, Mode 0
; via port 0x3ce). Then it runs 0x32 (50) outer passes. Each pass copies, for all
; four planes, 0x140 (320) words = 640 bytes from g_screen_buf to VGA (0x536c),
; selecting the plane through the Sequencer Map Mask (0x0102/0x0202/0x0402/0x0802).
; Between planes edi rewinds 0x280 (640) so all four planes write the same 640-byte
; span, while esi advances 0x7a80 within a plane plus 0x280 from the movsw to reach
; the next plane's slice (0x7a80 + 0x280 = 0x7d00, the plane stride). After plane 3
; esi rewinds 0x17700 (3 planes) leaving it, and edi, advanced by 640 for the next
; pass. 50 passes * 640 bytes = 0x7d00, one full plane, so the whole frame is shown.
;
; No args. Globals: 0x5368 g_screen_buf   0x536c VGA base
; Ports: 0x3ce/0x3cf GC index/data   0x3c4/0x3c5 Sequencer (Map Mask = idx 2)
;
FUN_0004a5a8:
        push    ebp
        mov     ebp, esp
        add     esp, -8                          ; reserve 8 scratch bytes (unused frame slack)
        cld
        mov     dx, 0x3ce                        ; GC index port
        mov     ax, 0xff08                       ; Bit Mask (idx 8) <- 0xff
        out     dx, ax
        mov     ax, 5                            ; Mode (idx 5) <- 0 : plain writes
        out     dx, ax
        mov     dl, 0xc4                         ; dx = 0x3c4 (Sequencer index)
        mov     esi, dword ptr [0x5368]          ; esi = g_screen_buf
        mov     edi, dword ptr [0x536c]          ; edi = VGA base
        mov     ebx, 0x32                        ; 50 outer passes
plane_pass:
        mov     ax, 0x102                        ; Map Mask <- plane 0
        out     dx, ax
        mov     ecx, 0x140                       ; 320 words = 640 bytes
        rep movsw word ptr es:[edi], word ptr [esi]
        sub     edi, 0x280                       ; rewind dest 640 (all planes share it)
        add     esi, 0x7a80                      ; step source to plane 1 (+0x280 movsw = 0x7d00)
        mov     ax, 0x202                        ; Map Mask <- plane 1
        out     dx, ax
        mov     ecx, 0x140
        rep movsw word ptr es:[edi], word ptr [esi]
        sub     edi, 0x280
        add     esi, 0x7a80                      ; step source to plane 2
        mov     ax, 0x402                        ; Map Mask <- plane 2
        out     dx, ax
        mov     ecx, 0x140
        rep movsw word ptr es:[edi], word ptr [esi]
        sub     edi, 0x280
        add     esi, 0x7a80                      ; step source to plane 3
        mov     ax, 0x802                        ; Map Mask <- plane 3
        out     dx, ax
        mov     ecx, 0x140
        rep movsw word ptr es:[edi], word ptr [esi]
        sub     esi, 0x17700                     ; rewind source 3 planes; net +0x280 for next pass
        dec     ebx
        jne     plane_pass
        leave
        ret
