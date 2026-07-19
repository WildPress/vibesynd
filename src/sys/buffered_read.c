/* 0x17998 -- buffered-read helper. If flags bit 0 (memory-resident): inline
 * memcpy(dst, base+off, len) via the memcpy intrinsic (rep movsd/movsb) and
 * return off+len. Else: seek(h,off,0) @0x3a93b, read(h,dst,len) @0x3a7c4,
 * return 0x3a97c(h) (tell). Recipe: -4s -oneatx -zp8 -s -zq (+intrinsic).
 */
extern int lseek(char *h, unsigned off, int whence);
extern int read(char *h, void *dst, unsigned len);
extern int tell(char *h);

void *memcpy(void *dst, const void *src, unsigned len);
#pragma intrinsic(memcpy)

int buffered_read(char *h, unsigned off, int flags, void *dst, unsigned len);
#pragma aux buffered_read modify [eax ecx edx ebx];

int buffered_read(char *h, unsigned off, int flags, void *dst, unsigned len)
{
    if (flags & 1) {
        memcpy(dst, h + off, len);
        return off + len;
    }
    lseek(h, off, 0);
    read(h, dst, len);
    return tell(h);
}
