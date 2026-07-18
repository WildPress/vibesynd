; stop_voice @ 0x39a82  (61 bytes) -- hand-written assembly (fully commented).
;
; stop_voice: stop playback on a voice (the "voice-stop trampoline"). It atomically
; clears the voice's active flag; if the voice really was active it releases the voice's
; handle and then dispatches driver command 0x68 (stop) to the voice.
;
; The routine takes no frame: it reads its single argument straight off the stack via
; esp, so it can be reached either by a normal call or by a tail-jump.
;
; Args:     [esp+4] = voice index (0..15).
; Globals:  0xbd7a  g_voice_active[16]  per-voice active flag (xchg'd to 0 here)
;           0xbd3a  g_voice_handle[16]  per-voice handle (-1 = none), released via free_seq_slot
; Tail-call: sound_dispatch_trampoline with command 0x68 (stop) if the voice was active.

stop_voice:
        mov     ebx, esp                     ; read arg without a frame
        mov     ebx, dword ptr [ebx + 4]     ; ebx = voice index
        cmp     ebx, 0x10                    ; index in range 0..15 ?
        jae     ret_out                      ;   no -> nothing to do
        shl     ebx, 1                        ; ebx *= 4  (dword table index)
        shl     ebx, 1
        mov     edx, 0
        xchg    dword ptr [ebx + 0xbd7a], edx ; edx = old g_voice_active[voice]; store 0 (atomic clear)
        cmp     edx, 0                        ; was the voice active?
        je      ret_out                      ;   no -> done
        mov     edx, dword ptr [ebx + 0xbd3a] ; edx = g_voice_handle[voice]
        cmp     edx, -1                      ; a live handle?
        je      stop_driver                  ;   no handle -> skip release
        push    edx
        call    0x396d5                      ; -> free_seq_slot: release/free the voice handle
        add     esp, 4
stop_driver:
        mov     eax, 0x68                    ; command 0x68 = stop
        jmp     0x392ac                      ; -> sound_dispatch_trampoline (tail-call: stop this voice)
ret_out:
        ret
