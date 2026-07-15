#!/usr/bin/env python3
"""mkdata.py -- author an OMF .obj that DEFINES every data global (g_XXXX) the
decompiled sources reference, so the whole program can be linked.

The decompiled functions reference globals SYMBOLICALLY (extern g_810e[] -> EXTDEF
"g_810e" + a fixup). Nothing defines those symbols -- the matching workflow only ever
compared code, never the data. This emits ONE OMF object with:
  * a single _DATA segment (in DGROUP) sized to cover the highest global,
  * the original data bytes from OBJECT2/OBJECT4 .linear.bin placed at their offsets,
  * a PUBDEF for every g_XXXX at offset = its address.

Because our code is symbolic, the linker relocates the whole blob together; the exact
absolute address no longer matters for LINKING (only the relative layout inside this
segment matters for faithful RUNNING, which is best-effort here -- see notes).

Layout (DGROUP offset == global address, the address baked into the original code):
  [0x00000, 0x0d000)  OBJECT2.linear.bin        (initialised data, 53248 B)
  [0x0d000, 0x14a60)  implicit zero             (OBJ2 BSS tail + OBJ3 BSS 0xc00)
  [0x14a60, 0x30a60)  OBJECT4.linear.bin        (initialised data, 114688 B)
  [0x30a60, seglen)   implicit zero             (OBJ4 BSS tail + 2 outlier globals)
seglen is stretched to cover the two far-outlier globals (g_5340b ptr, g_108110 table)
as zero-filled BSS -- they resolve, values best-effort.
"""
import re, glob, struct, sys, json

OBJ2 = "inputs/SYNDICAT_MAIN_OBJECT2.linear.bin"
OBJ4 = "inputs/SYNDICAT_MAIN_OBJECT4.linear.bin"
OUT  = "build/dataimg.obj"
# Contiguous DGROUP packing (OBJ2 | OBJ3 | OBJ4) -- matches the code's baked data displacements
# (which run 0..0x31092 with no gaps). OBJ4 begins right after OBJ2(0x13e60)+OBJ3(0xc00).
OBJ4_BASE = 0x14a60


def collect_globals():
    """Every g_* referenced in src -> its data-image address. Resolution is REGISTRY-FIRST:
    a semantic name (g_screen_buf) gets its address from manifest/globals.json; a bare g_<hex>
    name gets it from the hex. So globals can be renamed for readability without breaking the
    data image (the address is the anchor, not the name -- mirrors names.json for functions)."""
    reg = {}
    try:
        reg = json.load(open("manifest/globals.json"))
    except (FileNotFoundError, ValueError):
        pass
    name2addr = {}
    for p in glob.glob("src/**/*.c", recursive=True):
        txt = open(p, encoding="utf-8", errors="replace").read()
        for tok in re.findall(r"\bg_[0-9A-Za-z_]+\b", txt):
            if tok in name2addr:
                continue
            if tok in reg:                                   # renamed global: addr from registry
                name2addr[tok] = int(reg[tok]["addr"], 16)
            else:
                m = re.fullmatch(r"g_([0-9a-fA-F]+)", tok)   # bare g_<hex>: addr from the hex
                if m:
                    name2addr[tok] = int(m.group(1), 16)
                # else: unknown semantic name not in registry -> skip (would be link-stubbed)
    return name2addr


def rec(rtype, body):
    ln = len(body) + 1                       # +1 for checksum byte
    assert ln <= 0xffff, "record too long"
    return bytes([rtype, ln & 0xff, ln >> 8]) + body + bytes([0])   # checksum 0 = ignore


def idx(v):
    return bytes([v]) if v < 0x80 else bytes([0x80 | (v >> 8), v & 0xff])


def pstr(s):
    b = s.encode("latin1")
    return bytes([len(b)]) + b


