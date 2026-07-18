/* @ 0x00018958 (195B): open file, read 0x14-byte header, detect RNC magic.
 * Returns big-endian uncompressed size (bytes 4..7) if "RNC\1", else file size,
 * or -1 if open failed. */
extern int  cond_3call(void *, int);
extern void file_read_n(int, void *, int);
extern int  strncmp(void *, void *, int);
extern int  FUN_0003ab1a(int);
extern void FUN_00018878(int);
extern void printf(int, void *);

int open_detect_rnc_header(void *fn)
{
    unsigned char buf[40];
    char magic[5];
    int h;
    int r = -1;

    magic[0] = 'R';
    magic[1] = 'N';
    magic[2] = 'C';
    magic[3] = 1;
    magic[4] = 0;

    h = cond_3call(fn, 0x200);
    if (h > 0) {
        file_read_n(h, buf, 0x14);
        if (strncmp(buf, magic, 4) == 0) {
            r = buf[4];
            r = (r << 8) + buf[5];
            r = (r << 8) + buf[6];
            r = (r << 8) + buf[7];
        } else {
            r = FUN_0003ab1a(h);
        }
        FUN_00018878(h);
    } else {
        printf(0x164, fn);
    }
    return r;
}
