/* C runtime: _exit @ 0x3b1f6 (CLIB3S, Watcom 9.5). Runs the atexit/cleanup chain
   (run_fini_routines), then terminates via DOS int 21h AH=0x4C with the exit code in AL.
   The tail `mov ds,cs:[0x2d8d0]` (restore the program DS) is dead after the int but
   present on disk. Whole-function db-transcription into a frameless wrapper; the one
   call is a real masked reloc and the trailing RET is supplied by the wrapper. */
extern void run_fini_routines(void);
extern void __exit_(void);
#pragma aux __exit_ = "db 80" "call run_fini_routines" "db 88" "db 180" "db 76" "db 205" "db 33" "db 144" "db 144" "db 144" "db 46" "db 142" "db 29" "db 208" "db 216" "db 2" "db 0" parm [] modify exact [eax ecx edx];
void _exit(int code)
{
    __exit_();
}