def main():
    reloc = "--reloc-ptrs" in sys.argv
    globs = collect_globals()                # name -> addr
    addrs = sorted(set(globs.values()))
    maxa = addrs[-1]
    seglen = maxa + 0x1000                    # room for the highest global's object
    d2 = bytearray(open(OBJ2, "rb").read())
    d4 = bytearray(open(OBJ4, "rb").read())

    # --reloc-ptrs: DATA-side relocation recovery. OBJECT2/4 hold function POINTERS baked to the
    # ORIGINAL code addresses; when the program is RELINKED (--clibstart) our functions move, so
    # indirect calls through them misdispatch (the game's main loop is reached via one: [0xd928]).
    # Find every dword-aligned value that exactly matches a function START, zero it, and emit an OMF
    # FIXUPP -> that FUN_ symbol so wlink relocates it to our address.
    fn_starts = set()
    if reloc:
        man = json.load(open("manifest/functions.json"))
        fn_starts = {int(f["addr"], 16) for f in man["functions"] if 0x10000 <= int(f["addr"], 16) <= 0x4fdf4}
    ptr_syms = []                             # ordered unique FUN_ names -> EXTDEF index
    def collect(data, seg_base):
        sites = []
        if reloc:
            SHIFT = 0x28b8   # data ptrs hold RUNTIME code addrs; manifest = runtime - 0x28b8
            for i in range(0, len(data) - 3, 4):
                v = struct.unpack_from("<I", data, i)[0]
                m = v - SHIFT
                if m in fn_starts:
                    nm = "FUN_%08x" % m
                    if nm not in ptr_syms:
                        ptr_syms.append(nm)
                    struct.pack_into("<I", data, i, 0)   # zero; linker fills via the fixup
                    sites.append((seg_base + i, nm))
        return dict(sites)
    sites2 = collect(d2, 0x0)
    sites4 = collect(d4, OBJ4_BASE)
    print("globals=%d  max=0x%x  seglen=0x%x (%d KB)  reloc-ptrs=%d"
          % (len(addrs), maxa, seglen, seglen // 1024, len(sites2) + len(sites4)))

    out = bytearray()
    out += rec(0x80, pstr("DATAIMG"))
    out += rec(0x96, pstr("") + pstr("_DATA") + pstr("DATA") + pstr("DGROUP"))
    out += rec(0x99, bytes([0x69]) + struct.pack("<I", seglen) + idx(2) + idx(3) + idx(1))
    out += rec(0x9A, idx(4) + bytes([0xFF]) + idx(1))
    # EXTDEF (0x8C): the function symbols the data pointers reference
    eb = bytearray()
    for nm in ptr_syms:
        e = pstr(nm) + idx(0)
        if len(eb) + len(e) > 1000:
            out += rec(0x8C, bytes(eb)); eb = bytearray()
        eb += e
    if eb:
        out += rec(0x8C, bytes(eb))

    ent = bytearray()
    def flush_pub():
        nonlocal ent
        if ent:
            out.extend(rec(0x91, idx(1) + idx(1) + bytes(ent)))
            ent = bytearray()
    for name, a in sorted(globs.items(), key=lambda kv: kv[1]):
        e = pstr(name) + struct.pack("<I", a) + idx(0)   # PUBDEF uses the SOURCE name (semantic or hex)
        if len(ent) + len(e) > 1000:
            flush_pub()
        ent += e
    flush_pub()

    # LEDATA32 + FIXUPP for any pointer sites in each chunk (chunks are dword-aligned so 4-byte
    # pointers never split across a record boundary).
    def emit_ledata(base, data, sites):
        off = 0
        while off < len(data):
            chunk = bytes(data[off:off + 1024]); clen = len(chunk)
            out.extend(rec(0xA1, idx(1) + struct.pack("<I", base + off) + chunk))
            fb = bytearray()
            for a, nm in sites.items():
                if base + off <= a < base + off + clen:
                    doff = a - (base + off)
                    locat = 0x8000 | (1 << 14) | (9 << 10) | (doff & 0x3FF)   # M=1 abs, LOC=9 32-bit
                    fb += bytes([locat >> 8, locat & 0xFF])
                    fb += bytes([0x56]) + idx(ptr_syms.index(nm) + 1)          # frame=target,ext,noP
            if fb:
                out.extend(rec(0x9D, bytes(fb)))
            off += clen
    emit_ledata(0x0, d2, sites2)
    emit_ledata(OBJ4_BASE, d4, sites4)

    out += rec(0x8B, bytes([0x00]))
    open(OUT, "wb").write(out)
    print("wrote %s (%d bytes, %d PUBDEF names)" % (OUT, len(out), len(addrs)))


if __name__ == "__main__":
    main()
