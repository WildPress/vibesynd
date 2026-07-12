/* 0x35c88 -- read a 0x14-byte name record. Builds a path with sprintf(buf,
 * fmt@0x3cf0, str@0xb970, n), opens it (0x18828, mode 0x202); if fd > 0 reads
 * 0x14 bytes into out and closes. If out is still empty, strcpy a default from
 * the pointer table g_4720[g_a50d]. Always terminates at out[0x13].
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern void FUN_0003a4fa(char *buf, char *fmt, char *s, int n);
extern int FUN_00018828(char *path, int mode);
extern void FUN_000188a8(int fd, void *buf, int n);
extern void FUN_00018878(int fd);
extern void FUN_0003a8d7(char *dst, char *src);
extern char g_3cf0[];
extern char g_b970[];
extern unsigned char g_a50d;
extern char *g_4720[];

void FUN_00035c88(char *out, int n)
{
    char buf[100];
    int fd;

    *out = 0;
    FUN_0003a4fa(buf, g_3cf0, g_b970, n);
    fd = FUN_00018828(buf, 0x202);
    if (fd > 0) {
        FUN_000188a8(fd, out, 0x14);
        FUN_00018878(fd);
    }
    if (*out == 0)
        FUN_0003a8d7(out, g_4720[g_a50d]);
    out[0x13] = 0;
}
