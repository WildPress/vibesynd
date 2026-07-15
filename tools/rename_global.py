#!/usr/bin/env python3
"""rename_global.py -- rename generic g_<hex> globals to semantic names, safely.

Symbol names do NOT affect compiled bytes (globals are relocated externs, masked by the
reloc-aware matcher), so renaming never breaks a match. The whole-game build stays correct
because mkdata.py now resolves a global's data-image address REGISTRY-FIRST from
manifest/globals.json (this file), so the address -- not the name -- is the anchor.

For each (old g_<hex>, new semantic name): replace `\bold\b` in every src/**/*.c (code AND
comments, so they stay consistent) and record new -> {addr, type, desc} in globals.json.
Re-runnable: an old name already gone just renames 0 occurrences.

  python3 tools/rename_global.py            # apply the built-in catalogue below
  python3 tools/rename_global.py g_5338 g_rec8_table "u8*" "8-byte record table"   # one-off
"""
import re, glob, json, sys, os

REG = "manifest/globals.json"

# High-confidence catalogue, derived from docs/object-model.md (each already proven from
# matched bytes) and corroborating source-header comments.
CATALOGUE = {
    "g_810e": ("g_entity_pool",   "u8[]",     "entity/object pool A, base-2; node = g_entity_pool + id (id is a byte offset)"),
    "g_10e":  ("g_grid_heads",    "u16[]",    "128x128 spatial-grid head table: cell -> first entity id"),
    "g_ab60": ("g_dir_dx",        "s16[256]", "direction -> dx vector (256-step angle)"),
    "g_ad60": ("g_dir_dy",        "s16[256]", "direction -> dy vector (256-step angle)"),
    "g_5368": ("g_screen_buf",    "u8*",      "front screen buffer (blit target)"),
    "g_5370": ("g_back_buf",      "u8*",      "back screen buffer (blit target)"),
    "g_5338": ("g_rec8_table",    "u8*",      "table of 8-byte records (linked via +6, flag +5)"),
    "g_5350": ("g_objective_slots","u8*",     "0x20-stride record table (map/objective slots)"),
    "g_5358": ("g_map_cols",      "int*",     "map column-pointer table (built by 0x20d18)"),
    "g_10b5a":("g_shot_level",    "s16",      "shot cursor: level accumulator (collision walk)"),
    "g_10b5c":("g_shot_y",        "s16",      "shot cursor: y accumulator (collision walk)"),
    "g_10b5e":("g_shot_x",        "s16",      "shot cursor: x accumulator (collision walk)"),
    "g_10b22":("g_cursor_x",      "u16",      "cursor point x (tested against record boxes)"),
    "g_10b24":("g_cursor_y",      "u16",      "cursor point y (tested against record boxes)"),
    "g_db2c": ("g_minimap",       "u8[]",     "0x19 x 0x10 minimap grid"),
}


def load_reg():
    try:
        return json.load(open(REG))
    except (FileNotFoundError, ValueError):
        return {}


def is_hex(name):
    return re.fullmatch(r"g_[0-9a-fA-F]+", name) is not None


def apply(renames):
    reg = load_reg()
    files = glob.glob("src/**/*.c", recursive=True)
    total = {}
    for old, (new, typ, desc) in renames.items():
        assert old.startswith("g_") and is_hex(old), "old must be g_<hex>: " + old
        assert not is_hex(new), "new name must contain a non-hex char (mkdata disambiguation): " + new
        addr = old[2:]
        pat = re.compile(r"\b" + re.escape(old) + r"\b")
        n = 0
        for p in files:
            txt = open(p, encoding="utf-8", errors="replace").read()
            new_txt, c = pat.subn(new, txt)
            if c:
                open(p, "w", encoding="utf-8", newline="\n").write(new_txt)
                n += c
        reg[new] = {"addr": addr, "type": typ, "desc": desc}
        total[old] = (new, n)
        print(f"{old:9s} -> {new:20s} {n:4d} refs   [{typ}] {desc}")
    with open(REG, "w", newline="\n") as fp:
        json.dump(dict(sorted(reg.items())), fp, indent=1)
    renamed = sum(n for _, n in total.values())
    print(f"\n{len(renames)} globals renamed, {renamed} references updated; registry -> {REG}")


def main():
    if len(sys.argv) >= 4:
        old, new, typ = sys.argv[1], sys.argv[2], sys.argv[3]
        desc = sys.argv[4] if len(sys.argv) > 4 else ""
        apply({old: (new, typ, desc)})
    else:
        apply(CATALOGUE)


if __name__ == "__main__":
    main()
