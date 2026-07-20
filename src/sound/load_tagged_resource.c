/* frameless @ 0x38c28: find a tagged record in an open file and load its blob.
   Rewinds the file (0x3b407), then repeatedly reads a 6-byte header (byte tag0, byte
   tag1, dword offset) until tag1 == p2 and tag0 == p3 (or tag1 == -1 = end -> return 0).
   Seeks to the record's offset, reads a 2-byte length, mallocs that many bytes, stores
   the length at the head and reads the remaining length-2 bytes after it. Returns the
   blob, or 0 if the file is null, the record is absent, or p1[0xc]&0x20 is set.

   Matches CANONICAL -4s -oneatx -zp8 -s -zq (reloc-aware byte-identical).
   The fix is the struct. The header globals (tag0 0x11e34, tag1 0x11e35, offset 0x11e36)
   and the length (0x11e3a) are one contiguous object that fread(&g_hdr,6,..) then
   fread(&g_hdr.len,2,..) write. Spelt as four separate externs, canonical's relax-alias
   ('a') assumed the fread could not touch the distinctly-named tag reads, so it was free
   to re-order the two tag comparisons and picked the opposite callee-saved colouring --
   p2 landed in edi / p3 in esi, the reverse of the original's p2=esi / p3=edi. (That is
   why the function used to need the alias 'a' dropped: recipe -4s -oentx -zp8 -s -zq.)
   Declaring the header as one packed struct makes the tag reads provably alias the fread
   write, so relax-alias can no longer re-order them, and canonical seats p2=esi / p3=edi
   to match. pack(1) keeps offset at +2 and len at +6, faithful to the real addresses. */
extern int rewind(void *h);
extern int fread(void *dst, int size, int count, void *h);
extern int fseek(void *h, int off, int whence);
extern void *malloc(int size);

#pragma pack(1)
struct tagged_hdr {
    signed char tag0;      /* 0x11e34 */
    signed char tag1;      /* 0x11e35 */
    int off;               /* 0x11e36 */
    unsigned short len;    /* 0x11e3a */
};
extern struct tagged_hdr g_hdr;

void *load_tagged_resource(unsigned char *p1, unsigned short p2, unsigned short p3)
{
    unsigned char *blob;
    if (p1 == 0)
        return 0;
    rewind(p1);
    for (;;) {
        fread(&g_hdr, 6, 1, p1);
        if (g_hdr.tag1 == -1)
            return 0;
        if (g_hdr.tag1 != p2)
            continue;
        if (g_hdr.tag0 != p3)
            continue;
        break;
    }
    fseek(p1, g_hdr.off, 0);
    fread(&g_hdr.len, 2, 1, p1);
    blob = malloc(g_hdr.len);
    *(unsigned short *)blob = g_hdr.len;
    fread(blob + 2, g_hdr.len - 2, 1, p1);
    if (p1[0xc] & 0x20)
        return 0;
    return blob;
}
