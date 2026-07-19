/* 0x179f8 -- total-size scan of a container file. fd = name if flags&1
 * (memory-resident) else open(name,0x200) (fail -> 0). Read u32 table offset
 * at 0x3c, then 2 bytes at that offset into an int pre-seeded with g_a240;
 * strcmp(&val,0xb0) nonzero -> close+0. Read the 0xac header at the
 * table offset; from hdr+0x40 (offset) / hdr+0x44 (count) walk count 24-byte
 * records sequentially (buffered_read returns the next position), summing each
 * record's first dword. Close unless memory-resident; return sum+0xf.
 * Recipe: -4s -oneatx -zp8 -s -zq.
 */
extern int g_a240;

extern int open(char *name, int mode);   /* open */
extern int close(int fd);                 /* close */
extern int strcmp(void *p, int n);
extern int buffered_read(int h, unsigned off, int flags, void *dst, unsigned len);
#pragma aux buffered_read modify [eax ecx edx ebx];

int container_total_size(char *name, int flags);
#pragma aux container_total_size modify [eax ecx edx ebx];

int container_total_size(char *name, int flags)
{
    char buf[0xac];
    char rec[0x18];
    int val;
    unsigned int tbl;
    int fd;
    int total;
    unsigned int i;
    unsigned int pos;

    val = g_a240;
    total = 0;
    if (flags & 1) {
        fd = (int)name;
    } else {
        fd = open(name, 0x200);
        if (fd == -1)
            return 0;
    }
    buffered_read(fd, 0x3c, flags, &tbl, 4);
    buffered_read(fd, tbl, flags, &val, 2);
    if (strcmp(&val, 0xb0)) {
        close(fd);
        return 0;
    }
    buffered_read(fd, tbl, flags, buf, 0xac);
    pos = tbl + *(unsigned int *)(buf + 0x40);
    for (i = 0; i < *(unsigned int *)(buf + 0x44); ++i) {
        pos = buffered_read(fd, pos, flags, rec, 0x18);
        total += *(int *)rec;
    }
    if (!(flags & 1))
        close(fd);
    return total + 0xf;
}
