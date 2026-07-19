#!/usr/bin/env python3
"""names.py -- build a SEMANTIC-NAME layer over the FUN_<addr> anchors. The matching-decomp pipeline
keys on FUN_<addr> (source filename, externs, differ symbol->addr), so we do NOT rename those; instead
we record a readable label + description per function in manifest/names.json, and emit a human map at
docs/function-map.md. Descriptions are harvested from each source file's header comment; labels are the
curated set below (extend as functions are understood). Optionally applied into Ghidra for interactive RE.

  python3 tools/names.py            # write manifest/names.json + docs/function-map.md
"""
import json, glob, os, re, struct, bisect

# Curated semantic names (addr hex -> name), high-confidence from the reverse-engineering so far.
# These are the functions that execute on the boot->logo->menu intro path.
LABELS = {
    # --- Watcom CLIB runtime + prefix game fns (naming fleet, this session) ---
    "0003a033": "rle_blit_word",
    "0003a10c": "rle_blit_byte",
    "0003a19a": "frame_throttle",
    "0003a598": "sopen",
    "0003a7c4": "read",
    "0003a89d": "close",
    "0003a900": "strcat_word",
    "0003a9c8": "strcmp",
    "0003aa74": "malloc",
    "0003aa84": "nmalloc",
    "0003aaf8": "memset",
    "0003ab1a": "filelength",
    "0003ab59": "free",
    "0003ab69": "nfree",
    "0003ab8e": "write",
    "0003ad89": "exit",
    "0003addb": "open_rw_creat_trunc",
    "0003adf0": "setmode",
    "0003afa4": "watcom_copyright_banner",
    "0003b01e": "cstart",
    "0003b1d3": "write_msg_and_exit",
    "0003b1f6": "_exit",
    "0003b2aa": "inp",
    "0003b2b5": "fputc_nl",
    "0003b326": "fgets_console",
    "0003b594": "fseek",
    "0003b739": "parse_open_mode",
    "0003b7e8": "fopen_core",
    "0003b8cd": "fsopen",
    "0003b9ca": "fclose_free",
    "0003ba01": "build_temp_filename",
    "0003ba80": "close_stream",
    "0003bb1e": "doprnt",
    "0003bd37": "parse_conv_spec",
    "0003be93": "far_strnlen",
    "0003bebc": "far_wcsnlen",
    "0003bee3": "format_hex_field",
    "0003bf36": "format_fixed_frac",
    "0003c002": "prtf_out_call",
    "0003c019": "prtf_conversion",
    "0003c46d": "errno_ptr",
    "0003c473": "doserrno_ptr",
    "0003c491": "dosret_map_err",
    "0003c4b9": "set_doserr",
    "0003c529": "fd_to_stream",
    "0003c57b": "fd_handle_store",
    "0003c772": "dpmi_dos_alloc_paras",
    "0003c816": "heap_grow",
    "0003ca18": "memset_align_head",
    "0003ca4f": "memset_dwords",
    "0003cabb": "stack_avail",
    "0003cacb": "stk_check_thunk",
    "0003cade": "stack_limit_check",
    "0003cafb": "stack_overflow",
    "0003cb24": "stream_reopen",
    "0003cba4": "getenv",
    "0003cc26": "spawn_load_check",
    "0003cc45": "spawn_exec",
    "0003cfe6": "cstart_call_main",
    "0003d036": "run_init_routines",
    "0003d079": "run_fini_routines",
    "0003d0bc": "build_argv",
    "0003d16f": "parse_cmdline",
    "0003d24a": "fputs",
    "0003d2c3": "fputc",
    "0003d36b": "getc",
    "0003d40f": "fill_buffer",
    "0003d4bc": "int386x",
    "0003d4f3": "intr_call_core",
    "0003d90b": "memcpy",
    "0003d966": "flush_stream",
    "0003da4c": "alloc_stream",
    "0003dae1": "free_stream",
    "0003db63": "get_tmp_seed",
    "0003db9e": "utoa",
    "0003dbeb": "itoa",
    "0003dc1b": "strlen",
    "0003dc68": "ultoa",
    "0003dcfa": "os_getmem",
    "0003dd97": "brk",
    "0003dea0": "fcloseall",
    "0003df3c": "dos_findfirst",
    "0003df89": "copy_finddata_from_dta",
    "0003dfe8": "build_exec_block",
    "0003e143": "build_cmdtail",
    "0003e1af": "dos_exec",
    "0003e27a": "copy_path_component",
    "0003e2bb": "splitpath2",
    "0003e471": "spawn_with_env",
    "0003e48e": "exec_with_env",
    "0003e4aa": "set_heap_alloc_flag",
    "0003e4c0": "flushall",
    "0003e4f8": "getche",
    "0003e513": "qwrite",
    "0003e5b4": "setenvp",
    "0003e698": "check_dos_carry",
    "0003e6ab": "spawn_search_path",
    "0003e816": "restore_cursor_bg",
    "0003ea6b": "save_cursor_bg",
    "0003eda6": "draw_mouse_pointer",
    "0003ee21": "end_cursor_draw",
    "0003f0f1": "save_sprite_bg",
    "0003f2b7": "restore_sprite_bg",
    "0003f4b4": "draw_vline",
    "0003f575": "draw_vline_buf2",
    "0003f636": "draw_hline",
    "0003f8bb": "draw_hline_buf2",
    "0003fb40": "fill_rect",
    "0003feb3": "fill_rect_buf2",
    "0004d434": "wait_vsync",
    "0004a8d1": "blit_cursor_sprite",
    "0000d758": "update_agent_proximity_sound",
    "0000daa8": "deliver_entity_event_if_visible",
    "0000e5a8": "run_mission_briefing",
    "0000f5e8": "mouse_to_world_iso",
    "0000fb48": "tile_passability_test",
    "0000fd38": "tile_passability_test_b",
    # --- helpers/thunks named this session (final naming pass) ---
    "00015ee8": "draw_box_outline",       "00022c58": "count_active_players",
    "00022e38": "init_netgame_session",   "0002f538": "register_visible_entity",
    "00036038": "schedule_priority_sound","00037848": "subobject_node_expired",
    "00015d98": "present_frame",          "00015e08": "draw_fixed_box",
    "00018878": "file_close",             "000188c8": "file_write",
    "000269b8": "grid_detach_object",     "00036188": "init_sound_driver",
    "00037818": "pool_table_lookup_a686", "00039088": "stop_current_sequence",
    "00039188": "shutdown_music_drivers", "00039f49": "skip_flic_chunk",
    "00039f69": "read_flic_palette",      "00038c28": "load_tagged_resource",
    "00039b0f": "sound_cmd_thunks_7e_84", "000399b3": "snd_cmd_65",
    "00039ae7": "snd_cmd_7b",             "00039af1": "snd_cmd_85",
    "00039afb": "snd_cmd_7c",             "00039b05": "snd_cmd_7d",
    "00039b55": "snd_cmd_96",             "00039b5f": "snd_cmd_97",
    "00039b73": "snd_cmd_99",             "00039b7d": "snd_cmd_9a",
    "00039b87": "snd_cmd_9b",             "00039b91": "snd_cmd_9c",
    "00039bb9": "snd_cmd_aa",             "00039bc3": "snd_cmd_ab",
    # --- named this session by the documentation/naming wave (runtime + game loops) ---
    "0001c2c8": "update_bullet_sfx_states",
    "000380b8": "update_pickup_states",
    "00029088": "update_static_object_states",
    "0001ab98": "render_sorted_sprites",
    "000254f8": "run_world_map_screen",
    "00034d48": "run_menu_sequence",
    "0002ef78": "update_agent_ai",
    "0002a828": "setup_mission_view",
    "00013258": "tick_agent_states",
    "00029fc8": "update_mission_lists",
    "0001b3f8": "plot_agents_minimap",
    "00026a18": "update_drift_vector",
    "0001aa08": "draw_minimap",
    "00022cc8": "refresh_netgame_map",
    "00027158": "sync_network_players",
    "000356c8": "process_mission_outcome",
    "0001b658": "draw_scanner_markers",
    "0001bb48": "edge_scroll_dispatch",
    "00020ef8": "reset_hud_render_state",
    "00039bd7": "sound_cmd_thunks",
    "00018a28": "draw_rect_outline",
    "0002c4e8": "refresh_item_slots",
    "00029a28": "draw_status_labels",
    "00034608": "pick_passable_shot_dir",
    "0002d5b8": "path_probe_0x40",
    "00033b88": "find_grid_entity_facing_0xc0",
    "00016678": "draw_player_budget_label",
    "000287e8": "dpmi_free_dos_mem",
    "00026ad8": "advance_aim_along_dir",
    "00023038": "is_even_entity_type",
    # --- named this session from reconstruction work (understanding recovered) ---
    "00019608": "draw_tactical_map",       "00033db8": "find_grid_entity_facing_0x80",
    "000288f8": "recv_chunked",            "00026778": "render_dashed_line",
    "00028728": "dos_alloc_and_zero",      "00030868": "reacquire_and_engage_target",
    "0002d038": "entity_update_target_lock","0002bca8": "widget_list_dispatch",
    "00034198": "march_shot_damage",       "000223c8": "reequip_squad_row",
    "0002bbe8": "anim_tick_frame_list",    "0001b858": "render_draw_list",
    "0002def8": "projectile_select_turn",  "0001b798": "draw_object_sprite_list",
    "00024be8": "startup_main",            "0000d928": "main_game_loop",
    "00020fc8": "new_campaign_reset",      "00018338": "validate_records_or_abort",
    "000184b8": "realloc_block_descriptor","000180f8": "file_open_read_close",
    "00018158": "alloc_init_with_errcode", "00018958": "open_detect_rnc_header",
    "00017998": "buffered_read",           "000179f8": "container_total_size",
    "00017b48": "container_load",          "00020c88": "keyboard_state_machine",
    "000254a8": "keyboard_hook_install",   "00028b88": "mouse_init_int33",
    "00025238": "guarded_init_alloc",      "000253a8": "pool_records_add",
    "00034c28": "noop_ret",                "00035d08": "sound_driver_init",
    "00038cf8": "xmidi_music_init",        "00035e68": "walk_sound_record_table",
    "000392ac": "sound_dispatch_trampoline","00039393": "clear_voice_tables",
    "000393e0": "install_timer_isr",       "00039467": "reprogram_pit_ch0",
    "000394c6": "recompute_timer_period",  "0003954c": "init_voice_tables",
    "00039846": "timer_rate_critsec",      "00024b08": "parse_hex_arg",
    "0003adb2": "int386",                  "0003b3b9": "segread",
    "0003c594": "heap_alloc",              "00025338": "init_input_subsystem",
    "00018458": "global_add_and_call",
    # cont.28 -- named from documented purposes (game subsystems)
    "0002ad58": "mission_target_resolve", "0002c578": "agent_hud_render",
    "00021658": "new_game_reset",         "000133a8": "entity_state_dispatch",
    "00027428": "mp_session_setup",       "0002a288": "radar_panel_sm",
    "0001bc28": "sel_marker_dispatch",    "00012da8": "build_equip_row",
    "00034858": "vehicle_drive_step",     "00015f58": "economy_daily_tick",
    "00033568": "commit_funding",         "0002bee8": "command_list_interp",
    "00034368": "compass_tile_probe",     "00019318": "draw_circle",
    "0002e5f8": "los_trace",              "0002e808": "los_trace_far",
    "000338d8": "table_save_restore",     "00022858": "mission_map_init",
    "0000f898": "passability_4corner",    "00029ad8": "mission_statusline",
    "000272b8": "mp_record_sync",         "0002d0d8": "rate_byte_drift",
    "000265d8": "stats_panel_draw",       "00027fc8": "submit_command",
    "00028228": "netbios_op91",           "00028368": "netbios_op90",
    "00020728": "menu_list_draw_b",       "00037918": "drop_carried_items",
    "000205f8": "menu_list_draw",         "000264a8": "slot_claim_test",
    "00026c78": "move_entity_xyz",        "0002d468": "path_probe_y",
    "00028118": "xfer_buf_req_b1",        "00029c58": "hud_icon_select",
    "0001a918": "minimap_mark_rect",      "00027d88": "submit_ncb",
    "0002ea88": "entity_behaviour_dispatch", "0002dd48": "entity_event_dispatch",
    "00028558": "netbios_recv95",         "0002d8c8": "anim_speed_select",
    "00027f08": "dpmi_alloc_5para",       "00033cf8": "grid_hit_y",
    "00033c38": "grid_hit_x",             "000263f8": "masked_blit_cell",
    "000284a8": "xfer_buf_req94",         "0001ba48": "clamp_point_box",
    # cont.28 batch 2
    "00015e38": "blit_cell_wh",           "0002f608": "entity_aim_helper",
    "00038fe8": "sound_channel_select",   "00036298": "hittest_draw",
    "00036338": "hittest_draw_b",         "00036c78": "pool_chain_reset",
    "0002e4f8": "find_projectile_step",   "00013b38": "palette_flash3",
    "00020568": "find_rec_by_field8",     "00034088": "shot_collision_query",
    "00036208": "measure_draw_text",      "0002c468": "copy_5fields_8recs",
    "0001c178": "spawn_pool_15e70",       "0001ff98": "draw_gauge_segment",
    "0000fa88": "z_probe_b",              "00022b38": "find_free_slot_15e70",
    "00020b78": "count_leading_nonzero",  "00022ba8": "spawn_pool_11670",
    "00026da8": "grid_unlink_object",     "000183e8": "walk_records_2c",
    "00013ac8": "reset_flash_palette",    "0002e588": "pool_list_dispatch",
    "00020be8": "count_leading_nonzero_b","0002d738": "projectile_step",
    "00013bc8": "pool_list_last",         "0002d358": "combat_aim_update",
    "000299c8": "center_string_16",       "000355d8": "blit_backbuffer",
    "00027e78": "far_alloc_probe",        "000365e8": "measure_text_width",
    "000361a8": "center_draw_string",     "0002d7a8": "interp_scale_a",
    "0002d868": "interp_scale_c",         "0002d808": "interp_scale_b",
    "000146f8": "eligibility_test",       "00036648": "text_width_kern",
    "00035ed8": "zero_even_bytes",        "00035f28": "record_max",
    "0001a8c8": "fill_minimap_grid",     "00035538": "bulk_dword_copy",
    "00035588": "bulk_dword_copy_b",     "00036c28": "walk_chain_1c",
    "0002d998": "recompute_state_code",  "00025168": "init_call5",
    "0000dc08": "oneshot_state_setter",  "00037778": "walk_chain_3a",
    "00037a48": "detach_chain_3a",       "000376f8": "chain_sum_3a",
    "000269d8": "advance_linked_index",  "00034048": "snap_direction",
    "00016638": "scan_syndicate_recs",   "00020d18": "relocate_map_columns",
    "000165f8": "count_syndicate_recs",  "00028cc8": "build_two_buffers",
    "00037738": "pool_accessor_44",      "0000e568": "lcg_rand",
    "00026aa8": "max_abs",               "000377b8": "chain_length",
    "00034168": "clamp_dec_field54",     "00014998": "walk_15byte_chain",
    "00018ab8": "store_4_globals",       "00018488": "sub_and_call",
    "00027ed8": "guarded_call_2arg",     "00025378": "add_c_stride6",
    "00014cc8": "scan_pool_4",           "00013a98": "forward_if_flag",
    # cont.28 batch 4
    "000252d8": "timer_setup",           "00022728": "init_call_seq",
    "0003c74f": "heap_boundary_check",   "0003c42d": "strupr",
    "00026998": "world_tick",       "00022708": "zero_bss_region",
    "00022ca8": "init_squad_slots",      "0003ca0d": "clib_ret_zero",
    "0001c168": "noop_ret_b",            "00039158": "seq_active_check",
    "00029a68": "draw_localized",        "0003aee6": "stricmp_call",
    "0000fee8": "copy_if_set",           "0000fa18": "z_probe",
    "00032118": "lookup_base_record",    "000226a8": "init_record_if_alloc",
    "00018828": "cond_3call",            "0002d6c8": "aim_step",
    "0001a458": "dispatch_jt45",
    "0002fe68": "persuade_capture",     "00030078": "follow_leader",
    "000301e8": "follow_leader_b",       "0002fca8": "vehicle_ride",
    "0002fa48": "vehicle_board",         "00031858": "entity_pool_tick",
    # Vehicle system (pool B cars @ 0xdd10, kind [0x18]=5; pool-A ride handlers)
    "0002fbc8": "vehicle_exit",          "0002f878": "vehicle_drive_state",
    "0002f908": "vehicle_move_drive",    "00036fd8": "vehicle_pool_tick",
    "00020d98": "vehicle_hp_stamp",
    # RNC (Rob Northen Compression) resource codec + block loader (data-file loading)
    "0003a1ec": "rnc_decompress",        "0003a37a": "rnc_read_be_len",
    "0003a3c6": "rnc_input_bits",        "0003a449": "rnc_make_huffman",
    "0003a383": "rnc_read_huffman",      "000188a8": "file_read_n",
    # cont.29 -- game-subsystem batch (named from documented headers; matched fns)
    "0002d228": "anim_frame_tick",       "0002d3b8": "aim_cursor_advance",
    "00030508": "entity_halve_hp",       "00030708": "entity_apply_damage",
    "00034118": "field54_sub_threshold", "000164c8": "player_target_sweep",
    "00035b68": "save_game",             "00026e18": "grid_insert_object",
    "00037658": "chain_link_node",       "00037878": "chain_unlink_node",
    "00028ec8": "map_row_col_index",     "00033fb8": "map_passability_check",
    "00037ad8": "best_weapon_select",    "00037d08": "best_weapon_select_typed",
    "00014078": "net_sync_build",        "000279f8": "conn_status_scan",
    "00028878": "xfer_chunked",          "00012ca8": "session_init",
    "000188e8": "load_unpack_file",      "00035638": "set_message_line",
    # cont.29 -- unclassified batch (clear-semantic matched fns; generic forwarders left as FUN_)
    "000139a8": "kill_credit_eligible",  "00014c58": "sum_of_squares_call",
    "00016438": "syndicate_timer_pct",   "00022ed8": "reset_player_slot",
    "000289a8": "report_net_status",     "00028d08": "probe_nesw_zones",
    "0002de18": "launch_move_setup",     "0002ee18": "find_target_for_agent",
    "00035c88": "read_name_record",      "00036168": "guarded_sound_call",
    "00036d18": "flag_hp_adjust",        "000377e8": "pool_table_lookup",
    # cont.29 -- Watcom C runtime library fns, identified by tools/libname.py (byte match vs
    # CLIB3S.LIB module). One name per library module (highest coverage); helpers stay FUN_.
    "0003a526": "atol",       "0003dfcf": "cenvarg",   "0003db36": "chktty",
    "0003b239": "d_getvec",   "0003b273": "d_setvec",  "0003b9ee": "nibble_to_hex",
    "0003d3e4": "__filbuf",   "0003b972": "freopen",   "0003b539": "fgetc",
    "0003da03": "ftell",      "0003d894": "ioalloc",   "0003c44d": "isatty",
    "0003aed8": "labs",       "0003a93b": "lseek",     "0003dcb5": "ltoa",
    "0003e361": "canon_path_sep", "0003a579": "open",  "0003b22d": "outp",
    "0003be40": "prtf",       "0003d935": "qread",     "0003cbf9": "spawnve",
    "0003e7f7": "strchr",     "0003a8d7": "strcpy",    "0003aef9": "stricmp",
    "0003aea6": "strncmp",    "0003deee": "strnicmp",  "0003a97c": "tell",
    "0003da37": "tolower",    "0003dce5": "toupper",   "0003e590": "unlink",
    # cont.29 -- runtime lib, medium libname coverage but corroborated by each fn's own
    # header/signature (printf/system/rewind/getstream headers; sprintf/dosret by usage).
    "0003ad66": "printf",     "0003af38": "system",    "0003b407": "rewind",
    "0003a4fa": "sprintf",    "0003c479": "dosret",    "0003b90d": "getstream",
    # cont.30 -- CLIB disambiguation: the earlier libname pass put several names on the wrong
    # sibling (each pair confirmed by db-byte disasm + caller x-ref). freopen/nibble_to_hex/
    # canon_path_sep/__filbuf above are the corrected holders; the real fopen/fclose/makepath/
    # fgetc/fread sit here. 0x3d3e4 was mis-called fgetc but is __filbuf (the getc-underflow
    # refill), so the real text-mode fgetc frees to 0x3b539 and the real fread to 0x3b420.
    # remove is C remove()-over-unlink; the two spawn_* are the low-level spawn engine (NOT
    # system/sprintf -- those two were already correct).
    "0003b8f8": "fopen",      "0003b99e": "fclose",    "0003e381": "makepath",
    "0003b420": "fread",      "0003db69": "remove",    "0003cc74": "spawn_exec_core",
    "0003cfce": "exec_shell_forward",
    # cont.29 -- hand-asm graphics blitters (fully commented in their .asm companions)
    "00040236": "plot_point",     "0004d199": "fill_bytes",
    "0004d0b4": "blit_block",     "0004a734": "draw_sprite_rle",
    "0004a492": "vga_planar_present",
    # cont.30 -- hand-asm gfx/sound fns named from their commented .asm listings
    "0004287e": "iso_scene_walk",
    "00039ca0": "flic_play",      "00039e42": "flic_decode_frame",
    "00039ee2": "flic_parse_header", "00039f92": "flic_load_palette",
    "0003942f": "uninstall_timer_isr", "00039a82": "stop_voice",
    "0004cb68": "draw_text",      "0004cde3": "draw_text_fixed",
    "0004cacc": "draw_hex",       "0004cb1a": "draw_dec",
    "0004d221": "vec_to_angle",   "0004d393": "isqrt32",
    "0004d352": "isqrt16",        "0004d1db": "copy_bytes",
    "0004d04b": "copy_fixed_block", "0004d442": "poll_key",
    "0004d513": "hex32_to_ascii", "0004d451": "dec32_to_ascii",
    "000498ef": "set_video_mode", "0004987e": "upload_palette",
    "00049922": "reset_gfx_write_mode", "0004993b": "clear_offscreen",
    "0004997e": "clear_vga_screen",
    # cont.31 -- gfx/sound/game fns named from the fleet's commented listings + headers
    "000402e0": "plot_op_dispatch",  "000404b8": "plot_op_dispatch2",
    "00040f4e": "draw_tile_region",  "0004107b": "iso_tile_dispatch",
    "00047a7e": "iso_cell_dispatch",  "00045e61": "iso_block_dispatch",
    "00045f8a": "iso_block_dispatch2",  "000418ac": "merge_cell_mask",
    "00041a44": "occlusion_walk",  "000436b2": "occlusion_walk_excl",
    "0004483d": "occlusion_walk_excl2",  "00046188": "clear_occlusion_mask",
    "00049a31": "fill_planar_column",  "00049b67": "draw_animated_marker",
    "00049edf": "present_patch_gated",  "0004a3cc": "restore_backing_column",
    "0004a42f": "save_backing_column",  "0004a574": "present_frame_dispatch",
    "0004a5a8": "present_frame_planar",  "0004a63a": "draw_sprite_buf",
    "0004a66b": "draw_sprite_surf",  "0004a69c": "blit_sprite",
    "0004a6c8": "draw_sprite_rle_buf",  "0004a6fe": "draw_sprite_rle_vga",
    "0004a898": "capture_sprite",  "0004a909": "blit_width_dispatch",
    "0004b073": "span_op_dispatch",  "0004cc4d": "glyph_dispatch_wide",
    "0004cc77": "glyph_blit_wide",  "0004cd19": "glyph_dispatch_narrow",
    "0004cd3e": "glyph_blit_narrow",  "0004ce87": "glyph_stamp_fixed",
    "0004d069": "build_empty_slot_mask",  "000391a8": "run_seq_commands",
    "00039280": "driver_msg_lookup",  "00039495": "program_pit_period_us",
    "000395b6": "unload_all_drivers",  "00039625": "alloc_seq_slot",
    "000396d5": "free_seq_slot",  "00039722": "stop_all_seqs",
    "00039747": "start_seq",  "0003979c": "pause_seq",
    "000397f1": "set_seq_period",  "00039879": "set_seq_rate",
    "000398d7": "register_driver",  "00039966": "unregister_driver",
    "00039994": "voice_get_driver_obj",  "000399bd": "start_voice",
    "00011d68": "find_blocking_entity",  "000128b8": "find_nearby_ped",
    "00012ae8": "find_related_ped",  "00016318": "research_funding_tick",
    "00018ae8": "draw_line",  "00018d18": "draw_filled_shape",
    "0001b908": "draw_slot_record_chain",  "00021e18": "store_squad_loadout",
    "00022768": "find_free_pool_slots",  "00025348": "install_int9_handler",
    "00026ba8": "vec_to_angle_short",  "00026bc8": "aim_step_clamp",
    "00028628": "xfer_buf_req35",  "0002cf28": "refresh_hud_inventory",
    "0002d9e8": "squad_threat_test",  "0002e408": "homing_step",
    "0002ed28": "record_kill_stats",  "00033e78": "map_tile_hit_dispatch",
    "00035f78": "play_sound_slot",  "00020018": "menu_list_hit_test",
    "00020158": "draw_item_detail",  "00025d58": "draw_agent_detail",
    "000363d8": "draw_wrapped_text",  "00036698": "draw_ui_text",
    "00036808": "text_input_widget",  "000149e8": "apply_command_effect",
    "00023158": "run_mission_command",  "0002c218": "command_list_interp2",
}


