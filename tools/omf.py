#!/usr/bin/env python3
"""Minimal OMF (.obj) reader: extract the _TEXT segment bytes and the exact byte
offsets + sizes of every relocation (FIXUPP record), so the matcher can mask real
fixup sites instead of guessing all-zero placeholders. This catches relocations
that carry a non-zero addend (array bounds, arr[const], loop cmp), which the naive
zero-run masker misses.

    from omf import text_bytes_and_fixups
    text, fixups = text_bytes_and_fixups("build/FUN_x.obj")   # fixups = [(off,size),...]
"""

def _index(data, i):
    """OMF index: 1 byte if <0x80, else 2 bytes (big-endian, high bit set on first)."""
    b = data[i]
    if b & 0x80:
        return ((b & 0x7f) << 8) | data[i + 1], i + 2
    return b, i + 1

def _records(raw):
    i, n = 0, len(raw)
    while i + 3 <= n:
        rectype = raw[i]
        rlen = raw[i + 1] | (raw[i + 2] << 8)
        body = raw[i + 3 : i + 3 + rlen - 1]     # drop trailing checksum byte
        yield rectype, body
        i += 3 + rlen

# fixup LOC field (bits 13-10 of locat) -> size in bytes of the fixed location
_LOC_SIZE = {0: 1, 4: 1, 1: 2, 5: 2, 2: 2, 9: 4, 13: 4, 3: 4, 11: 6}

def text_bytes_and_fixups(path):
    raw = open(path, "rb").read()
    recs = list(_records(raw))

    # LNAMES (0x96): collect names (1-based)
    names = []
    for rt, body in recs:
        if rt == 0x96:
            j = 0
            while j < len(body):
                ln = body[j]; j += 1
                names.append(body[j:j + ln].decode("latin1")); j += ln

    # SEGDEF (0x98 / 0x99 SEGDEF32): map segment ordinal -> name; find _TEXT ordinal
    seg_ord, text_seg = 0, None
    for rt, body in recs:
        if rt in (0x98, 0x99):
            seg_ord += 1
            acbp = body[0]; j = 1
            if (acbp >> 5) & 7 == 0:      # A==0 -> absolute segment: frame(2)+offset(1)
                j += 3
            j += 4 if rt == 0x99 else 2   # segment length
            name_idx, j = _index(body, j)
            nm = names[name_idx - 1] if 1 <= name_idx <= len(names) else ""
            if nm == "_TEXT":
                text_seg = seg_ord

    # accumulate _TEXT LEDATA and its fixups
    buf = bytearray()
    fixups = []
    cur_ledata_off = None    # enum offset of the most recent _TEXT LEDATA
    for rt, body in recs:
        if rt in (0xA0, 0xA1):               # LEDATA / LEDATA32
            j = 0
            segi, j = _index(body, j)
            if rt == 0xA1:
                enoff = int.from_bytes(body[j:j + 4], "little"); j += 4
            else:
                enoff = int.from_bytes(body[j:j + 2], "little"); j += 2
            data = body[j:]
            if segi == text_seg:
                end = enoff + len(data)
                if len(buf) < end:
                    buf.extend(b"\x00" * (end - len(buf)))
                buf[enoff:end] = data
                cur_ledata_off = enoff
            else:
                cur_ledata_off = None
        elif rt in (0x9C, 0x9D):             # FIXUPP / FIXUPP32
            if cur_ledata_off is None:
                continue
            is32 = rt == 0x9D
            j = 0
            while j < len(body):
                b0 = body[j]
                if not (b0 & 0x80):          # THREAD subrecord: skip (with any index)
                    method = (b0 >> 2) & 7
                    j += 1
                    if (method & 3) in (0, 1, 2):
                        _, j = _index(body, j)
                    continue
                locat = (body[j] << 8) | body[j + 1]; j += 2
                loc = (locat >> 10) & 0xf
                drec_off = locat & 0x3ff
                fixdata = body[j]; j += 1
                F = fixdata & 0x80; frame_meth = (fixdata >> 4) & 7
                T = fixdata & 0x08; P = fixdata & 0x04; targ_meth = fixdata & 3
                if not F and frame_meth in (0, 1, 2):
                    _, j = _index(body, j)
                if not T and targ_meth in (0, 1, 2):
                    _, j = _index(body, j)
                if not P:
                    j += 4 if is32 else 2     # target displacement (addend)
                fixups.append((cur_ledata_off + drec_off, _LOC_SIZE.get(loc, 4)))
    return bytes(buf), sorted(set(fixups))


if __name__ == "__main__":
    import sys
    text, fx = text_bytes_and_fixups(sys.argv[1])
    print(f"{sys.argv[1]}: {len(text)} text bytes, {len(fx)} fixups")
    print("  bytes :", text.hex())
    print("  fixups:", [(hex(o), s) for o, s in fx])
