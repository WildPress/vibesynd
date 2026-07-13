/* C runtime: sprintf @ 0x3a4fa (CLIB3S, Watcom 9.5). PARKED (near-miss, structure exact).
   Variadic formatter front-end: set up a va_list pointing at the first vararg, call the
   shared _doprnt core FUN_0003bb1e(buf, fmt, &args, out_callback) which returns the produced
   length, then NUL-terminate buf[len] and return len. The 4th arg 0x2cd9c is the buffer-put
   callback (a resolved code address, pushed as a literal -> exact bytes).

   WALL (scheduler + register-role, both allocator-internal, not source-reachable):
   every instruction is present and correct; the only residues are two tie-breaks in the
   -oneatx schedule, and they are MUTUALLY EXCLUSIVE with -oneat:
     * -oneatx: buf-reload-before-cleanup is correct, but (a) the va_list store `mov [ebp-4],eax`
       is SUNK past the `push 0x2cd9c` (target stores first), and (b) the final `buf[len]=0`
       SIB picks base=buf/index=len where target picks base=len/index=buf (register-role tie;
       swapping the C index expr `len[buf]` does not move it -- Watcom normalizes).
     * -oneat: the va store is NOT sunk (correct), but the buf-reload is scheduled AFTER the
       esp cleanup (`add esp,0x10; mov edx,[ebp+8]`) where target does mov-then-add.
   Target needs -oneat's store behaviour + -oneatx's reload behaviour simultaneously; the
   'x' (alias-relax) flag toggles both together. Same CLIB scheduler wall class as the region.
   Best leading match: -oneat -> first diff at 0x20 (32/44). Structurally cleanest: -oneatx. */
extern int FUN_0003bb1e(char *buf, const char *fmt, char **argp, void *outfn);

int FUN_0003a4fa(char *buf, const char *fmt, ...)
{
    char *args = (char *)(&fmt + 1);
    int   len;

    len = FUN_0003bb1e(buf, fmt, &args, (void *)0x2cd9c);
    buf[len] = '\0';
    return len;
}
