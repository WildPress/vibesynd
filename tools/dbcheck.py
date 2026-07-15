#!/usr/bin/env python3
"""dbcheck.py -- does a db-transcription .c compile to the byte-exact target? For the big library
functions that dbgen's helper ceiling left parked. Prints match/diff per name.
    docker run --rm -v "$PWD":/work -w /work synd-decomp python3 tools/dbcheck.py FUN_000436b2 ...
"""
import sys, os
sys.path.insert(0, "tools")
import regdiff
from omf import text_bytes_and_fixups

for name in sys.argv[1:]:
    try:
        tb, _ = regdiff.load_target(name)
    except StopIteration:
        print(f"{name}: not in manifest"); continue
    c = regdiff.compile_one(name, "-4s -s -zq")
    if not c:
        print(f"{name}: COMPILE-FAIL"); continue
    ob, fx = c
    exact = len(ob) == len(tb) and regdiff.mask(tb, fx) == regdiff.mask(ob, fx)
    print(f"{name}: target={len(tb)}B ours={len(ob)}B fixups={len(fx)} -> "
          + ("*** BYTE MATCH ***" if exact else f"DIFF (score {regdiff.analyze(tb, ob, fx)['score']*100:.0f}%)"))
