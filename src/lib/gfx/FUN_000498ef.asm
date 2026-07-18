; FUN_000498ef @ 0x498ef  (51 bytes) -- hand-written assembly (fully commented).
;
; FUN_000498ef: set the BIOS video mode and configure the mouse driver's range.
; A small startup/init helper. It sets the video mode passed in the low byte of
; arg0 (INT 0x10 with AH=0), then tells the INT 0x33 mouse driver the coordinate
; limits it should clamp the pointer to:
;   AX=7  set horizontal min/max, CX=0 .. DX=0x27e (0..638)
;   AX=8  set vertical   min/max, CX=0 .. DX=0x18e (0..398)
;
; Args (stack / cdecl):
;   [ebp+8]   video mode number in AL (AH is forced to 0 = "set mode")
; Ports/interrupts:
;   INT 0x10  BIOS video services (set mode)
;   INT 0x33  mouse driver (functions 7 and 8, set X/Y range)
;
FUN_000498ef:
        push    ebp
        mov     ebp, esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        mov     ax, word ptr [ebp + 8]           ; al = requested video mode
        mov     ah, 0                            ; AH=0 -> BIOS "set video mode"
        int     0x10
        mov     ax, 7                            ; mouse fn 7: set horizontal range
        xor     cx, cx                           ;   min x = 0
        mov     dx, 0x27e                        ;   max x = 638
        int     0x33
        mov     ax, 8                            ; mouse fn 8: set vertical range
        xor     cx, cx                           ;   min y = 0
        mov     dx, 0x18e                        ;   max y = 398
        int     0x33
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        ret
