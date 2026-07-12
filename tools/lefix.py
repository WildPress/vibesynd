#!/usr/bin/env python3
"""Correct LE fixup-table parser for SYNDICAT_MAIN.EXE (replaces the buggy
tools/archive/le_fixups.py, whose record walker ignored ADDITIVE fixups
(target flag 0x04) and desynced, yielding wrong source addresses).

Record layout (LE/LX FIXUPP):
  u8 src_flags   low nibble = source type (2=sel16, 5=off16, 6=far16:32,
                 7=off32, 8=self-rel32); 0x10 = alias flag; 0x20 = source LIST
  u8 trg_flags   low 2 bits = target type (0 internal / 1 import-ord /
                 2 import-name / 3 entry); 0x04 = ADDITIVE value present;
                 0x10 = 32-bit target offset; 0x20 = 32-bit additive;
                 0x40 = 16-bit object/module number
  then: source offset s16 (or u8 count if list)
  then: target block (object number + optional offset, by type)
  then: additive value u16/u32 if trg_flags & 0x04
  then: if list, count x s16 source offsets

KEY FINDING (validated: 8688/8688 stored==toff, zero mismatches): the fixup
tables are SPEC-CORRECT; it is SYNDICAT_MAIN_OBJECT1.linear.bin that is
extracted 0x28b8 bytes (= e_lfanew, the MZ/DOS4GW stub size) INTO object 1 --
its first 0x28b8 bytes of code are missing from the file, and every manifest
address is uniformly (true_obj1_offset + 0x10000 - 0x28b8). All relative
comparisons (150+ byte-matches) were unaffected. Consequences:
  - this parser reports sources in MANIFEST space (it subtracts 0x28b8);
  - target offsets convert as manifest = toff + 0x10000 - 0x28b8;
  - "callees below 0x10000" (0xfa18, 0xfa88, 0xffc8, 0xe568, ...) are real
    functions inside the cut-off prefix, reachable as manifest [0xd748,0x10000);
  - the LE entry point (obj1:+0x2d85c) is manifest 0x3afa4-ish (RTL cstart),
    NOT the bogus size-1 manifest entry at 0x2d85c.

Self-check: for internal fixups with 32-bit offset sources, the dword STORED
at the source location equals the record's target offset (this linker writes
the target offset as the placeholder). Validation prints the match rate.

Usage:
  python3 tools/lefix.py check                    # parse + self-check
  python3 tools/lefix.py src <linear_hex> [n]     # fixups whose source covers addr (n=range)
  python3 tools/lefix.py tgt <obj> <off_hex> [n]  # fixups targeting obj:+off..+off+n
"""
import struct, sys

EXE = "inputs/SYNDICAT_MAIN.EXE"

def u8(b, o):  return b[o]
def u16(b, o): return struct.unpack_from("<H", b, o)[0]
def s16(b, o): return struct.unpack_from("<h", b, o)[0]
def u32(b, o): return struct.unpack_from("<I", b, o)[0]

