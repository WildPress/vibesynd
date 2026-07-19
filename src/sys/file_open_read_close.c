/* file_open_read_close @ 0x180f8 - file open+read+close wrapper; sets g_3eec error code. */
extern unsigned int g_3eec;
extern int open(char *name, int mode);
extern unsigned int filelength(int fd);
extern void close(int fd);

#pragma aux file_open_read_close modify [eax ecx edx ebx];

unsigned int file_open_read_close(char *name)
{
    int fd;
    unsigned int result;

    g_3eec = 0;
    fd = open(name, 0x200);
    if (fd == -1) {
        g_3eec = 3;
        return 0xffffffff;
    }
    result = filelength(fd);
    if (result == 0xffffffff)
        g_3eec = 5;
    close(fd);
    return result;
}
