#!/usr/bin/env python3
"""buildgame.py -- link the whole decompiled program into a DOS/4GW LE executable,
auto-stubbing whatever is genuinely unresolved. Runs IN-CONTAINER.

Steps:
  1. select one object per real FUN_ source; validate each as OMF, collecting its
     PUBLIC defs + EXTERN refs. Malformed / empty-_TEXT objects are excluded + listed.
  2. missing = refs - defs - data-globals(dataimg). These are symbols no object defines
     (matching-artifact aliases like FUN_..._ / g_..w, sub-0x10000 pseudo-funcs, the few
     genuinely-uncarved callees, and the excluded malformed objects' own names).
  3. author stubs.obj: a ret-only _TEXT body with a PUBDEF for every missing FUN_*, and a
     BSS _DATA with a PUBDEF for every missing g_*. (Stubs make the link COMPLETE; the
     count is the precise remaining gap.)
  4. wlink all valid objects + dataimg.obj + stubs.obj (+ CLIB3S.LIB for C-runtime pulls)
     -> GAME.EXE, entry at the LE header's start.
"""
import os, re, json, glob, shutil, subprocess, struct, sys

W = "/work/toolchain/watcom10a/WATCOM"
WORK = "/tmp/game"


def read_omf(path):
    """return (publics:set, externs:list, has_text:bool) or None if unparseable."""
    try:
        data = open(path, "rb").read()
    except OSError:
        return None
    if not data:
        return None
    def rdidx(b, j):
        return (((b[j] & 0x7f) << 8) | b[j+1], j+2) if b[j] & 0x80 else (b[j], j+1)
    pubs, exts, has_text, names, textseg, segord = set(), [], False, [], None, 0
    i = 0
    try:
        while i + 3 <= len(data):
            rt = data[i]; ln = data[i+1] | (data[i+2] << 8); body = data[i+3:i+3+ln-1]; i += 3+ln
            if rt == 0x96:
                j = 0
                while j < len(body):
                    nl = body[j]; names.append(body[j+1:j+1+nl].decode("latin1")); j += 1+nl
            elif rt in (0x98, 0x99):
                segord += 1
                acbp = body[0]; j = 1
                if (acbp >> 5) & 7 == 0:
                    j += 3
                j += 4 if rt == 0x99 else 2
                ni, j = rdidx(body, j)
                if 1 <= ni <= len(names) and names[ni-1] == "_TEXT":
                    textseg = segord
            elif rt in (0xA0, 0xA1):
                j = 0; segi, j = rdidx(body, j)
                if segi == textseg:
                    off = int.from_bytes(body[j:j+(4 if rt == 0xA1 else 2)], "little")
                    j += 4 if rt == 0xA1 else 2
                    if len(body) - j > 0:
                        has_text = True
            elif rt in (0x90, 0x91):
                j = 0; g, j = rdidx(body, j); s, j = rdidx(body, j)
                if s == 0:
                    j += 2
                while j < len(body):
                    nl = body[j]; nm = body[j+1:j+1+nl].decode("latin1"); j += 1+nl
                    j += 4 if rt == 0x91 else 2
                    _, j = rdidx(body, j)
                    pubs.add(nm)
            elif rt in (0x8c, 0x8d):
                j = 0
                while j < len(body)-1:
                    nl = body[j]; exts.append(body[j+1:j+1+nl].decode("latin1")); j += 1+nl+1
    except (IndexError, UnicodeDecodeError):
        return None
    return pubs, exts, has_text


def idx(v):
    return bytes([v]) if v < 0x80 else bytes([0x80 | (v >> 8), v & 0xff])


def pstr(s):
    b = s.encode("latin1"); return bytes([len(b)]) + b


def omf_rec(rt, body):
    ln = len(body) + 1
    return bytes([rt, ln & 0xff, ln >> 8]) + body + bytes([0])


