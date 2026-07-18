#!/usr/bin/env python3
"""origbuild.py -- build a runnable, RELOCATABLE LE at ORIGINAL addresses. Runs IN-CONTAINER.

The image places object1 code at 0x10000 and the data (OBJECT2|OBJ3-BSS|OBJECT4, contiguous DGROUP)
after it, entry at the genuine __x386_start (0x3d85c). CRUCIALLY it reconstructs the LE FIXUP TABLE:
DOS/4GW loads the LE at a DPMI-chosen base (observed 0xf0000, NOT the 0x10000 reloc_base), so without
fixups every baked ABSOLUTE reference (fn-pointer tables, cross-object data addrs) points at the wrong
place -> the game jumps through a garbage pointer (0xcccccccc) and dies (R6003 divide-by-0 in the fault
handler). The original ships 8688+1372 = 10060 fixups; tools/lefix.py parses them. We re-emit them as
OMF FIXUPP records so wlink writes a proper LE fixup table and the loader relocates correctly.

Fixup mapping (lefix gives src in manifest space, target as (obj,off)):
  runtime_src = src_lin + 0x28b8
    code source (obj1)  : _TEXT offset = runtime_src - 0x10000
    data source (obj2)  : _DATA offset = runtime_src - 0x50000
    data source (obj4)  : _DATA offset = 0x14a60 + (runtime_src - 0x80000)
  target -> _TEXT (tobj1, disp=toff) or DGROUP data (tobj2 disp=toff / tobj3 disp=0x13e60+toff /
                                                     tobj4 disp=0x14a60+toff)
  Cross-object targets go via EXTDEF __txbase / __dgbase (defined PUBDEF in the owning object at :0).

  python3 tools/origbuild.py    # -> build/GAMEO.EXE
"""
import struct, os, shutil, subprocess, sys
import importlib.util

SEG1 = os.environ.get("ORIGBUILD_SEG1", "build/obj1_full.bin")
EXE  = "inputs/SYNDICAT_MAIN.EXE"     # the genuine LE -- source of the FULL DGROUP data pages
CODE_BASE = 0x10000
ENTRY = 0x3d85c
SHIFT = 0x28b8
# DGROUP disp offsets (the code's DGROUP coordinate): OBJ2 at 0, OBJ3 at 0x13e60, OBJ4 at 0x14a60.
# fixup toff IS the code's DGROUP offset (VERIFIED: g_3568 fixup toff=0x3568 = its DGROUP offset).
OBJ3_DG = 0x13e60
OBJ4_DG = 0x14a60
# DATA PLACEMENT: buf is indexed in 0-based DGROUP coords, so buf[dgroup_off] = DGROUP[dgroup_off].
# We reconstruct OBJECT2/OBJECT4 from the LE DATA PAGES of SYNDICAT_MAIN.EXE (le_object_data, same
# method as linearize.py for object1) -- this yields their FULL images from offset 0. OBJECT2 goes at
# buf[0], OBJECT4 at buf[OBJ4_DG]; OBJECT3 [0x13e60,0x14a60) is BSS (zero).
# The old build used the *.linear.bin extracts, which are each missing their first 0x28b8 bytes (the
# e_lfanew stub cut), and placed them at buf[0x28b8] -- so DGROUP [0,0x28b8) was left ZERO. That region
# holds real initialised data (e.g. the container magic "LX" at 0xb0, the "data/*.dat" name table, the
# RTL error strings): with it zeroed, container_total_size's magic strcmp failed -> sound/resource init
# aborted. Using the full LE pages recovers it. (VERIFIED: full_obj2[0x28b8:] == the old linear.bin at
# 0 mismatches, so nothing above 0x28b8 changes; full_obj2[0xb0] = "LX\0\0not enough memory...".)
W = "/work/toolchain/watcom10a/WATCOM"
WORK = "/tmp/gameo"

idx  = lambda v: bytes([v]) if v < 0x80 else bytes([0x80 | (v >> 8), v & 0xff])
pstr = lambda s: bytes([len(s)]) + s.encode("latin1")
def rec(rt, body):
    ln = len(body) + 1
    return bytes([rt, ln & 0xff, ln >> 8]) + body + bytes([0])


