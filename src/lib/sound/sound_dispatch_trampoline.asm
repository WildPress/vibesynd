; sound_dispatch_trampoline @ 0x392ac  (18 bytes) -- hand-written assembly (fully commented).
;
; sound_dispatch_trampoline: the central indirect-dispatch stub of the sound driver.
; It looks up a per-voice command handler and tail-jumps into it, so the handler runs
; on this same stack with the same arguments the caller pushed. This is the classic
; AIL/Miles "driver command" pattern: every voice (0..15) owns a small table of
; {command-code, handler} pairs, and a call site selects one by putting a command code
; in eax and the voice index as its first stack argument.
;
; Args:
;   eax          = command code / driver ordinal (e.g. 0x64, 0x66, 0x68)
;   [esp+4]      = first stack argument = voice index (0..15)
;   remaining stack args belong to the resolved handler and are passed through untouched.
; Returns:
;   whatever the handler returns, or eax = 0 if the voice has no handler for this command.
;
; Globals: none directly; the lookup walks g_voice_driver[] via driver_msg_lookup.

sound_dispatch_trampoline:
        mov     ebx, esp                     ; point at the incoming stack
        mov     ebx, dword ptr [ebx + 4]     ; ebx = first arg = voice index
        call    0x39280                      ; -> driver_msg_lookup: resolve handler for (voice ebx, command eax)
        cmp     eax, 0                        ; handler found?
        je      not_found                    ;   no -> return 0
        jmp     eax                          ; tail-jump into handler (keeps caller's args/stack)
not_found:
        ret                                  ; eax already 0
