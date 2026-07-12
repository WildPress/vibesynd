/* C runtime: stricmp @ 0x0003aef9 (CLIB3S). Size 63B. NEAR-MISS (parked). Return block matches byte-exact; only the
   pointer-register assignment differs (target s1->eax/s2->edx, ours swapped) -- a param
   allocator choice no -o flag flips. Register-role wall (playbook S3).
   Built from Open Watcom bld/clib/string/c/stricmp.c form (9.5 increments at loop bottom). */
int FUN_0003aef9(const char *s1, const char *s2)
{
    unsigned char c1;
    unsigned char c2;

    for( ;; ) {
        c1 = *s1;
        if( c1 >= 'A' && c1 <= 'Z' )
            c1 += 'a' - 'A';
        c2 = *s2;
        if( c2 >= 'A' && c2 <= 'Z' )
            c2 += 'a' - 'A';
        if( c1 != c2 )
            break;
        if( c1 == 0 )
            break;
        ++s1;
        ++s2;
    }
    return( c1 - c2 );
}
