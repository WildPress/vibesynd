/* frameless @ 0x38c28: find a tagged record in an open file and load its blob.
   Rewinds the file (0x3b407), then repeatedly reads a 6-byte header (byte tag0, byte
   tag1, dword offset) until tag1 == p2 and tag0 == p3 (or tag1 == -1 = end -> return 0).
   Seeks to the record's offset, reads a 2-byte length, mallocs that many bytes, stores
   the length at the head and reads the remaining length-2 bytes after it. Returns the
   blob, or 0 if the file is null, the record is absent, or p1[0xc]&0x20 is set.

   CLOSED by a per-function FLAG lever (not a source change). Under the bulk recipe
   -4s -oneatx the original held p2 in esi / p3 in edi and 9.5b assigned the opposite
   callee-saved pair -- a register tie-break that no source spelling moved. Dropping the
   single optimisation letter 'a' (relax-alias) from the -o bundle flips that colouring:
   compiled with -oneatx MINUS 'a' the whole function is reloc-aware byte-identical.
   Recipe: -4s -oentx -zp8 -s -zq
   (verified: RELOC-AWARE match YES; the 14 relocations mask as before. 'a' relaxes the
   aliasing model, so removing it makes 9.5b treat the fread-written globals g_11e34/
   g_11e35 more conservatively, which is what seats p2=esi / p3=edi to match the original.
   The full extended flag grid finds NO other combo that matches, and packing is inert.) */
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
