/* 0x17998 -- buffered-read helper. If flags bit 0 (memory-resident): inline
 * memcpy(dst, base+off, len) via the memcpy intrinsic (rep movsd/movsb) and
 * return off+len. Else: seek(h,off,0) @0x3a93b, read(h,dst,len) @0x3a7c4,
 * return 0x3a97c(h) (tell). Recipe: -4s -oneatx -zp8 -s -zq (+intrinsic).
 */
extern int FUN_0003a93b(char *h, unsigned off, int whence);
extern int FUN_0003a7c4(char *h, void *dst, unsigned len);
extern int FUN_0003a97c(char *h);

void *memcpy(void *dst, const void *src, unsigned len);
#pragma intrinsic(memcpy)

int FUN_00017998(char *h, unsigned off, int flags, void *dst, unsigned len);
#pragma aux FUN_00017998 modify [eax ecx edx ebx];

int FUN_00017998(char *h, unsigned off, int flags, void *dst, unsigned len)
{
    if (flags & 1) {
        memcpy(dst, h + off, len);
        return off + len;
    }
    FUN_0003a93b(h, off, 0);
    FUN_0003a7c4(h, dst, len);
    return FUN_0003a97c(h);
}
