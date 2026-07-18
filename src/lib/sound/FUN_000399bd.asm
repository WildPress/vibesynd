; FUN_000399bd @ 000399bd  (197 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_000399bd.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_000399bd:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        cmp     dword ptr [ebp + 8], 0x10        ; 837d0810
        jae     0x39a71                          ; 0f83a3000000
        mov     dword ptr [0xbdc6], 0xffffffff   ; c705c6bd0000ffffffff
        push    dword ptr [ebp + 8]              ; ff7508
        call    0x39994                          ; e8b4ffffff     -> FUN_00039994
        add     esp, 4                           ; 83c404
        mov     edi, eax                         ; 8bf8
        mov     esi, dword ptr [edi + 0x20]      ; 8b7720
        cmp     esi, -1                          ; 83feff
        je      0x39a2d                          ; 7440
        mov     eax, 0x67                        ; b867000000
        mov     ebx, dword ptr [ebp + 8]         ; 8b5d08
        call    0x39280                          ; e886f8ffff     -> FUN_00039280
        mov     ebx, eax                         ; 8bd8
        cmp     ebx, 0                           ; 83fb00
        je      0x39a2d                          ; 742c
        mov     edi, eax                         ; 8bf8
        push    edi                              ; 57
        call    0x39625                          ; e81cfcffff     -> FUN_00039625
        add     esp, 4                           ; 83c404
        mov     ebx, dword ptr [ebp + 8]         ; 8b5d08
        shl     ebx, 1                           ; d1e3
        shl     ebx, 1                           ; d1e3
        mov     dword ptr [ebx + 0xbd3a], eax    ; 89833abd0000
        mov     dword ptr [0xbdc6], eax          ; a3c6bd0000
        push    esi                              ; 56
        push    dword ptr [0xbdc6]               ; ff35c6bd0000
        call    0x39846                          ; e81cfeffff     -> timer_rate_critsec
        add     esp, 8                           ; 83c408
        push    dword ptr [ebp + 0x18]           ; ff7518
        push    dword ptr [ebp + 0x14]           ; ff7514
        push    dword ptr [ebp + 0x10]           ; ff7510
        push    dword ptr [ebp + 0xc]            ; ff750c
        push    dword ptr [ebp + 8]              ; ff7508
        mov     eax, 0x66                        ; b866000000
        call    0x392ac                          ; e866f8ffff     -> sound_dispatch_trampoline
        add     esp, 0x14                        ; 83c414
        mov     ebx, dword ptr [ebp + 8]         ; 8b5d08
        shl     ebx, 1                           ; d1e3
        shl     ebx, 1                           ; d1e3
        mov     dword ptr [ebx + 0xbd7a], 1      ; c7837abd000001000000
        cmp     dword ptr [0xbdc6], -1           ; 833dc6bd0000ff
        je      0x39a71                          ; 740e
        push    dword ptr [0xbdc6]               ; ff35c6bd0000
        call    0x39747                          ; e8d9fcffff     -> FUN_00039747
        add     esp, 4                           ; 83c404
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x39a7c                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3
