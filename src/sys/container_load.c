/* 0x17b48 -- resource/container LOADER (sibling of the size-scanner 0x179f8).
 * NEAR-MISS (NOT matched; large fn, pervasive register-role divergence across the
 * whole body -- allocator assigns different regs throughout, not a single fixable
 * point). Structure/logic correct; parked as a register-allocation wall.
 * open(name,0x200) unless flags&1 (memory-resident, fd=name). Compute total
 * size via container_total_size; if flags&4, malloc that size into 'out' (fail->close+0)
 * then memset it. Read u32 table offset at 0x3c, 2-byte version-ish word at tbl
 * seeded with g_a244; FUN_0003a9c8(&val,0xb0) nonzero -> close+0. Read the 0xac
 * header at tbl. Phase 1: walk hdr[0x44] segment records (24B each from
 * tbl+hdr[0x40]); for each, walk rec[0x10] chunk entries (8B each from
 * tbl+hdr[0x48]), 16-aligning the first chunk when rec[8]&3, copying
 * min(chunk-len, rec[0]-consumed) bytes from (chunkidx<<hdr[0x2c])+hdr[0x80]
 * into the running output pointer; record per-segment/per-chunk base pointers.
 * Phase 3: relocation pass over hdr[0x14] tables (4B ptrs from tbl+hdr[0x68]);
 * for each non-empty range walk reloc entries validating type bytes and patching
 * seg1[table][off] += resolved addr. Close unless memory-resident; return out.
 * Recipe: -4s -oneatx -zp8 -s -zq.
 */
extern int g_a244;

extern int FUN_0003a579(char *name, int mode);            /* open */
extern void FUN_0003a89d(int fd);                         /* close */
extern int FUN_0003a9c8(void *p, int n);
extern void *FUN_0003aa74(unsigned n);                    /* malloc */
extern void FUN_0003aaf8(void *dst, int val, int len);    /* memset */
extern int buffered_read(int h, unsigned off, int flags, void *dst, unsigned len);
extern int container_total_size(char *name, int flags);
#pragma aux buffered_read modify [eax ecx edx ebx];
#pragma aux container_total_size modify [eax ecx edx ebx];

void *container_load(char *name, int flags, void *out);
#pragma aux container_load modify [eax ecx edx ebx];

void *container_load(char *name, int flags, void *out)
{
    unsigned char *seg1[100];
    char hdr[0xac];
    unsigned char *a4[10];
    char rec[0x18];
    unsigned char in8[8];
    unsigned char b10[4];
    unsigned char b14[4];
    unsigned char b18[4];
    unsigned char b1c[2];
    unsigned char b20[2];
    int r28;
    int pos2c;
    int pos30;
    unsigned int r34;
    unsigned int clen;
    int val;
    int gidx;
    int posInner;
    int savedIdx;
    unsigned int k1;
    unsigned int tbl;
    int prev;
    int cur;
    int fd;
    unsigned char *pp;
    unsigned int uv;
    unsigned int uv3;
    int pos;

    val = g_a244;
    gidx = 0;
    if (flags & 1) {
        fd = (int)name;
    } else {
        fd = FUN_0003a579(name, 0x200);
        if (fd == -1)
            return (void *)0;
    }
    uv = container_total_size(name, flags);
    if (flags & 4) {
        out = FUN_0003aa74(uv);
        if (out == (void *)0) {
            if ((flags & 1) == 0)
                FUN_0003a89d(fd);
            return (void *)0;
        }
    }
    FUN_0003aaf8(out, 0, uv);
    buffered_read(fd, 0x3c, flags, &tbl, 4);
    buffered_read(fd, tbl, flags, &val, 2);
    if (FUN_0003a9c8(&val, 0xb0)) {
        if ((flags & 1) == 0)
            FUN_0003a89d(fd);
        return (void *)0;
    }
    buffered_read(fd, tbl, flags, hdr, 0xac);

    pos = tbl + *(unsigned int *)(hdr + 0x40);
    pp = (unsigned char *)out;
    k1 = 0;
    if (k1 < *(unsigned int *)(hdr + 0x44)) {
        do {
            pos2c = buffered_read(fd, pos, flags, rec, 0x18);
            uv = 0;
            if (uv < *(unsigned int *)(rec + 0x10)) {
                pos = tbl + *(unsigned int *)(hdr + 0x48);
                do {
                    posInner = buffered_read(fd, pos, flags, in8, 8);
                    if (uv == 0) {
                        if ((rec[8] & 2) && (rec[8] & 1) && ((int)pp & 0xf))
                            pp = pp + (0x10 - ((unsigned int)pp & 0xf));
                        a4[k1 + 1] = pp;
                    }
                    seg1[gidx] = pp;
                    gidx = gidx + 1;
                    uv3 = *(unsigned int *)rec - (unsigned int)(pp - a4[k1 + 1]);
                    clen = *(unsigned short *)(in8 + 4);
                    if (uv3 < clen)
                        clen = uv3;
                    buffered_read(fd,
                        (*(unsigned int *)in8 << *(unsigned char *)(hdr + 0x2c)) + *(unsigned int *)(hdr + 0x80),
                        flags, pp, clen);
                    uv = uv + 1;
                    pp = pp + clen;
                    pos = posInner;
                } while (uv < *(unsigned int *)(rec + 0x10));
            }
            k1 = k1 + 1;
            pos = pos2c;
        } while (k1 < *(unsigned int *)(hdr + 0x44));
    }

    pos = buffered_read(fd, tbl + *(unsigned int *)(hdr + 0x68), flags, &prev, 4);
    r34 = 0;
    if (r34 < *(unsigned int *)(hdr + 0x14)) {
        r28 = 0;
        do {
            pos = buffered_read(fd, pos, flags, &cur, 4);
            if (prev != cur) {
                savedIdx = r28;
                uv = prev + tbl + *(unsigned int *)(hdr + 0x6c);
                pos30 = pos;
                do {
                    pos = buffered_read(fd, uv, flags, b18, 1);
                    pos = buffered_read(fd, pos, flags, b10, 1);
                    if ((b18[0] & 7) == 0) {
                        if ((flags & 1) == 0)
                            FUN_0003a89d(fd);
                        return (void *)0;
                    }
                    if ((b10[0] & 4) && (b10[0] & 0x20) == 0) {
                        if ((flags & 1) == 0)
                            FUN_0003a89d(fd);
                        return (void *)0;
                    }
                    if (b18[0] & 0x20) {
                        if ((flags & 1) == 0)
                            FUN_0003a89d(fd);
                        return (void *)0;
                    }
                    pos = buffered_read(fd, pos, flags, b1c, 2);
                    pos = buffered_read(fd, pos, flags, b14, 1);
                    uv = buffered_read(fd, pos, flags, b20, 2);
                    if (*(unsigned short *)b1c <= *(unsigned int *)(hdr + 0x28)) {
                        *(unsigned int *)(*(unsigned char **)((char *)seg1 + savedIdx) + *(unsigned short *)b1c) =
                            *(unsigned short *)b20 + (unsigned int)a4[b14[0]];
                    }
                    pos = pos30;
                } while (uv < (unsigned int)(cur + tbl + *(unsigned int *)(hdr + 0x6c)));
            }
            prev = cur;
            r28 = r28 + 4;
            r34 = r34 + 1;
        } while (r34 < *(unsigned int *)(hdr + 0x14));
    }
    if ((flags & 1) == 0)
        FUN_0003a89d(fd);
    return out;
}
