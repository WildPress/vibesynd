/* validate_records_or_abort @ 0x18338 - walk records (stride 0x2c) while *(int*)(rec+0x1c)!=0;
   classify each via realloc_block_descriptor, tally errors, then report+abort if any. */
extern int  realloc_block_descriptor(char *rec);
extern void printf(int code, int arg);
extern void exit(int arg);
#pragma aux exit aborts;

void validate_records_or_abort(char *p)
{
    short cnt = 0;

    while (*(int *)(p + 0x1c) != 0) {
        short r = (short)realloc_block_descriptor(p);
        if (r < 0) {
            if (r == -1) printf(0xb4, (int)p);
            if (r == -2) printf(0xd8, (int)p);
            if (r == -3) printf(0xfc, (int)p);
            cnt++;
        }
        if (r == 0) {
            cnt++;
            printf(0x11c, (int)p);
        }
        p += 0x2c;
    }

    if (cnt != 0) {
        printf(0x12c, cnt);
        exit(1);
    }
}
