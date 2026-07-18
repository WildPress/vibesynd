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
    "00026998": "init_subsystems",       "00022708": "zero_bss_region",
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
    "0003b239": "d_getvec",   "0003b273": "d_setvec",  "0003b9ee": "fclose",
    "0003d3e4": "fgetc",      "0003b972": "fopen",     "0003b539": "fread",
    "0003da03": "ftell",      "0003d894": "ioalloc",   "0003c44d": "isatty",
    "0003aed8": "labs",       "0003a93b": "lseek",     "0003dcb5": "ltoa",
    "0003e361": "makepath",   "0003a579": "open",      "0003b22d": "outp",
    "0003be40": "prtf",       "0003d935": "qread",     "0003cbf9": "spawnve",
    "0003e7f7": "strchr",     "0003a8d7": "strcpy",    "0003aef9": "stricmp",
    "0003aea6": "strncmp",    "0003deee": "strnicmp",  "0003a97c": "tell",
    "0003da37": "tolower",    "0003dce5": "toupper",   "0003e590": "unlink",
    # cont.29 -- runtime lib, medium libname coverage but corroborated by each fn's own
    # header/signature (printf/system/rewind/getstream headers; sprintf/dosret by usage).
    "0003ad66": "printf",     "0003af38": "system",    "0003b407": "rewind",
    "0003a4fa": "sprintf",    "0003c479": "dosret",    "0003b90d": "getstream",
    # cont.29 -- hand-asm graphics blitters (fully commented in their .asm companions)
    "00040236": "plot_point",     "0004d199": "fill_bytes",
    "0004d0b4": "blit_block",     "0004a734": "draw_sprite_rle",
    "0004a492": "vga_planar_present",
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
