/* frameless @ 0x38c28: find a tagged record in an open file and load its blob.
   Rewinds the file (0x3b407), then repeatedly reads a 6-byte header (byte tag0, byte
   tag1, dword offset) until tag1 == p2 and tag0 == p3 (or tag1 == -1 = end -> return 0).
   Seeks to the record's offset, reads a 2-byte length, mallocs that many bytes, stores
   the length at the head and reads the remaining length-2 bytes after it. Returns the
   blob, or 0 if the file is null, the record is absent, or p1[0xc]&0x20 is set.

   PARKED near-miss (202/206, a single 4-byte transpose). Logic + all 14 relocations
   match exactly; the sole diff is that the original holds p2 in esi and p3 in edi, while
   9.5b assigns them the opposite callee-saved registers in the match loop. A clean
   register tie-break: not moved by cpermute (2500), by &&-vs-continue restructuring, or
   by five flag variants. Register-role family. */
extern int rewind(void *h);
extern int fread(void *dst, int size, int count, void *h);
extern int fseek(void *h, int off, int whence);
extern void *malloc(int size);
extern signed char g_11e34;
extern signed char g_11e35;
extern int g_11e36;
extern unsigned short g_11e3a;

void *load_tagged_resource(unsigned char *p1, unsigned short p2, unsigned short p3)
{
    unsigned char *blob;
    if (p1 == 0)
        return 0;
    rewind(p1);
    for (;;) {
        fread(&g_11e34, 6, 1, p1);
        if (g_11e35 == -1)
            return 0;
        if (g_11e35 != p2)
            continue;
        if (g_11e34 != p3)
            continue;
        break;
    }
    fseek(p1, g_11e36, 0);
    fread(&g_11e3a, 2, 1, p1);
    blob = malloc(g_11e3a);
    *(unsigned short *)blob = g_11e3a;
    fread(blob + 2, g_11e3a - 2, 1, p1);
    if (p1[0xc] & 0x20)
        return 0;
    return blob;
}
