; FUN_0004a8d1 @ 0x4a8d1  (56 bytes) -- hand-written assembly (fully commented).
;
; FUN_0004a8d1: draw a sprite from a descriptor through the width-dispatched worker
; blit_width_dispatch. Sets ES = DS (string stores use ES:EDI) and unpacks the descriptor,
; passing the sprite data in ESI and a fixed value 0x800 (2048) in EDI. In the
; blit_width_dispatch family EDI looks like a destination row pitch / stride rather than a
; pointer, so this appears to blit the sprite into a 2048-byte-pitch surface. (The
; exact destination is chosen inside blit_width_dispatch's per-width jump table, which is not
; resolved here -- treat the destination as that worker's business.)
;
; The descriptor (pointer in arg4) is: +0 = pixel data, +4 = width, +5 = height.
;
; Args (stack / cdecl):
;   [ebp+8]    x (word)          [ebp+0xc]  y (word)
;   [ebp+0x14] descriptor ptr
; Registers passed to blit_width_dispatch:
;   ax=x  bx=y  cl=width  ch=height  esi=sprite data  edi=0x800 (stride/param)
; Calls:    blit_width_dispatch @ 0x4a909
;
FUN_0004a8d1:
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
        mov     esi, dword ptr [esi]             ; esi = sprite pixel data
        mov     edi, 0x800                       ; edi = 0x800 (stride / param)
        call    0x4a909                          ; blit_width_dispatch: width-dispatched blit
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
