#!/usr/bin/env python3
"""Write LoRA captions for the Syndicate tiles: visual description + game-data
function tag (from COL01.DAT tile types) + fixed style tail.

  python3 tools/gen_tile_captions.py <COL01.DAT> <dataset dir>
"""
import os, sys
import rnc

TRIG = "syndtile isometric pixel-art tile"
TAIL = "dark dystopian city, retro dos-game pixel art"

# game tile-type byte (COL01.DAT) -> gameplay function phrase.
# Slope/road directions are the game's own SN/NS/EW/WE codes (low end at ground
# level, rising towards the named side).
FUNC = {
    0x00: "non-solid overhead element you walk behind, upper z-layer",
    0x01: "walkable ramp rising towards the north",
    0x02: "walkable ramp rising towards the south",
    0x03: "walkable ramp rising towards the west",
    0x04: "walkable ramp rising towards the east",
    0x05: "walkable ground, floor level",
    0x06: "walkable road edge along the east-west side",
    0x07: "walkable road edge along the west-east side",
    0x08: "walkable road edge along the south-north side",
    0x09: "walkable road edge along the north-south side",
    0x0A: "solid wall that blocks movement",
    0x0B: "walkable curved road corner",
    0x0C: "thin handrail, fence or light you walk behind, upper z-layer",
    0x0D: "elevated roof or rooftop structure, upper z-layer",
    0x0E: "walkable pedestrian road crossing",
    0x0F: "walkable road with lane markings",
    0x10: "non-solid overhead element you walk behind, upper z-layer",
}

# per-tile visual descriptions (filled in as tiles are viewed)
VISUAL = {}


def write(col01_path, out):
    d = open(col01_path, "rb").read()
    types = rnc.unpack(d) if d[:3] == b"RNC" else d
    n = 0
    for tid, vis in VISUAL.items():
        func = FUNC.get(types[tid], "tile")
        cap = "%s, %s, %s, %s\n" % (TRIG, vis, func, TAIL)
        open(os.path.join(out, "tile_%03d.txt" % tid), "w").write(cap)
        n += 1
    return n


if __name__ == "__main__":
    print("import this module, set VISUAL, then call write()")