def load_fixups():
    spec = importlib.util.spec_from_file_location("lefix", "tools/lefix.py")
    lefix = importlib.util.module_from_spec(spec); spec.loader.exec_module(lefix)
    _, objs, fixups = lefix.parse()
    code_fx, data_fx = [], []      # (src_off, target_kind, disp)   target_kind: 'T'=_TEXT 'D'=DGROUP
    for src_lin, stype, ttype, tobj, toff, add, pg in fixups:
        if ttype != 0 or toff is None:
            continue
        rt = src_lin + SHIFT
        # target = code DGROUP offset (toff already IS the DGROUP offset for the low object)
        if tobj == 1:               tk, disp = 'T', toff
        elif tobj == 2:             tk, disp = 'D', toff
        elif tobj == 3:             tk, disp = 'D', OBJ3_DG + toff
        elif tobj == 4:             tk, disp = 'D', OBJ4_DG + toff
        else:                       continue
        # source location: code -> _TEXT offset; data -> its code-DGROUP offset (buf is placed so this
        # resolves to the right shifted byte, matching where the target data lives)
        if 0x10000 <= rt < 0x4fdf4:
            code_fx.append((rt - 0x10000, tk, disp))
        elif 0x50000 <= rt < 0x63e60:
            data_fx.append((rt - 0x50000, tk, disp))
        elif 0x80000 <= rt < 0x9c632:
            data_fx.append((OBJ4_DG + (rt - 0x80000), tk, disp))
    return code_fx, data_fx


def emit_obj(name, seg_bytes, fixups, own_seg, own_name, extern_name, entry_off=None):
    """One object: SEGDEF own_seg(idx1) in DGROUP, PUBDEF <own_name>base@:0, EXTDEF <extern_name>base,
    LEDATA seg bytes with the fixup sources zeroed, FIXUPP per chunk. own_seg 'T' -> _TEXT/CODE,
    'D' -> _DATA/DATA. Code targets -> SEGDEF _TEXT if own is T else EXTDEF __txbase; data targets ->
    SEGDEF _DATA if own is D else EXTDEF __dgbase."""
    data = bytearray(seg_bytes)
    for off, tk, disp in fixups:
        data[off:off+4] = b"\x00\x00\x00\x00"
    seg_cls = ("_TEXT", "CODE") if own_seg == 'T' else ("_DATA", "DATA")
    out = bytearray()
    out += rec(0x80, pstr(name))
    out += rec(0x96, pstr("") + pstr(seg_cls[0]) + pstr(seg_cls[1]) + pstr("DGROUP"))
    out += rec(0x99, bytes([0x69]) + struct.pack("<I", len(data)) + idx(2) + idx(3) + idx(1))  # SEGDEF idx1
    if own_seg == 'D':
        out += rec(0x9A, idx(4) + bytes([0xFF]) + idx(1))                                       # GRPDEF DGROUP -> _DATA
        grp = 1
    else:
        grp = 0                                                                                 # _TEXT: no group
    # PUBDEF (0x91 = 32-bit): base symbol at seg:0
    out += rec(0x91, idx(grp) + idx(1) + pstr(own_name) + struct.pack("<I", 0) + idx(0))
    out += rec(0x8C, pstr(extern_name) + idx(0))                                                # EXTDEF idx1
    if entry_off is None:
        entry_rec = rec(0x8B, bytes([0x00]))
    else:
        entry_rec = rec(0x8B, bytes([0xC1, 0x50]) + idx(1) + struct.pack("<I", entry_off))
    # LEDATA + FIXUPP. Chunks are up to 1024 bytes, but a chunk boundary must NEVER bisect a fixup's
    # 4-byte source: a FIXUPP LOCAT can only address bytes inside its own LEDATA record, so a 32-bit
    # fixup straddling the boundary (doff 0x3fd..0x3ff) gets only its low byte relocated and its target
    # offset is silently corrupted (e.g. 0xbbf4 -> 0xf4). That was the sound-callback dispatch crash:
    # `CALL [ESI+0xbbf4]` relocated to DGROUP+0xf4 (a "%s" format string) -> wild jump with sound on.
    # So end each chunk at the start of any fixup that would otherwise straddle its tail; the fixup then
    # sits at offset 0 of the next chunk, fully contained. (27 code fixups were affected.)
    fix_at = {off: (tk, disp) for off, tk, disp in fixups}
    fixoffs = sorted(fix_at)
    o = 0
    while o < len(data):
        end = min(o + 1024, len(data))
        while True:                                     # pull `end` back past any straddling fixup
            crossers = [X for X in fixoffs if o < X < end and X + 4 > end]
            if not crossers:
                break
            end = min(crossers)
        out += rec(0xA1, idx(1) + struct.pack("<I", o) + bytes(data[o:end]))
        fb = bytearray()
        for off in fixoffs:
            if off < o or off + 4 > end:                # only fixups whose full 4 bytes lie in [o,end)
                continue
            tk, disp = fix_at[off]
            locat = 0x8000 | (1 << 14) | (9 << 10) | ((off - o) & 0x3FF)  # M=1 abs, LOC=9 32-bit off
            fb += bytes([locat >> 8, locat & 0xFF])
            if tk == own_seg:
                # same-object target: frame=SEGDEF(1), target=SEGDEF(1) + disp
                fb += bytes([0x00]) + idx(1) + idx(1) + struct.pack("<I", disp)
            else:
                # cross-object target: frame=target(m5), target=EXTDEF(1) + disp
                fb += bytes([0x52]) + idx(1) + struct.pack("<I", disp)
        if fb:
            out += rec(0x9D, bytes(fb))
        o = end
    out += entry_rec
    open(os.path.join(WORK, name + ".OBJ"), "wb").write(out)
    return len(data)


