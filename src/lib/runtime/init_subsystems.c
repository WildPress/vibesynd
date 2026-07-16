/* @ 0x26998 -- subsystem init call sequence. */
extern void FUN_00026a18(void), entity_pool_tick(void), FUN_000380b8(void),
            FUN_0001c2c8(void), FUN_00029088(void), vehicle_pool_tick(void);

void init_subsystems(void)
{
    FUN_00026a18();
    entity_pool_tick();
    FUN_000380b8();
    FUN_0001c2c8();
    FUN_00029088();
    vehicle_pool_tick();
}
