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
}


def header_desc(path):
    t = open(path, encoding="utf-8", errors="replace").read()
    m = re.search(r"/\*(.*?)\*/", t, re.S)
    if not m:
        return ""
    b = re.sub(r"\s+", " ", m.group(1)).strip()
    b = re.sub(r"^FUN_[0-9a-fA-F]+ @ 0x[0-9a-fA-F]+ \([0-9]+B\)\s*-{0,2}\s*", "", b)
    b = re.sub(r"^@ ?0x[0-9a-fA-F]+ ?\([0-9]+B\):?\s*", "", b)
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
        o.write("Semantic names + descriptions over the `FUN_<addr>` anchors "
                "(source filename stays `FUN_<addr>`; this is the readable layer). "
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
