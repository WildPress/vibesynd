; capture_sprite @ 0x4a898  (57 bytes) -- hand-written assembly (fully commented).
;
; capture_sprite: descriptor blit between VGA memory and the descriptor's own buffer,
; through the width-dispatched worker span_op_dispatch. It sets ES = DS (the worker uses
; ES:EDI string stores) and unpacks a sprite descriptor, but note the pointer roles
; are swapped relative to draw_sprite_buf: here EDI holds the descriptor's pixel buffer
; and ESI holds the VGA base. If span_op_dispatch treats ESI as source and EDI as dest,
; this copies FROM VGA INTO the descriptor buffer -- i.e. it looks like a masked
; read-back / capture of a sprite-shaped screen region, rather than a draw. (Inferred
; from the reversed pointers; the operation selector dx is left as the caller set it,
; not reloaded here.)
;
; The descriptor (pointer in arg4) is: +0 = pixel buffer, +4 = width, +5 = height.
;
; Args (stack / cdecl):
;   [ebp+8]    x (word)          [ebp+0xc]  y (word)
;   [ebp+0x14] descriptor ptr
; Registers passed to span_op_dispatch:
;   ax=x  bx=y  cl=width  ch=height  edi=descriptor buffer  esi=VGA base
; Globals:  0x536c  VGA video base
; Calls:    span_op_dispatch @ 0x4b073
;
capture_sprite:
        push    ebp
        mov     ebp, esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        push    ebp
        push    es                               ; save ES (worker uses ES:EDI)
        mov     ax, ds
        mov     es, ax                           ; ES = DS (flat)
        mov     esi, dword ptr [ebp + 0x14]      ; esi = descriptor ptr
        mov     ax, word ptr [ebp + 8]           ; ax = x
        mov     bx, word ptr [ebp + 0xc]         ; bx = y
        mov     cl, byte ptr [esi + 4]           ; cl = width
        mov     ch, byte ptr [esi + 5]           ; ch = height
        mov     edi, dword ptr [esi]             ; edi = descriptor pixel buffer
        mov     esi, dword ptr [0x536c]          ; esi = VGA base
        call    0x4b073                          ; span_op_dispatch: width-dispatched blit
        pop     es
        pop     ebp
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        ret