def make_stubs(func_names, data_names, path, entry_code=None):
    """ret-only _TEXT with a PUBDEF per func; BSS _DATA with a PUBDEF per data sym.
    entry_code: raw bytes placed at _TEXT offset 0 to serve as the program entry (the
    real reconstructed C-runtime startup). If None, a clean-exit stub is used instead."""
    ENTRY = entry_code if entry_code is not None else b"\xB8\x00\x4C\xCD\x21"
    out = bytearray()
    out += omf_rec(0x80, pstr("STUBS"))
    # names: 1='' 2=_TEXT 3=CODE 4=_DATA 5=DATA 6=DGROUP 7=STACK
    out += omf_rec(0x96, pstr("")+pstr("_TEXT")+pstr("CODE")+pstr("_DATA")+pstr("DATA")+pstr("DGROUP")+pstr("STACK"))
    tlen = len(ENTRY) + len(func_names)           # entry routine + one ret byte per stub
    out += omf_rec(0x99, bytes([0x69]) + struct.pack("<I", tlen) + idx(2) + idx(3) + idx(1))  # _TEXT seg#1
    dlen = max(1, 4 * len(data_names))
    out += omf_rec(0x99, bytes([0x69]) + struct.pack("<I", dlen) + idx(4) + idx(5) + idx(1))  # _DATA seg#2
    # STACK seg#3: ACBP A=para(3)<<5 | C=stack(5)<<2 | P=use32(1) = 0x75 ; 64k BSS
    out += omf_rec(0x99, bytes([0x75]) + struct.pack("<I", 0x10000) + idx(7) + idx(7) + idx(1))
    out += omf_rec(0x9A, idx(6) + bytes([0xFF]) + idx(2))   # DGROUP -> _DATA (seg #2)
    # entry routine occupies _TEXT[0..len(ENTRY)); ret-stubs follow after it
    ENTRY_LEN = len(ENTRY)
    ent = bytearray()
    for k, nm in enumerate(func_names):
        ent += pstr(nm) + struct.pack("<I", ENTRY_LEN + k) + idx(0)
        if len(ent) > 900:
            out += omf_rec(0x91, idx(0)+idx(1)+bytes(ent)); ent = bytearray()
    if ent:
        out += omf_rec(0x91, idx(0)+idx(1)+bytes(ent))
    # PUBDEF data in _DATA (seg #2)
    ent = bytearray()
    for k, nm in enumerate(data_names):
        ent += pstr(nm) + struct.pack("<I", 4*k) + idx(0)
        if len(ent) > 900:
            out += omf_rec(0x91, idx(1)+idx(2)+bytes(ent)); ent = bytearray()
    if ent:
        out += omf_rec(0x91, idx(1)+idx(2)+bytes(ent))
    # LEDATA _TEXT: ENTRY routine at offset 0, then ret (0xC3) stubs. STUBS.OBJ is linked
    # FIRST, so this entry sits at _TEXT offset 0 == LE object1:0 == DOS/4GW's default start.
    text = ENTRY + (b"\xC3" * len(func_names))
    # LEDATA payload capped at 1024 bytes/record
    off = 0
    while off < len(text):
        chunk = text[off:off+1024]
        out += omf_rec(0xA1, idx(1) + struct.pack("<I", off) + chunk)
        off += len(chunk)
    out += omf_rec(0x8B, bytes([0x00]))          # plain MODEND (default entry used)
    open(path, "wb").write(out)


