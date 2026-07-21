/* demo_asm_native.c -- proof that the game's OWN assembly runs natively.
 *
 * The hot-path routines are byte-transcriptions with offsets baked for the DOS 0x10000
 * layout; tools/asm_symbolize.py rewrites them into relocatable asm (calls + data refs made
 * symbolic). This links the symbolized RNC decompressor, gives it the DGROUP data region it
 * addresses (`__dgroup`), and calls it on a real compressed palette from the user's data.
 *
 * If the game's actual machine code decompresses correctly in a native 32-bit process, the
 * relocation pipeline works end to end. Build with port/build_asm_native.sh.
 *
 * rnc_decompress is cdecl: (src, dst) on the stack, returns the unpacked length in eax.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* the data segment the symbolized asm references as `__obj<N> + <offset>`. RNC touches
 * only obj2 (bit-buffer state + Huffman tables), so obj2's base is all this demo needs. */
unsigned char __obj2[0x20000];

extern long rnc_decompress(void *src, void *dst);

int main(int argc, char **argv) {
    const char *path = (argc > 1) ? argv[1] :
        "/mnt/c/Program Files (x86)/GOG Galaxy/Games/Syndicate Plus/SYNDICAT/DATA/HPALETTE.DAT";
    unsigned char packed[8192], out[4096];
    FILE *f = fopen(path, "rb");
    long n, r, i, mx = 0;
    if (!f) { fprintf(stderr, "cannot open %s\n", path); return 1; }
    n = (long)fread(packed, 1, sizeof packed, f);
    fclose(f);
    (void)n;
    memset(out, 0, sizeof out);

    r = rnc_decompress(packed, out);          /* <-- the game's real asm, running natively */

    for (i = 0; i < r && i < (long)sizeof out; i++) if (out[i] > mx) mx = out[i];
    printf("rnc_decompress returned %ld bytes; max value 0x%02lx\n", r, mx);
    if (r == 768 && mx <= 0x3f) {
        printf("NATIVE-ASM-OK: the game's assembly decompressed a 768-byte 6-bit palette\n");
        return 0;
    }
    printf("UNEXPECTED (want 768 bytes, max <= 0x3f)\n");
    return 1;
}