def parse():
    data = open(EXE, "rb").read()
    H = u32(data, 0x3c)
    assert data[H:H+2] in (b"LE", b"LX"), "no LE/LX header"
    page_size   = u32(data, H+0x28)
    n_pages     = u32(data, H+0x14)
    obj_tab_off = u32(data, H+0x40) + H
    n_objects   = u32(data, H+0x44)
    fpt_off     = u32(data, H+0x68) + H
    frt_off     = u32(data, H+0x6c) + H
    objs = []
    for i in range(n_objects):
        o = obj_tab_off + i*24
        objs.append(dict(num=i+1, vsize=u32(data, o), base=u32(data, o+4),
                         flags=u32(data, o+8), pidx=u32(data, o+12),
                         pcnt=u32(data, o+16)))
    def page_obj(logical_page):
        for ob in objs:
            if ob["pidx"] <= logical_page < ob["pidx"] + ob["pcnt"]:
                return ob, (logical_page - ob["pidx"]) * page_size
        return None, 0
    fpt = [u32(data, fpt_off + 4*i) for i in range(n_pages + 1)]
    fixups = []   # (src_lin, src_type, ttype, tobj, toff, additive)
    bad = 0
    for p in range(1, n_pages + 1):
        ob, page_off = page_obj(p)
        rec = frt_off + fpt[p-1]
        end = frt_off + fpt[p]
        while rec < end:
            sf = u8(data, rec); tf = u8(data, rec+1); rec += 2
            stype = sf & 0x0f
            if sf & 0x20:
                cnt = u8(data, rec); rec += 1
                srcoffs = None
            else:
                srcoffs = [s16(data, rec)]; rec += 2
            ttype = tf & 0x03
            tobj = toff = additive = None
            if ttype == 0:
                tobj = u16(data, rec) if tf & 0x40 else u8(data, rec)
                rec += 2 if tf & 0x40 else 1
                if stype != 0x02:
                    toff = u32(data, rec) if tf & 0x10 else u16(data, rec)
                    rec += 4 if tf & 0x10 else 2
            elif ttype == 1:
                rec += 2 if tf & 0x40 else 1
                rec += 4 if tf & 0x80 else 2
            elif ttype == 2:
                rec += 2 if tf & 0x40 else 1
                rec += 4 if tf & 0x10 else 2
            else:
                rec += 2 if tf & 0x40 else 1
            if tf & 0x04:
                additive = u32(data, rec) if tf & 0x20 else u16(data, rec)
                rec += 4 if tf & 0x20 else 2
            if srcoffs is None:
                srcoffs = [s16(data, rec + 2*k) for k in range(cnt)]
                rec += 2*cnt
            if ob is None:
                bad += len(srcoffs); continue
            for so in srcoffs:
                # stub-anchored grid: subtract e_lfanew (0x28b8) from the
                # naive page*0x1000+srcoff position (validated fp4-64).
                fixups.append((ob["base"] + page_off + so - 0x28b8, stype,
                               ttype, tobj, toff, additive, p))
        if rec != end:
            print(f"WARN: page {p} record walk ended at {rec-frt_off:#x}, "
                  f"expected {end-frt_off:#x}", file=sys.stderr)
    return data, objs, fixups

def main():
    data, objs, fixups = parse()
    mode = sys.argv[1] if len(sys.argv) > 1 else "check"
    if mode == "check":
        print(f"objects: " + "  ".join(
            f"obj{o['num']} base={o['base']:#x} vsize={o['vsize']:#x}" for o in objs))
        print(f"total fixup sources: {len(fixups)}")
        from collections import Counter
        print("by source type:", dict(Counter(f[1] for f in fixups)))
        print("by target type:", dict(Counter(f[2] for f in fixups)))
        print("with additive:", sum(1 for f in fixups if f[5] is not None))
        # self-check: internal 32-bit-offset fixups, source inside obj1
        ok = miss = outside = 0
        o1 = objs[0]
        for src, stype, ttype, tobj, toff, add, pg in fixups:
            if ttype != 0 or stype != 7 or toff is None:
                continue
            fo = src - o1["base"]
            if not (0 <= fo <= o1["vsize"] - 4):
                outside += 1; continue
            seg = open_obj1()
            stored = u32(seg, fo)
            want = toff + (add or 0)
            if stored == want: ok += 1
            else:
                miss += 1
                if miss <= 10:
                    print(f"  MISMATCH src={src:#x} stored={stored:#x} "
                          f"tgt=obj{tobj}:+{toff:#x} add={add}")
        print(f"self-check (stored dword == target offset): "
              f"{ok} ok, {miss} mismatch, {outside} outside obj1")
    elif mode == "src":
        want = int(sys.argv[2], 16)
        rng = int(sys.argv[3], 16) if len(sys.argv) > 3 else 4
        for src, stype, ttype, tobj, toff, add, pg in fixups:
            if want - 4 < src < want + rng:
                print(f"src={src:#x} stype={stype} ttype={ttype} "
                      f"-> obj{tobj}:+{toff:#x} add={add}"
                      if toff is not None else
                      f"src={src:#x} stype={stype} ttype={ttype} tobj={tobj}")
    elif mode == "tgt":
        tobj_want = int(sys.argv[2])
        lo = int(sys.argv[3], 16)
        hi = lo + (int(sys.argv[4], 16) if len(sys.argv) > 4 else 4)
        hits = [(s, st, tt, to, tf, ad) for (s, st, tt, to, tf, ad, pg) in fixups
                if tt == 0 and to == tobj_want and tf is not None and lo <= tf < hi]
        hits.sort(key=lambda x: x[4])
        for src, stype, ttype, tobj, toff, add in hits:
            print(f"src={src:#x} stype={stype} -> obj{tobj}:+{toff:#x} add={add}")

_seg = None
def open_obj1():
    global _seg
    if _seg is None:
        _seg = open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "rb").read()
    return _seg

if __name__ == "__main__":
    main()