def le_object_data(idx0):
    """Full initialised-data image of LE object idx0 (0-based) from SYNDICAT_MAIN.EXE's DATA PAGES.
    Same mechanism linearize.py uses for object1: pages are stored SEQUENTIALLY from the header's
    Data-Pages-Offset (H+0x80). Returns (bytes, vsize). Unlike the *.linear.bin extracts, this
    includes the object from offset 0 (no 0x28b8 stub cut)."""
    d = open(EXE, "rb").read()
    H = struct.unpack_from("<I", d, 0x3c)[0]
    assert d[H:H+2] in (b"LE", b"LX"), "no LE/LX header"
    u32 = lambda o: struct.unpack_from("<I", d, H+o)[0]
    page_size = u32(0x28); objtab = u32(0x40) + H; data_pages_off = u32(0x80)
    vsize, base, flags, pidx, pcnt = struct.unpack_from("<IIIII", d, objtab + idx0*24)
    start = data_pages_off + (pidx - 1) * page_size
    return bytearray(d[start:start + pcnt * page_size]), vsize


def make_data_bytes():
    # DGROUP must cover EVERY global, incl. far BSS outliers (~0x108110, e.g. the C heap control
    # block), or refs past the initialised data read garbage -> malloc fails -> game aborts early.
    # Match mkdata: seglen = highest g_ referenced in src + 0x1000.
    import re, glob as _g
    maxa = 0
    for p in _g.glob("src/**/*.c", recursive=True):
        for m in re.findall(r"\bg_([0-9a-fA-F]+)\b", open(p, encoding="utf-8", errors="replace").read()):
            maxa = max(maxa, int(m, 16))
    d2, _ = le_object_data(1)          # OBJECT2 -> DGROUP offset 0    (FULL, incl. [0,0x28b8) prefix)
    d4, vs4 = le_object_data(3)        # OBJECT4 -> DGROUP offset OBJ4_DG
    # The TRUE DGROUP vsize (LE-defined) = OBJ4_DG + obj4 vsize. Far "globals" like g_108110 are actually
    # the C-runtime HEAP control block (~0xd7000 PAST DGROUP), NOT DGROUP members -- covering them in
    # DGROUP over-sizes it by ~0x100000 and pushes the big-block heap arena (and the sound driver) up,
    # which crashes the driver at its wrong load address (heap must match the original layout). Setting
    # ORIGBUILD_TIGHT_DGROUP sizes DGROUP to the LE vsize so the heap lands like MAIN's.
    dgvsize = OBJ4_DG + vs4            # = 0x14a60 + 0x1c632 = 0x31092 (the genuine DGROUP size)
    seglen = dgvsize if os.environ.get("ORIGBUILD_TIGHT_DGROUP") else max(OBJ4_DG + len(d4) + 0x2000, maxa + 0x1000)
    buf = bytearray(seglen)
    buf[0:len(d2)] = d2                        # OBJECT2 at true DGROUP 0
    buf[OBJ4_DG:OBJ4_DG+len(d4)] = d4          # OBJECT4 at true DGROUP 0x14a60 (OBJECT3 BSS between -> zero)
    print("data seglen=0x%x (max global 0x%x); OBJ2@0x0 (%d B) OBJ4@0x%x (%d B); DGROUP prefix recovered"
          % (seglen, maxa, len(d2), OBJ4_DG, len(d4)))
    return bytes(buf)


