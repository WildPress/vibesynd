#!/usr/bin/env python3
"""Minimal LE (Linear Executable) fixup-table parser for SYNDICAT_MAIN.EXE.

Goal (first cut): resolve individual relocations so we can tell what an address
constant in the un-relocated OBJECT1 image *really* points to once the loader's
fixups are applied. This is the first brick of the full-relocation solution.

Usage:
    python3 tools/le_fixups.py [SOURCE_ADDR_HEX]
Prints the LE header summary, object table, total fixup count, and (if given, or
using the default) the fixup whose SOURCE lands on that linear address.

Linear addresses here match Ghidra's: object k's bytes live at [reloc_base,
reloc_base+size), and OBJECT1's reloc_base is 0x10000.
"""
import struct, sys

EXE = "inputs/SYNDICAT_MAIN.EXE"

# our accessor FUN_0003c46d @ 0x3c46d = B8 <imm32> C3 ; imm32 starts at +1
DEFAULT_SRC = 0x3c46e

def u8(b, o):  return b[o]
def u16(b, o): return struct.unpack_from("<H", b, o)[0]
def s16(b, o): return struct.unpack_from("<h", b, o)[0]
def u32(b, o): return struct.unpack_from("<I", b, o)[0]

def main():
    data = open(EXE, "rb").read()
    e_lfanew = u32(data, 0x3c)
    H = e_lfanew
    sig = data[H:H+2]
    print(f"MZ e_lfanew = {e_lfanew:#x}  LE header sig = {sig!r}")
    if sig not in (b"LE", b"LX"):
        sys.exit("not an LE/LX image at e_lfanew")

    page_size   = u32(data, H+0x28)
    n_pages     = u32(data, H+0x14)
    obj_tab_off = u32(data, H+0x40) + H
    n_objects   = u32(data, H+0x44)
    fpt_off     = u32(data, H+0x68) + H   # fixup page table  (+0x64 is dircnt!)
    frt_off     = u32(data, H+0x6c) + H   # fixup record table
    eip_obj     = u32(data, H+0x18)
    eip         = u32(data, H+0x1c)
    print(f"page_size={page_size:#x}  n_pages={n_pages}  n_objects={n_objects}")
    print(f"obj_table@{obj_tab_off:#x}  fixup_page_table@{fpt_off:#x}  fixup_rec_table@{frt_off:#x}")
    print(f"entry: object {eip_obj}, EIP {eip:#x}")

    # --- object table (24 bytes/entry) ---
    objs = []
    print("\n# Object table")
    for i in range(n_objects):
        o = obj_tab_off + i*24
        vsize = u32(data, o+0x00)
        base  = u32(data, o+0x04)
        flags = u32(data, o+0x08)
        pidx  = u32(data, o+0x0c)   # 1-based first page
        pcnt  = u32(data, o+0x10)
        objs.append(dict(num=i+1, vsize=vsize, base=base, flags=flags, pidx=pidx, pcnt=pcnt))
        print(f"  obj{i+1}: base={base:#010x} vsize={vsize:#x} pages[{pidx}..{pidx+pcnt-1}] flags={flags:#x}")

    def page_to_obj_off(logical_page):
        """logical_page (1-based) -> (obj, offset-of-page-start within object)."""
        for ob in objs:
            if ob["pidx"] <= logical_page < ob["pidx"] + ob["pcnt"]:
                return ob, (logical_page - ob["pidx"]) * page_size
        return None, 0

    def linear_of(obj, off):
        return obj["base"] + off if obj else None

    def which_obj(lin):
        for ob in objs:
            if ob["base"] <= lin < ob["base"] + ob["vsize"]:
                return ob, lin - ob["base"]
        return None, None

    # --- fixup page table: n_pages+1 dword offsets into the record table ---
    fpt = [u32(data, fpt_off + 4*i) for i in range(n_pages + 1)]

    want = int(sys.argv[1], 16) if len(sys.argv) > 1 else DEFAULT_SRC
    total = 0
    hits = []
    for p in range(1, n_pages + 1):
        ob, page_off = page_to_obj_off(p)
        rec = frt_off + fpt[p-1]
        end = frt_off + fpt[p]
        while rec < end:
            src_flags = u8(data, rec); rec += 1
            trg_flags = u8(data, rec); rec += 1
            srclist = bool(src_flags & 0x20)
            if srclist:
                cnt = u8(data, rec); rec += 1
                srcoffs = []
            else:
                srcoffs = [s16(data, rec)]; rec += 1 + 1  # signed word (2 bytes)
            # NB: s16 read consumed 2 bytes; fix pointer (above added only 2? adjust)
            # target
            ttype = trg_flags & 0x03
            objn = tgt_off = ordinal = None
            if ttype == 0:  # internal reference
                if trg_flags & 0x40:
                    objn = u16(data, rec); rec += 2
                else:
                    objn = u8(data, rec); rec += 1
                if (src_flags & 0x0f) == 0x02:   # 16-bit selector, no offset
                    tgt_off = 0
                else:
                    if trg_flags & 0x10:
                        tgt_off = u32(data, rec); rec += 4
                    else:
                        tgt_off = u16(data, rec); rec += 2
            elif ttype == 1:  # import by ordinal
                rec += 2 if (trg_flags & 0x40) else 1     # module ordinal
                if trg_flags & 0x80: ordinal = u32(data, rec); rec += 4
                else: ordinal = u16(data, rec); rec += 2
            elif ttype == 2:  # import by name
                rec += 2 if (trg_flags & 0x40) else 1
                rec += 4 if (trg_flags & 0x10) else 2
            elif ttype == 3:  # via entry table
                rec += 2
            if srclist:
                srcoffs = [s16(data, rec + 2*k) for k in range(cnt)]
                rec += 2*cnt
            for so in srcoffs:
                total += 1
                src_lin = linear_of(ob, page_off + so)
                if src_lin is not None and want <= src_lin <= want + 3:
                    tobj = next((x for x in objs if x["num"] == objn), None) if objn else None
                    tgt_lin = (tobj["base"] + tgt_off) if tobj else None
                    hits.append((src_lin, ttype, objn, tgt_off, tgt_lin))

    print(f"\ntotal fixup source references parsed: {total}")
    print(f"\n# Fixups whose SOURCE covers {want:#x} (our accessor's imm32):")
    if not hits:
        print("  (none) -> that constant is NOT relocated; it is a literal value.")
    for src_lin, ttype, objn, tgt_off, tgt_lin in hits:
        print(f"  src={src_lin:#x} type={ttype} -> obj{objn}:+{tgt_off:#x} = linear {tgt_lin:#x}")
        ob, off = which_obj(tgt_lin) if tgt_lin else (None, None)
        if ob:
            print(f"      target lands in obj{ob['num']} at offset {off:#x}")

    # bonus: where does the literal 0x11e3c fall, treated as a linear address?
    ob, off = which_obj(0x11e3c)
    if ob:
        print(f"\nliteral 0x11e3c as a linear address -> obj{ob['num']} offset {off:#x}")

if __name__ == "__main__":
    main()