def main():
    rec = json.load(open("manifest/recipes.json"))
    # a function source is any src/**/*.c whose basename is a manifest function name (FUN_<addr> OR a
    # semantic name after tools/apply_names.py). name_of maps addr -> current name for the stubs below.
    man_fns = json.load(open("manifest/functions.json"))["functions"]
    man_names = {f["name"] for f in man_fns}
    name_of = {f["addr"]: f["name"] for f in man_fns}
    addr_of = {f["name"]: f["addr"] for f in man_fns}   # short OBJ names key on addr (names may be semantic)
    names = sorted(os.path.basename(p)[:-2] for p in glob.glob("src/**/*.c", recursive=True)
                   if os.path.basename(p)[:-2] in man_names)
    DEFAULT = "-4s -oneatx -zp8 -s -zq"
    os.makedirs(WORK, exist_ok=True)
    for f in glob.glob(WORK + "/*"):
        os.remove(f)

    # --clibstart: compile a real `main` (returns 42 so DOS errorlevel proves it executed) that
    # the genuine Watcom cstart will call once DOS/4GW accepts the image.
    if "--clibstart" in sys.argv:
        # main() hands off to the game's real main FUN_00024be8(argc,argv). This uses CLIB's
        # proven startup (past 1012, reaches main) and BYPASSES the game's own __x386_start, whose
        # DOS/4GW handoff our hand-built single-blob LE (GAMEO) mishandles.
        GAMEMAIN = name_of.get("00024be8", "FUN_00024be8")   # follows apply_names.py rename
        INT386 = name_of.get("0003adb2", "FUN_0003adb2")
        if "--diagmain" in sys.argv:
            body = ("extern int printf(const char*,...);\nextern int fflush(void*);\n"
                    "extern void %s(int,void*,void*);\n" % INT386 +
                    "int main(int argc,char**argv){\n"
                    "  int buf[16];\n"
                    "  printf(\"M1\\n\"); fflush(0);\n"
                    "  ((unsigned char*)buf)[1]=0xf; %s(0x10,buf,buf);\n" % INT386 +
                    "  printf(\"M2-int386-ok mode=%d\\n\", ((unsigned char*)buf)[0]); fflush(0);\n"
                    "  return 7;\n}\n")
        else:
            body = ("int main(void){return 42;}\n" if "--gamemain" not in sys.argv else
                    "extern void %s(int,char**);\n" % GAMEMAIN +
                    "int main(int argc,char**argv){%s(argc,argv);return 0;}\n" % GAMEMAIN)
        open("src/_mainstub.c", "w").write(body)
        subprocess.run(["bash", "tools/wcc_95.sh", "_mainstub", "-4s -zq"], capture_output=True, text=True)
        if os.path.exists("build/_mainstub.obj"):
            shutil.copy("build/_mainstub.obj", os.path.join(WORK, "MAIN.OBJ"))
        os.remove("src/_mainstub.c")

    defs, refs = set(), set()
    objlist, bad, compiled, missing_src = [], [], 0, []
    for nm in names:
        obj = "build/%s.obj" % nm
        if not os.path.exists(obj):
            flags = rec.get(nm, {}).get("flags", DEFAULT)
            subprocess.run(["bash", "tools/wcc_95.sh", nm, flags], capture_output=True, text=True)
            if not os.path.exists(obj):
                missing_src.append(nm); continue
        r = read_omf(obj)
        if r is None or not r[2]:            # unparseable or empty _TEXT
            bad.append(nm); continue
        pubs, exts, _ = r
        defs |= pubs
        refs |= set(exts)
        short = "F%X.OBJ" % (int(addr_of[nm], 16) & 0xFFFFFF)   # unique per addr; handles semantic names
        shutil.copy(obj, os.path.join(WORK, short))
        objlist.append(short)

    # data globals defined by dataimg.obj
    dr = read_omf("build/dataimg.obj")
    data_defs = dr[0] if dr else set()
    shutil.copy("build/dataimg.obj", os.path.join(WORK, "DATA.OBJ"))

    # the recovered object1 code PREFIX (tools/prefix_obj.py): one relink-safe object holding the
    # game's MAIN LOOP (FUN_0000d928) + startup fns the shifted linear.bin dropped. Its PUBDEFs
    # satisfy the FUN_0000xxxx references the main body makes, so they stop being ret-stubs.
    if os.path.exists("build/prefix.obj"):
        pr = read_omf("build/prefix.obj")
        if pr:
            defs |= pr[0]; refs |= set(pr[1])
        shutil.copy("build/prefix.obj", os.path.join(WORK, "PREFIX.OBJ"))
        objlist.append("PREFIX.OBJ")
        print("linked build/prefix.obj (%d PUBDEFs)" % (len(pr[0]) if pr else 0))

    # everything referenced but not defined by our objects or the data image.
    # CLIB symbols (leading underscore runtime helpers) are left for CLIB3S.LIB to satisfy.
    unresolved = refs - defs - data_defs
    # Watcom register vs stack calling convention decorate a CODE symbol differently: the
    # register convention (-4r/-3r) appends a TRAILING underscore, the stack convention (-4s/-3s)
    # does not. So the SAME function `foo` can be referenced as `foo` by a `-s` caller and as
    # `foo_` by a `-r` caller, while its defining object publishes only one of the two names.
    # Bridge either mismatch with a wlink ALIAS (both directions) instead of mis-stubbing the ref
    # as BSS data -- a jmp/call into zeroed BSS crashes rather than degrades. Covers the sound
    # dispatch cluster (`sound_dispatch_trampoline_` from the -4r stubs; `snd_cmd_65`.. clean names
    # referenced by the -4s driver table while the -4r stubs define `snd_cmd_65_`..) and the
    # sibling `fill_bytes_`/`lcg_rand_`/... alias class.
    alias_pairs = []
    for s in unresolved:
        if s.endswith("_") and s[:-1] in defs:          # ref foo_ , def foo
            alias_pairs.append((s, s[:-1]))
        elif not s.endswith("_") and (s + "_") in defs:  # ref foo , def foo_
            alias_pairs.append((s, s + "_"))
    # ADDRESS aliases: a ref like FUN_<addr> / FUN_LE_<addr> / fn_<addr> names a function by its
    # original address, but that function is BUILT under a semantic name. If a function starts at
    # <addr> and its semantic name is defined, alias the address-name onto it. (Mid-function offsets
    # -- e.g. ISR handlers -- have no name_of[addr] and stay stubbed.)
    import re as _re
    addr_alias = []
    for s in sorted(unresolved):
        m = _re.match(r"(?:FUN_LE_|FUN_|fn_)0*([0-9a-fA-F]+)_?$", s)
        if not m:
            continue
        key = "%08x" % int(m.group(1), 16)
        tgt = name_of.get(key)
        if tgt and tgt in defs and tgt != s:
            addr_alias.append((s, tgt))
    alias_pairs = sorted(set(alias_pairs) | set(addr_alias))
    unresolved = unresolved - {a for a, _ in alias_pairs}
    print("address aliases (FUN_<addr> -> built name): %d" % len(addr_alias))
    # classify EVERY unresolved symbol: code if a call-target name (FUN_/fn_), else data.
    # nearly all are matching-artifact aliases (fn_/tbl_/pool_/_g_/suffixed g_) the reloc-
    # masking workflow introduced; a few are sub-0x10000 pseudo-funcs + the 4 excluded objs.
    stub_funcs = sorted(s for s in unresolved if s.startswith("FUN_") or s.startswith("fn_"))
    stub_data  = sorted(unresolved - set(stub_funcs))
    clib_like  = []
    # Wire the REAL entry: the LE header's start is linear 0x3d85c (C-runtime startup), which
    # lives in an un-carved gap [0x3d85c, 0x3dae1). Drop those raw bytes in as the entry so the
    # program jumps into the actual reconstructed startup instead of a clean-exit stub.
    entry_code = None
    if "--realentry" in sys.argv:
        seg = open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "rb").read()
        entry_code = seg[0x3d85c - 0x10000 : 0x3dae1 - 0x10000]
        print("real entry: %d startup bytes @0x3d85c wired at _TEXT:0" % len(entry_code))
    make_stubs(stub_funcs, stub_data, os.path.join(WORK, "STUBS.OBJ"), entry_code)

    realstart = "--clibstart" in sys.argv
    lines = ["system begin myd4g", "  option osname=DOS/4G",
             "  op stub=C:\\BINB\\WSTUB.EXE", "  format os2 le", "end",
             "system myd4g", "option quiet", "option stack=64k",
             "libpath C:\\LIB386", "libpath C:\\LIB386\\DOS",
             "name D:\\GAME.EXE", "option map=D:\\GAME.MAP"]
    for a, tgt in alias_pairs:                                # register-convention name -> clean def
        lines.append("alias %s=%s" % (a, tgt))
    if realstart:
        # link the GENUINE Watcom startup: wlink auto-pulls CSTRTX3S (which owns the LE entry
        # __x386_start that DOS/4GW's loader requires) + CLIB from the library; MAIN.OBJ supplies
        # the `main` cstart calls. Tests whether the real startup gets us PAST the 1012 loader check.
        lines.append("file D:\\MAIN.OBJ")
    lines.append("file D:\\STUBS.OBJ")
    for o in objlist:
        lines.append("file D:\\%s" % o)
    lines.append("file D:\\DATA.OBJ")                        # data object last
    lines.append("library C:\\LIB386\\DOS\\CLIB3S.LIB")
    open(os.path.join(WORK, "T.LNK"), "w", newline="\r\n").write("\n".join(lines)+"\n")
    open(os.path.join(WORK, "GO.BAT"), "w", newline="\r\n").write(
        "C:\\BIN\\WLINK.EXE @D:\\T.LNK > D:\\LINK.TXT\n")
    open(os.path.join(WORK, "dbx.conf"), "w").write(
        "[cpu]\ncore=auto\ncycles=max\n[autoexec]\nmount c %s\nmount d %s\n"
        "set PATH=Z:\\\nd:\ncall D:\\GO.BAT\nexit\n" % (W, WORK))

    print("valid objects : %d" % len(objlist))
    print("malformed/empty (excluded): %d %s" % (len(bad), bad if len(bad) <= 12 else bad[:12]+["..."]))
    print("missing src   : %d %s" % (len(missing_src), missing_src))
    print("reg-conv aliases resolved: %d %s" % (len(alias_pairs), [a for a, _ in alias_pairs][:20]))
    print("stub FUNCS    : %d" % len(stub_funcs))
    print("stub DATA     : %d %s" % (len(stub_data), stub_data))
    print("other unresolved (left to CLIB): %d %s" % (len(clib_like), clib_like[:20]))

    env = dict(os.environ, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy")
    subprocess.run(["timeout", "300", "dosbox", "-conf", os.path.join(WORK, "dbx.conf")],
                   env=env, capture_output=True)
    log = os.path.join(WORK, "LINK.TXT")
    txt = open(log, encoding="latin1").read() if os.path.exists(log) else "(no log)"
    tail = [l for l in txt.splitlines() if l.strip()]
    print("=== LINK LOG (last 25 lines) ===")
    print("\n".join(tail[-25:]))
    exe = os.path.join(WORK, "GAME.EXE")
    if os.path.exists(exe):
        shutil.copy(exe, "build/GAME.EXE")          # persist to the mount
        print("=== GAME.EXE: %d bytes (-> build/GAME.EXE) ===" % os.path.getsize(exe))
    else:
        print("=== GAME.EXE: NOT PRODUCED ===")


if __name__ == "__main__":
    main()