def main():
    for p in (SEG1,):
        if not os.path.exists(p):
            raise SystemExit("missing %s -- run tools/linearize.py first" % p)
    os.makedirs(WORK, exist_ok=True)
    for f in os.listdir(WORK):
        os.remove(os.path.join(WORK, f))
    code_fx, data_fx = load_fixups()
    cap = int(os.environ.get("FXCAP", "0"))
    if cap:
        code_fx, data_fx = code_fx[:cap], data_fx[:cap]
    print("fixups: code=%d data=%d" % (len(code_fx), len(data_fx)))
    code = open(SEG1, "rb").read()
    nT = emit_obj("CODE", code, code_fx, 'T', "__txbase", "__dgbase", entry_off=ENTRY - CODE_BASE)
    nD = emit_obj("DATA", make_data_bytes(), data_fx, 'D', "__dgbase", "__txbase")
    # stack object, linked last
    st = bytearray(); st += rec(0x80, pstr("STK")); st += rec(0x96, pstr("") + pstr("STACK"))
    st += rec(0x99, bytes([0x75]) + struct.pack("<I", 0x10000) + idx(2) + idx(2) + idx(1)); st += rec(0x8B, bytes([0x00]))
    open(os.path.join(WORK, "STK.OBJ"), "wb").write(st)
    print("code _TEXT=%d B, data _DATA=%d B" % (nT, nD))

    # Link with the NATIVE Open Watcom v2 wlink (no DOSBox -> no 10060-fixup memory fault).
    OW = "/opt/watcom"
    exe = os.path.join(WORK, "GAMEO.EXE")
    # Custom DOS/4G system (NOT `system dos4g`, which links OW's OWN startup + OVERRIDES our entry with
    # it -> OW cstart runs, never calls the game main, exits). This keeps our MODEND entry = the game's
    # real __x386_start (0x3d85c). Native wlink handles the 10060-fixup volume DOSBox wlink page-faults on.
    lines = ["system begin myd4g", "  option osname=DOS/4G",
             "  op stub=" + OW + "/binw/wstub.exe", "  format os2 le", "end", "system myd4g",
             "option quiet", "option stack=64k", "name " + exe,
             "file " + os.path.join(WORK, "CODE.OBJ"),
             "file " + os.path.join(WORK, "DATA.OBJ"),
             "file " + os.path.join(WORK, "STK.OBJ")]
    lnk = os.path.join(WORK, "t.lnk")
    open(lnk, "w").write("\n".join(lines) + "\n")
    env = dict(os.environ, WATCOM=OW, PATH=OW + "/binl64:" + os.environ.get("PATH", ""))
    r = subprocess.run([OW + "/binl64/wlink", "@" + lnk], env=env, capture_output=True, text=True)
    out_txt = (r.stdout + r.stderr).strip()
    if out_txt:
        print("=== LINK LOG ===\n" + "\n".join(out_txt.splitlines()[-15:]))
    if os.path.exists(exe):
        shutil.copy(exe, "build/GAMEO.EXE"); print("=== GAMEO.EXE: %d bytes ===" % os.path.getsize(exe))
    else:
        print("=== GAMEO.EXE: NOT PRODUCED ===")


if __name__ == "__main__":
    main()
