/* @ 0x26998 -- subsystem init call sequence. */
extern void FUN_00026a18(void), FUN_00031858(void), FUN_000380b8(void),
            FUN_0001c2c8(void), FUN_00029088(void), FUN_00036fd8(void);

void init_subsystems(void)
{
    FUN_00026a18();
    FUN_00031858();
    FUN_000380b8();
    FUN_0001c2c8();
    FUN_00029088();
    FUN_00036fd8();
}