def header_desc(path):
    t = open(path, encoding="utf-8", errors="replace").read()
    m = re.search(r"/\*(.*?)\*/", t, re.S)
    if not m:
        return ""
    b = re.sub(r"\s+", " ", m.group(1)).strip()
    b = re.sub(r"^FUN_[0-9a-fA-F]+ @ 0x[0-9a-fA-F]+ \([0-9]+B\)\s*-{0,2}\s*", "", b)
    b = re.sub(r"^@ ?0x[0-9a-fA-F]+ ?\([0-9]+B\):?\s*", "", b)
    # keep the generated map in James's style: no em dashes or semicolons in prose
    b = b.replace(" -- ", ", ").replace(" — ", ", ").replace("—", ", ")
    b = b.replace("; ", ", ").replace(";", ",")
    return b[:200]


def main():
    man = json.load(open("manifest/functions.json"))["functions"]
    by = {f["addr"]: f for f in man}
    src_for = {}
    for p in glob.glob("src/**/*.c", recursive=True):
        nm = os.path.basename(p)[:-2]
        if re.match(r"FUN_[0-9a-fA-F]+$", nm):
            src_for[nm] = p

    # intro-executing set (from the trace), for the ordered map
    execset = set()
    if os.path.exists("build/tr_orig.bin"):
        d = open("build/tr_orig.bin", "rb").read(); n = len(d) // 4
        offs = set(struct.unpack("<%dI" % n, d[:n*4]))
        starts = sorted(int(f["addr"], 16) for f in man)
        rng = sorted((int(f["addr"], 16), int(f["addr"], 16) + f["size"]) for f in man)
        lo = [r[0] for r in rng]
        for o in offs:
            a = o + 0xd748
            i = bisect.bisect_right(lo, a) - 1
            if i >= 0 and rng[i][0] <= a < rng[i][1]:
                execset.add("%08x" % rng[i][0])

    names = {}
    for f in man:
        addr = f["addr"]; nm = f["name"]
        p = src_for.get(nm)
        sub = p.split("/")[1] if p and p.startswith("src/") and "/" in p[4:] else ""
        names[nm] = {"addr": addr, "label": LABELS.get(addr, ""),
                     "subsystem": sub, "desc": header_desc(p) if p else "",
                     "runs_in_intro": addr in execset}
    json.dump(names, open("manifest/names.json", "w"), indent=1)

    # readable map of the intro path
    intro = sorted(a for a in execset)
    with open("docs/function-map.md", "w", encoding="utf-8") as o:
        o.write("# Function map\n\n")
        o.write("Semantic names and descriptions over the `FUN_<addr>` anchors "
                "(the source filename stays `FUN_<addr>`, this is the readable layer). "
                "Full machine-readable map in `manifest/names.json`.\n\n")
        o.write("## Intro path (boot -> Bullfrog logo -> main menu), %d functions that execute\n\n" % len(intro))
        o.write("| addr | name | subsystem | what it does |\n|---|---|---|---|\n")
        for a in intro:
            nm = "FUN_%s" % a; e = names.get(nm, {})
            lbl = e.get("label") or "*(unnamed)*"
            dd = (e.get("desc") or "").replace("|", "\\|")[:110]
            o.write("| `0x%s` | **%s** | %s | %s |\n" % (a.lstrip("0") or "0", lbl, e.get("subsystem", ""), dd))
    named = sum(1 for a in intro if names.get("FUN_%s" % a, {}).get("label"))
    print("wrote manifest/names.json (%d functions) + docs/function-map.md" % len(names))
    print("intro path: %d functions, %d given semantic names, %d described-only" % (len(intro), named, len(intro) - named))


if __name__ == "__main__":
    main()
