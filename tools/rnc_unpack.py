#!/usr/bin/env python3
"""rnc_unpack.py -- RNC ProPack method-1 depacker (Rob Northen Compression).

A faithful Python replica of our decompiled rnc_decompress (@0x3a1ec) and its helpers
rnc_input_bits (@0x3a3c6), rnc_read_huffman (@0x3a383), rnc_make_huffman (@0x3a449),
rnc_read_be_len (@0x3a37a) -- disassembled complete (calls intact) from the game's own
OBJECT1 code image. The game's palettes and some screens are RNC-1 compressed.

State mirrors the globals: lo=0xbfbc, hi=0xbfbe, cnt=0xbfc1, esi=byte index into the
packed stream. Bits are consumed LSB-first; literals are copied raw from the stream and
the bit buffer is re-synced afterwards, exactly as the asm does.

Oracle: HPALETTE.DAT -> 768 bytes, every byte <= 0x3f (6-bit VGA).
"""
import sys, struct

MASK16 = 0xffff


def _ror16(v, n):
    n &= 15
    return ((v >> n) | (v << (16 - n))) & MASK16 if n else v & MASK16


def _rol16(v, n):
    n &= 15
    return ((v << n) | (v >> (16 - n))) & MASK16 if n else v & MASK16


class Rnc:
    def __init__(self, data):
        self.d = data
        self.esi = 0
        self.lo = 0      # 0xbfbc  (current word)
        self.hi = 0      # 0xbfbe  (reservoir word)
        self.cnt = 0     # 0xbfc1  (bits available in reservoir)

    def _word(self, off):
        return (self.d[off] | (self.d[off + 1] << 8)) if off + 1 < len(self.d) else \
               (self.d[off] if off < len(self.d) else 0)

    def input_bits(self, n):
        """rnc_input_bits: return low n bits of lo, then shift the 32-bit [hi:lo]
        window right by n, refilling hi from the stream across a word boundary."""
        ret = self.lo & ((1 << n) - 1)
        ch = self.cnt - n
        if ch >= 0:                       # no refill; consume n bits
            cl = n
            self.cnt = ch
        else:                             # refill path
            oldcnt = self.cnt
            # consume the available `oldcnt` bits first
            dx = _ror16(self.hi & ((1 << oldcnt) - 1), oldcnt)
            self.hi = (self.hi >> oldcnt) & MASK16
            self.lo = ((self.lo >> oldcnt) | dx) & MASK16
            self.esi += 2
            self.hi = self._word(self.esi)
            cl = n - oldcnt               # remaining bits to consume from new hi
            self.cnt = 16 - cl
        dx = _ror16(self.hi & ((1 << cl) - 1), cl)
        self.hi = (self.hi >> cl) & MASK16
        self.lo = ((self.lo >> cl) | dx) & MASK16
        return ret

    def make_huffman(self):
        """rnc_make_huffman: returns [(mask, code, valueclass), ...]."""
        num = self.input_bits(5)
        if num == 0:
            return []
        lengths = [self.input_bits(4) for _ in range(num)]
        table = []
        codeb = 0                          # MSB-aligned running canonical code (16-bit)
        for L in range(1, 17):
            for i in range(num):
                if lengths[i] == L:
                    mask = (1 << L) - 1
                    # code = bit-reverse of codeb's top L bits (ax = reversed low-L of bx)
                    bx = (codeb >> (16 - L)) & MASK16
                    code = 0
                    for _ in range(L):
                        code = (code << 1) | (bx & 1)
                        bx >>= 1
                    table.append((mask, code & mask, i))
                    codeb = (codeb + (1 << (16 - L))) & MASK16
        return table

    def read_huffman(self, table):
        """rnc_read_huffman: match (lo & mask)==code, consume L bits, decode value."""
        for (mask, code, i) in table:
            if (self.lo & mask) == code:
                L = 0
                m = mask
                while m:
                    L += 1; m >>= 1
                self.input_bits(L)          # consume the matched code's L bits
                if i < 2:
                    return i
                return (1 << (i - 1)) | self.input_bits(i - 1)
        raise ValueError("no huffman match; lo=0x%04x" % self.lo)

    def _resync(self):
        """rnc_decompress bit-buffer resync after a byte-aligned literal copy."""
        cl = self.cnt
        w0 = self._word(self.esi)
        rolled = _rol16(w0, cl)
        dxmask = (1 << cl) - 1
        self.lo &= dxmask
        keep = rolled & dxmask
        w1 = self._word(self.esi + 2)
        self.lo = (self.lo | ((w0 << cl) & MASK16)) & MASK16
        self.hi = (((w1 << cl) & MASK16) | keep) & MASK16

    def unpack(self):
        d = self.d
        if d[:3] != b"RNC" or d[3] != 1:
            raise ValueError("not RNC method 1")
        unpacked_size = struct.unpack(">I", d[4:8])[0]
        num_blocks = d[17]
        out = bytearray()
        self.esi = 18
        self.cnt = 0
        self.lo = self._word(18)
        self.hi = 0
        self.input_bits(2)                  # discard first two bits
        for _ in range(num_blocks):
            raw = self.make_huffman()
            dist = self.make_huffman()
            length = self.make_huffman()
            count = self.input_bits(16)
            first = True
            while count > 0:
                if not first:
                    distval = self.read_huffman(dist)
                    lenval = self.read_huffman(length)
                    mlen = lenval + 2
                    dd = distval + 1
                    for _ in range(mlen):
                        out.append(out[-dd])
                first = False
                litlen = self.read_huffman(raw)
                if litlen:
                    for k in range(litlen):
                        out.append(d[self.esi + k])
                    self.esi += litlen
                    self._resync()
                count -= 1
        return bytes(out[:unpacked_size]), unpacked_size


def unpack(data):
    return Rnc(data).unpack()


if __name__ == "__main__":
    path = sys.argv[1]
    data = open(path, "rb").read()
    print("header:", data[:18].hex(), "unpacked=%d blocks=%d" % (
        struct.unpack(">I", data[4:8])[0], data[17]))
    try:
        out, want = unpack(data)
        mx = max(out) if out else 0
        ok = "PALETTE-OK" if (len(out) == want and mx <= 0x3f) else \
             ("SIZE-OK" if len(out) == want else "MISMATCH(%d/%d)" % (len(out), want))
        print("unpacked: %d bytes (want %d), max byte=0x%02x  -> %s" % (len(out), want, mx, ok))
        if len(sys.argv) > 2:
            open(sys.argv[2], "wb").write(out); print("wrote", sys.argv[2])
    except Exception as e:
        import traceback; traceback.print_exc()
        print("FAIL:", e)
