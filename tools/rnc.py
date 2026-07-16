#!/usr/bin/env python3
"""RNC ProPack method 1 decompressor (Rob Northen Compression).

Syndicate's data files are RNC-packed, so we need this to unpack the assets. The
algorithm matches the game's own decoder (which we decoded during the decomp). The
bit-reader and literal-run handling follow the reference Python implementation at
https://github.com/bitplane/propack (MIT), which saved a lot of debugging. Method 2
is not needed (Syndicate uses method 1, unencrypted).

  python3 tools/rnc.py <packed> [<out>]     # unpack, verifying the CRC
"""
import struct, sys

HEADER_SIZE = 18


def _crc_table():
    t = []
    for i in range(256):
        c = i
        for _ in range(8):
            c = (c >> 1) ^ 0xA001 if c & 1 else c >> 1
        t.append(c)
    return t


CRC = _crc_table()


def crc16(data, start=0, length=None):
    if length is None:
        length = len(data) - start
    c = 0
    for i in range(start, start + length):
        c = (c >> 8) ^ CRC[(c ^ data[i]) & 0xFF]
    return c


def _inverse_bits(value, count):
    r = 0
    for _ in range(count):
        r = (r << 1) | (value & 1)
        value >>= 1
    return r


class _BitReader:
    def __init__(self, data, offset):
        self.data = data
        self.pos = offset
        self.buf = 0
        self.cnt = 0

    def peek(self, off):
        i = self.pos + off
        return self.data[i] if i < len(self.data) else 0

    def read_byte(self):
        b = self.peek(0)
        self.pos += 1
        return b

    def bits(self, count):
        out = 0
        step = 1
        for _ in range(count):
            if not self.cnt:
                b1 = self.read_byte()
                b2 = self.read_byte()
                self.buf = (self.peek(1) << 24) | (self.peek(0) << 16) | (b2 << 8) | b1
                self.cnt = 16
            if self.buf & 1:
                out |= step
            self.buf >>= 1
            step <<= 1
            self.cnt -= 1
        return out


def _make_table(br):
    n = br.bits(5)
    if not n:
        return []
    if n > 16:
        n = 16
    depths = [br.bits(4) for _ in range(n)]
    codes = [0] * n
    val = 0
    div = 0x80000000
    for bitlen in range(1, 17):
        for i in range(n):
            if depths[i] == bitlen:
                codes[i] = _inverse_bits(val // div, bitlen)
                val += div
        div >>= 1
    return [(depths[i], codes[i]) for i in range(n)]


def _decode(br, table):
    for i, (depth, code) in enumerate(table):
        if depth and code == (br.buf & ((1 << depth) - 1)):
            br.bits(depth)
            if i < 2:
                return i
            return br.bits(i - 1) | (1 << (i - 1))
    raise ValueError("no huffman code matched")


def unpack(data):
    sig, method, unpacked, packed, ucrc, pcrc, leeway, chunks = struct.unpack_from(">3sBIIHHBB", data)
    if sig != b"RNC" or method != 1:
        raise ValueError("not RNC method 1")
    br = _BitReader(data, HEADER_SIZE)
    if br.bits(1):
        raise ValueError("file is locked")
    if br.bits(1):
        raise ValueError("file is encrypted")
    out = bytearray()
    while len(out) < unpacked:
        raw_t = _make_table(br)
        off_t = _make_table(br)
        cnt_t = _make_table(br)
        subs = br.bits(16)
        for sc in range(subs):
            litlen = _decode(br, raw_t)
            if litlen:
                for _ in range(litlen):
                    out.append(br.read_byte())
                # reload the lookahead after reading raw bytes straight from the stream
                lo, mid, hi = br.peek(0), br.peek(1), br.peek(2)
                br.buf = (((hi << 16) | (mid << 8) | lo) << br.cnt) | (br.buf & ((1 << br.cnt) - 1))
            if sc < subs - 1:
                offset = _decode(br, off_t) + 1
                count = _decode(br, cnt_t) + 2
                for _ in range(count):
                    out.append(out[-offset])
    out = bytes(out[:unpacked])
    if crc16(out) != ucrc:
        raise ValueError("unpacked CRC mismatch")
    return out


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return
    data = open(sys.argv[1], "rb").read()
    out = unpack(data)
    print("unpacked %d bytes, CRC OK" % len(out))
    if len(sys.argv) > 2:
        open(sys.argv[2], "wb").write(out)
    elif len(out) == 768:
        print("palette: 256 RGB, value range %d..%d (VGA 6-bit if <= 63)" % (min(out), max(out)))


if __name__ == "__main__":
    main()
