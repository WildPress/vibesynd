/* 0x188e8 -- load+unpack a file. open(path,0x200) @0x18828; if fd != -1:
 * read(fd, buf, size_from_header(path) @0x18958 [RNC detector, banked]),
 * unpack @0x3a1ec(buf,buf), on negative result error(0x14c, path) @0x3ad66,
 * close(fd) @0x18878. Returns fd. Recipe: -4s -oneatx -zp8 -s -zq
 */
extern int FUN_00018828(char *path, int mode);
extern int FUN_00018958(char *path);
extern void FUN_000188a8(int fd, void *buf, int n);
extern int FUN_0003a1ec(void *dst, void *src);
extern void FUN_0003ad66(int code, char *path);
extern void FUN_00018878(int fd);

int FUN_000188e8(char *path, void *buf)
{
    int fd = FUN_00018828(path, 0x200);

    if (fd != -1) {
        FUN_000188a8(fd, buf, FUN_00018958(path));
        if (FUN_0003a1ec(buf, buf) < 0)
            FUN_0003ad66(0x14c, path);
        FUN_00018878(fd);
    }
    return fd;
}
