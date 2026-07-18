; FUN_000399bd @ 0x399bd  (197 bytes) -- hand-written assembly (fully commented).
;
; FUN_000399bd: start playback on a voice. Given a voice index and four playback
; parameters, it prepares the voice (sets up its handle and timer/callback rate) and
; then dispatches driver command 0x66 -- the "play/trigger" command -- with the four
; parameters, finally marking the voice active. The whole body runs with interrupts
; disabled (pushfd/cli ... restore) because it touches the same per-voice tables the
; timer ISR walks.
;
; Args (cdecl):
;   [ebp+8]  = voice index (0..15)
;   [ebp+0xc], [ebp+0x10], [ebp+0x14], [ebp+0x18] = four playback params, passed
;              straight through to the driver's 0x66 handler.
; Globals:
;   0xbdc6  scratch cell holding the voice's freshly-built handle (-1 = none yet)
;   0xbd3a  g_voice_handle[16]   per-voice handle table (set here from FUN_00039625)
;   0xbd7a  g_voice_active[16]   per-voice active flag (set to 1 at the end)
; Calls: FUN_00039994 (cmd 0x64 -> driver object), FUN_00039280 (resolve cmd 0x67
;   handler), FUN_00039625 (build the voice handle), timer_rate_critsec (set callback
;   rate), sound_dispatch_trampoline (cmd 0x66 = play), FUN_00039747 (post-start hook).
;
; The (voice index >= 0x10) guard rejects out-of-range voices; the two "esi==-1" /
; "handler==0" early-outs skip the rate/handle setup and go straight to the 0x66
; dispatch, so a voice with no 0x67 handler still plays, just without the extra setup.

FUN_000399bd:
        push    ebp
        mov     ebp, esp
        push    esi
        push    edi
        pushfd                               ; save caller's flags (incl. IF)
        cli                                  ; enter critical section
        cmp     dword ptr [ebp + 8], 0x10    ; voice index in range?
        jae     done                         ;   no -> bail out
        mov     dword ptr [0xbdc6], 0xffffffff ; scratch handle = none
        push    dword ptr [ebp + 8]
        call    0x39994                      ; -> FUN_00039994: driver object for this voice (cmd 0x64)
        add     esp, 4
        mov     edi, eax                     ; edi = driver object
        mov     esi, dword ptr [edi + 0x20]  ; esi = object field +0x20 (rate/period seed)
        cmp     esi, -1                      ; field valid?
        je      dispatch_play                ;   no -> skip rate/handle setup
        mov     eax, 0x67                    ; command 0x67 (per-voice "prepare" handler)
        mov     ebx, dword ptr [ebp + 8]     ; ebx = voice index (selector)
        call    0x39280                      ; -> FUN_00039280: resolve the 0x67 handler
        mov     ebx, eax
        cmp     ebx, 0                        ; handler present?
        je      dispatch_play                ;   no -> skip setup
        mov     edi, eax
        push    edi
        call    0x39625                      ; -> FUN_00039625: build the voice handle (using that handler)
        add     esp, 4                        ;   eax = new handle
        mov     ebx, dword ptr [ebp + 8]
        shl     ebx, 1                        ; ebx *= 4
        shl     ebx, 1
        mov     dword ptr [ebx + 0xbd3a], eax ; g_voice_handle[voice] = handle
        mov     dword ptr [0xbdc6], eax       ; scratch handle = handle
        push    esi                          ; esi = rate seed from object[+0x20]
        push    dword ptr [0xbdc6]           ; the handle
        call    0x39846                      ; -> timer_rate_critsec: program this voice's callback rate
        add     esp, 8
dispatch_play:
        push    dword ptr [ebp + 0x18]       ; param 4
        push    dword ptr [ebp + 0x14]       ; param 3
        push    dword ptr [ebp + 0x10]       ; param 2
        push    dword ptr [ebp + 0xc]        ; param 1
        push    dword ptr [ebp + 8]          ; voice index (selector)
        mov     eax, 0x66                    ; command 0x66 = play/trigger
        call    0x392ac                      ; -> sound_dispatch_trampoline (run the play handler)
        add     esp, 0x14                     ; drop the five pushed args
        mov     ebx, dword ptr [ebp + 8]
        shl     ebx, 1                        ; ebx *= 4
        shl     ebx, 1
        mov     dword ptr [ebx + 0xbd7a], 1  ; mark g_voice_active[voice] = 1
        cmp     dword ptr [0xbdc6], -1        ; did we build a handle above?
        je      done                         ;   no -> skip post-start hook
        push    dword ptr [0xbdc6]
        call    0x39747                      ; -> FUN_00039747: post-start hook on the handle
        add     esp, 4
done:
        push    ebp                          ; --- restore caller's interrupt state, then flags ---
        mov     ebp, esp
        test    byte ptr [ebp + 5], 2        ; saved EFLAGS bit 9 (IF) set on entry?
        cli
        je      flags_restored               ;   IF was clear -> leave interrupts off
        sti                                  ;   IF was set   -> re-enable
flags_restored:
        pop     ebp
        popfd                                ; restore the rest of the saved flags
        pop     edi
        pop     esi
        leave
        ret
