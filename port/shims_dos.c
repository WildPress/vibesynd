/* shims_dos.c -- portable-C replacements for the DOS/CLIB helpers that still issued raw DOS
 * calls. Part of Phase 1 of the port: replace every DOS-touching routine with clean C so the
 * fault-handler emulator can be retired. The game is -4s (stack calling), so these are cdecl.
 */

/* outp(port, value) -- a raw `out dx,al`. Nothing native listens on hardware ports, so this is
 * a no-op. (The palette/PIT that mattered are handled by their own shims.) */
int shim_outp(int port, int value) { (void)port; return value; }

/* d_getvec(_, intnum) -- get an interrupt vector. There are no real vectors natively. */
int shim_d_getvec(int a, int intnum) { (void)a; (void)intnum; return 0; }

/* d_setvec(handler, ..., intnum) -- install an interrupt vector: no-op (ISRs are shimmed). */
void shim_d_setvec(int a, int b, int c) { (void)a; (void)b; (void)c; }

/* dos_exec(...) -- launch a DOS child program. Not supported / not needed natively; report ok
 * so the caller proceeds (matches the emulator's previous behaviour). */
int shim_dos_exec(int a, int b, int c, int d) { (void)a; (void)b; (void)c; (void)d; return 0; }

/* spawnve(buf, useSlash) -- CLIB switch-char helper: builds "<switch>c" (switch char is '/').
 * buf is the FIRST arg ([ebp+0xc] in the original after its two pushes). */
char *shim_spawnve(char *buf, int useSlash) {
    (void)useSlash;
    if (!buf) return buf;
    buf[0] = '/'; buf[1] = 'c'; buf[2] = 0;
    return buf;
}
