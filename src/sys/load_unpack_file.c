/* 0x188e8 -- load+unpack a file. open(path,0x200) @0x18828; if fd != -1:
 * read(fd, buf, size_from_header(path) @0x18958 [RNC detector, banked]),
 * unpack @0x3a1ec(buf,buf), on negative result error(0x14c, path) @0x3ad66,
 * close(fd) @0x18878. Returns fd. Recipe: -4s -oneatx -zp8 -s -zq
 */
extern int cond_3call(char *path, int mode);
extern int open_detect_rnc_header(char *path);
extern void file_read_n(int fd, void *buf, int n);
extern int rnc_decompress(void *dst, void *src);
extern void printf(int code, char *path);
extern void FUN_00018878(int fd);

int load_unpack_file(char *path, void *buf)
{
    int fd = cond_3call(path, 0x200);

    if (fd != -1) {
        file_read_n(fd, buf, open_detect_rnc_header(path));
        if (rnc_decompress(buf, buf) < 0)
            printf(0x14c, path);
        FUN_00018878(fd);
    }
    return fd;
}
