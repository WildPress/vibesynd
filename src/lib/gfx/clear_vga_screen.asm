; clear_vga_screen @ 0x4997e  (179 bytes) -- hand-written assembly (fully commented).
;
; clear_vga_screen: clear the visible VGA screen to a solid colour, all four planes.
; Only runs in planar (mode-X) render mode. It first resets the Graphics Controller
; to plain write mode (Bit Mask 0xff, Mode 0 via port 0x3ce), then for each of the
; four planes it enables that plane through the Sequencer Map Mask (port 0x3c4,
; index 2 -> 0x0102/0x0202/0x0402/0x0802) and fills 0x9600 (38400) bytes of VGA
; memory at 0xa0000 with the colour byte via fill_bytes.
;
; Writing straight to physical VGA base 0xa0000 (rather than the offscreen buffer)
; means this paints the live display directly.
;
; Args (stack / cdecl):
;   [ebp+8]    colour byte (passed as a dword to fill_bytes)
; Globals:  0x105  render-mode flags
; Ports:    0x3ce/0x3cf  GC index/data      0x3c4/0x3c5  Sequencer (Map Mask = idx 2)
; Calls:    fill_bytes @ 0x4d199
;
clear_vga_screen:
        push    ebp
        mov     ebp, esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        cld
        test    byte ptr [0x105], 2              ; planar mode?
        je      try_bit2                         ;   no -> other modes
        mov     dx, 0x3ce                        ; GC index port
        mov     ax, 0xff08                       ; Bit Mask (idx 8) <- 0xff
        out     dx, ax
        mov     ax, 5                            ; Mode (idx 5) <- 0 : plain writes
        out     dx, ax
        mov     dl, 0xc4                         ; dx = 0x3c4 (Sequencer index)

        mov     ax, 0x102                        ; Map Mask <- plane 0
        out     dx, ax
        push    0x9600                           ; count = 38400 bytes
        push    dword ptr [ebp + 8]              ; value = colour
        push    0xa0000                          ; dst = VGA base
        call    0x4d199                          ; fill_bytes -> clear plane 0
        add     esp, 0xc

        mov     ax, 0x202                        ; Map Mask <- plane 1
        out     dx, ax
        push    0x9600
        push    dword ptr [ebp + 8]
        push    0xa0000
        call    0x4d199                          ; fill_bytes -> clear plane 1
        add     esp, 0xc

        mov     ax, 0x402                        ; Map Mask <- plane 2
        out     dx, ax
        push    0x9600
        push    dword ptr [ebp + 8]
        push    0xa0000
        call    0x4d199                          ; fill_bytes -> clear plane 2
        add     esp, 0xc

        mov     ax, 0x802                        ; Map Mask <- plane 3
        out     dx, ax
        push    0x9600
        push    dword ptr [ebp + 8]
        push    0xa0000
        call    0x4d199                          ; fill_bytes -> clear plane 3
        add     esp, 0xc
        jmp     done
try_bit2:
        test    byte ptr [0x105], 4              ; discard-layer mode?
        je      try_bit0
        jmp     done                             ;   yes -> nothing to do
try_bit0:
        test    byte ptr [0x105], 1              ; 8bpp mode?
        je      done                             ;   (this routine only clears the planar screen)
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        ret
