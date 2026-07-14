#!/usr/bin/env python3
"""origbuild.py -- build a runnable image at ORIGINAL addresses (the reconstruction path).

Rationale (see AGENTS.md): the game resolves cross-references three ways -- baked rel32 (relative),
baked absolute [disp] data refs (DS-relative), and 851 function-POINTERS baked into OBJ2 data. All of
these are only valid at the ORIGINAL memory layout (code@0x10000, data@0x50000). Rather than relink
everything symbolically, place the whole code image at 0x10000 and the data at its DS base, entry at
the genuine startup 0x3d85c -- then every baked pointer is already correct.

This first cut uses the ORIGINAL code image (OBJECT1.linear.bin) as one _TEXT blob to answer the key
question: does DOS/4GW accept a wlink-built LE like this (the 1012 check)? If yes, we have a running
game and can then splice our per-function objects in at their addresses (matched == identical bytes).

  python3 tools/origbuild.py         # build build/GAMEO.EXE
"""
import struct, os

SEG1 = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
DATA = "build/dataimg.obj"                 # contiguous OBJ2|OBJ3|OBJ4 (from mkdata.py)
CODE_BASE = 0x10000
ENTRY = 0x3afa4        # __x386_start (jmp over version strings) -- the true LE entry. The LE header
                      # EIP (0x2d85c) maps here via the +0xd748 linear.bin offset, NOT to 0x3d85c
                      # (which is an int-dispatch table).

idx  = lambda v: bytes([v]) if v < 0x80 else bytes([0x80 | (v >> 8), v & 0xff])
pstr = lambda s: bytes([len(s)]) + s.encode("latin1")
def rec(rt, body):
    ln = len(body) + 1
    return bytes([rt, ln & 0xff, ln >> 8]) + body + bytes([0])


def make_code_obj(path):
    code = open(SEG1, "rb").read()
    out = bytearray()
    out += rec(0x80, pstr("GAMECODE"))
    out += rec(0x96, pstr("") + pstr("_TEXT") + pstr("CODE") + pstr("STACK"))
    # SEGDEF32 _TEXT: ACBP para|public|use32 = 0x69; segname=_TEXT(2) class=CODE(3) ovl=""(1)
    out += rec(0x99, bytes([0x69]) + struct.pack("<I", len(code)) + idx(2) + idx(3) + idx(1))
    # SEGDEF32 STACK: ACBP para|stack|use32 = 0x75; 64k; segname=STACK(4) class=STACK(4)
    out += rec(0x99, bytes([0x75]) + struct.pack("<I", 0x10000) + idx(4) + idx(4) + idx(1))
    # LEDATA32 in 1024-byte chunks
    off = 0
    while off < len(code):
        chunk = code[off:off+1024]
        out += rec(0xA1, idx(1) + struct.pack("<I", off) + chunk)
        off += len(chunk)
    # MODEND32 (0x8B) with start address: Main|Start|Logical(0xC1); End-Data 0x50 = frame method 5
    # (frame=target), target by SEGDEF index; target=_TEXT(1); 4-byte offset (entry within object).
    out += rec(0x8B, bytes([0xC1, 0x50]) + idx(1) + struct.pack("<I", ENTRY - CODE_BASE))
    open(path, "wb").write(out)
    print("code obj: %d bytes of code, entry offset 0x%x" % (len(code), ENTRY - CODE_BASE))


if __name__ == "__main__":
    os.makedirs("build", exist_ok=True)
    make_code_obj("build/gamecode.obj")
    print("wrote build/gamecode.obj  (+ needs build/dataimg.obj from mkdata.py)")
