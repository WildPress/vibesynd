; draw_sprite_buf @ 0x4a63a  (49 bytes) -- hand-written assembly (fully commented).
;
; draw_sprite_buf: draw a sprite from a descriptor into the screen buffer. A thin wrapper
; that unpacks a sprite descriptor and the call arguments into the register convention
; of the width-dispatched planar blit worker span_op_dispatch, then calls it.
;
; The descriptor (pointer in arg2) is: +0 = sprite pixel data, +4 = width byte,
; +5 = height byte. Its known caller is the draw-list walker at 0x1b908, which
; schedules draw_sprite_buf(x, y, record, mode) for each object in a map cell.
;
; Args (stack / cdecl):
;   [ebp+8]    x (word)          [ebp+0xc]  y (word)
;   [ebp+0x10] descriptor ptr    [ebp+0x14] mode/operation selector (-> dx)
; Registers passed to span_op_dispatch:
;   ax=x  bx=y  cl=width  ch=height  esi=sprite data  edi=dest buffer  dx=mode
; Globals:  0x5368  g_screen_buf (destination)
; Calls:    span_op_dispatch @ 0x4b073
;
draw_sprite_buf:
        push    ebp
        mov     ebp, esp
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        mov     edi, dword ptr [ebp + 0x10]      ; edi = descriptor ptr
        mov     ax, word ptr [ebp + 8]           ; ax = x
        mov     bx, word ptr [ebp + 0xc]         ; bx = y
        mov     cl, byte ptr [edi + 4]           ; cl = width
        mov     ch, byte ptr [edi + 5]           ; ch = height
        mov     esi, dword ptr [edi]             ; esi = sprite pixel data
        mov     dx, word ptr [ebp + 0x14]        ; dx = mode/operation selector
        mov     edi, dword ptr [0x5368]          ; edi = g_screen_buf (dest)
        call    0x4b073                          ; span_op_dispatch: planar masked blit
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        leave
        ret
