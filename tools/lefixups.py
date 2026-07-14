#!/usr/bin/env python3
"""lefixups.py -- parse the LE fixup (relocation) table of SYNDICAT_MAIN.LE to recover the
GROUND TRUTH memory layout the original code assumes: for every fixup in the code object,
the (target object, target offset) it points at.

WHY: our decompiled code references data globals by baked absolute displacement (g_810e ->
`[0x810e]`). To relink at original addresses (so the db-transcribed library half's baked
call/data displacements resolve), we must know EXACTLY which data object each reference lands
in and at what offset. The fixup table is authoritative; address arithmetic alone was
ambiguous (contiguous-DGROUP vs per-object-base vs offset-within-OBJ4).

Output: per-target-object histogram of referenced offsets (min/max/count) + the code-internal
call/jmp fixup span. That pins the data segmentation.
"""
import struct, collections

f = open("inputs/SYNDICAT_MAIN.LE", "rb").read()
u32 = lambda o: struct.unpack_from("<I", f, o)[0]
u16 = lambda o: struct.unpack_from("<H", f, o)[0]

pagesize   = u32(0x28)
num_pages  = u32(0x14)
objtab     = u32(0x40)
numobj     = u32(0x44)
pagemapoff = u32(0x48)
fixpageoff = u32(0x68)
fixrecoff  = u32(0x6c)
print("pagesize=0x%x num_pages=%d numobj=%d" % (pagesize, num_pages, numobj))
print("objtab@0x%x pagemap@0x%x fixpage@0x%x fixrec@0x%x" % (objtab, pagemapoff, fixpageoff, fixrecoff))

# object table: base, vsize, flags, first-page-index(1-based), page-count
objs = []
for i in range(numobj):
    e = objtab + i*24
    vsize, base, flags, firstpage, pagecnt = struct.unpack_from("<IIIII", f, e)
    objs.append((base, vsize, firstpage, pagecnt))
    print("  OBJ%d base=0x%x vsize=0x%x firstpage=%d pages=%d" % (i+1, base, vsize, firstpage, pagecnt))

# map a global (1-based) page number -> owning object index + linear base of that page
def page_object(pg):
    for oi, (base, vsize, firstpage, pagecnt) in enumerate(objs):
        if firstpage <= pg < firstpage + pagecnt:
            return oi, base + (pg - firstpage) * pagesize
    return None, None

# fixup page table: (num_pages+1) dwords -> byte offset into fixup record table for each page
fpt = [u32(fixpageoff + i*4) for i in range(num_pages + 1)]

def rd_index(o, big):
    if big:
        return u16(o), o+2
    return f[o], o+1

seg1 = open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "rb").read()   # the matched code image
# per-object target offset stats + a few code-internal targets
tgt = collections.defaultdict(lambda: [1 << 40, -1, 0])   # obj -> [min,max,count]
srcobj_hist = collections.Counter()
samples = collections.defaultdict(list)                   # obj -> [(src_lin, toff, bytes_at_src)]
parsed = 0
for pg in range(1, num_pages + 1):
    oi, pbase = page_object(pg)
    start = fixrecoff + fpt[pg-1]
    end   = fixrecoff + fpt[pg]
    o = start
    while o < end:
        src = f[o]; flags = f[o+1]; o += 2
        srctype = src & 0x0f
        srclist = src & 0x20
        # source offsets
        src_off0 = 0
        if srclist:
            cnt = f[o]; o += 1
            src_off0 = struct.unpack_from("<h", f, o)[0]
            o += 2 * cnt                       # skip the source offset list
        else:
            src_off0 = struct.unpack_from("<h", f, o)[0]
            o += 2                              # one 2-byte source offset
        # capture source offset(s) in page (re-read: we only stored one for non-list)
        reftype = flags & 0x03
        big_obj = flags & 0x40
        # target object (internal ref, reftype 0)
        if reftype == 0:
            objnum, o = rd_index(o, big_obj)
            # target offset: present unless src is a 16-bit selector fixup (srctype 2)
            if srctype == 2:
                toff = 0
            elif flags & 0x10:                 # 32-bit target offset
                toff = u32(o); o += 4
            else:
                toff = u16(o); o += 2
            st = tgt[objnum]
            st[0] = min(st[0], toff); st[1] = max(st[1], toff); st[2] += 1
            srcobj_hist[oi+1] += 1
            # sample: read the 4 bytes at the source site in the code image
            if oi == 0 and len(samples[objnum]) < 4 and not srclist:
                src_lin = pbase + src_off0
                b = seg1[src_lin - 0x10000: src_lin - 0x10000 + 4]
                if len(b) == 4:
                    samples[objnum].append((src_lin, toff, struct.unpack("<I", b)[0]))
            parsed += 1
        elif reftype == 1:                     # import by ordinal
            _, o = rd_index(o, big_obj)
            o += 2 if not (flags & 0x08) else 1
            if not (flags & 0x80): o += (4 if flags & 0x10 else 2)
        elif reftype == 2:                     # import by name
            _, o = rd_index(o, big_obj)
            o += (4 if flags & 0x10 else 2)
        else:
            break

print("\nparsed %d internal fixups; by SOURCE object:" % parsed, dict(srcobj_hist))
print("TARGET object -> referenced offset range [min,max] and count:")
for objnum in sorted(tgt):
    mn, mx, c = tgt[objnum]
    b = objs[objnum-1][0] if objnum-1 < len(objs) else 0
    print("  -> OBJ%d (base 0x%x): offsets 0x%x .. 0x%x  (%d refs)" % (objnum, b, mn, mx, c))

print("\nSAMPLE fixup sites (src_linear, target_offset, 4 bytes in linear.bin at src):")
for objnum in sorted(samples):
    for src_lin, toff, val in samples[objnum]:
        print("  OBJ%d: src@0x%x  target_off=0x%x  bytes_in_image=0x%x" % (objnum, src_lin, toff, val))
