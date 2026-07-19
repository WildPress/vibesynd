/* @ 0x26998 -- subsystem init call sequence. */
extern void update_drift_vector(void), entity_pool_tick(void), update_pickup_states(void),
            update_bullet_sfx_states(void), update_static_object_states(void), vehicle_pool_tick(void);

void world_tick(void)
{
    update_drift_vector();
    entity_pool_tick();
    update_pickup_states();
    update_bullet_sfx_states();
    update_static_object_states();
    vehicle_pool_tick();
}
