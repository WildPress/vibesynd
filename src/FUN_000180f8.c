/* FUN_000180f8 @ 0x180f8 - file open+read+close wrapper; sets g_3eec error code. */
extern unsigned int g_3eec;
extern int FUN_0003a579(char *name, int mode);
extern unsigned int FUN_0003ab1a(int fd);
extern void FUN_0003a89d(int fd);

#pragma aux FUN_000180f8 modify [eax ecx edx ebx];

unsigned int FUN_000180f8(char *name)
{
    int fd;
    unsigned int result;

    g_3eec = 0;
    fd = FUN_0003a579(name, 0x200);
    if (fd == -1) {
        g_3eec = 3;
        return 0xffffffff;
    }
    result = FUN_0003ab1a(fd);
    if (result == 0xffffffff)
        g_3eec = 5;
    FUN_0003a89d(fd);
    return result;
}
