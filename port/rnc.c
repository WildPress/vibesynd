/* rnc.c -- RNC ProPack method-1 depacker for the native port.
 *
 * A C port of tools/rnc_unpack.py, itself a faithful replica of the game's own
 * rnc_decompress (@0x3a1ec) and helpers. Validated: every HPAL and MSELECT palette
 * unpacks to exactly 768 bytes with all values <= 0x3f (6-bit VGA), and MLOGOS.DAT
 * to its full 40960 bytes.
 *
 * The game's palettes and several screens are stored RNC-1 compressed; the port
 * needs this to load the user's own data files at runtime (no assets are bundled).
 */
#include "rnc.h"
#include <string.h>

#define MASK16 0xffffu

static unsigned ror16(unsigned v, int n) {
    n &= 15;
    return n ? (((v >> n) | (v << (16 - n))) & MASK16) : (v & MASK16);
}
static unsigned rol16(unsigned v, int n) {
    n &= 15;
    return n ? (((v << n) | (v >> (16 - n))) & MASK16) : (v & MASK16);
}

typedef struct {
    const unsigned char *d;
    long len;
    long esi;
    unsigned lo, hi;   /* 0xbfbc current word, 0xbfbe reservoir word */
    int cnt;           /* 0xbfc1 bits available in reservoir */
} rnc_st;

static unsigned rnc_word(rnc_st *s, long off) {
    if (off + 1 < s->len) return (unsigned)(s->d[off] | (s->d[off + 1] << 8));
    if (off < s->len)     return s->d[off];
    return 0;
}

static unsigned rnc_bits(rnc_st *s, int n) {
    unsigned ret = s->lo & ((1u << n) - 1);
    int ch = s->cnt - n, cl;
    unsigned dx;
    if (ch >= 0) {
        cl = n;
        s->cnt = ch;
    } else {
        int oldcnt = s->cnt;
        dx = ror16(s->hi & ((1u << oldcnt) - 1), oldcnt);
        s->hi = (s->hi >> oldcnt) & MASK16;
        s->lo = ((s->lo >> oldcnt) | dx) & MASK16;
        s->esi += 2;
        s->hi = rnc_word(s, s->esi);
        cl = n - oldcnt;
        s->cnt = 16 - cl;
    }
    dx = ror16(s->hi & ((1u << cl) - 1), cl);
    s->hi = (s->hi >> cl) & MASK16;
    s->lo = ((s->lo >> cl) | dx) & MASK16;
    return ret;
}

/* one Huffman decode table: parallel arrays, matched linearly (shortest code first) */
typedef struct { unsigned mask[64], code[64]; int vclass[64], n; } huf;

static void rnc_make_huffman(rnc_st *s, huf *h) {
    int num = (int)rnc_bits(s, 5), i, L, k;
    unsigned codeb = 0;
    h->n = 0;
    if (num == 0) return;
    int lengths[32];
    if (num > 32) num = 32;
    for (i = 0; i < num; i++) lengths[i] = (int)rnc_bits(s, 4);
    for (L = 1; L <= 16; L++) {
        for (i = 0; i < num; i++) {
            if (lengths[i] == L) {
                unsigned mask = (1u << L) - 1, bx = (codeb >> (16 - L)) & MASK16, code = 0;
                for (k = 0; k < L; k++) { code = (code << 1) | (bx & 1); bx >>= 1; }
                if (h->n < 64) {
                    h->mask[h->n] = mask;
                    h->code[h->n] = code & mask;
                    h->vclass[h->n] = i;
                    h->n++;
                }
                codeb = (codeb + (1u << (16 - L))) & MASK16;
            }
        }
    }
}

static int rnc_read_huffman(rnc_st *s, const huf *h) {
    int e;
    for (e = 0; e < h->n; e++) {
        if ((s->lo & h->mask[e]) == h->code[e]) {
            int L = 0, i = h->vclass[e];
            unsigned m = h->mask[e];
            while (m) { L++; m >>= 1; }
            rnc_bits(s, L);
            if (i < 2) return i;
            return (int)((1u << (i - 1)) | rnc_bits(s, i - 1));
        }
    }
    return -1;   /* no match -> corrupt stream */
}

static void rnc_resync(rnc_st *s) {
    int cl = s->cnt;
    unsigned w0 = rnc_word(s, s->esi), rolled = rol16(w0, cl);
    unsigned dxmask = (1u << cl) - 1, keep = rolled & dxmask, w1 = rnc_word(s, s->esi + 2);
    s->lo &= dxmask;
    s->lo = (s->lo | ((w0 << cl) & MASK16)) & MASK16;
    s->hi = (((w1 << cl) & MASK16) | keep) & MASK16;
}

long rnc_unpack(const unsigned char *src, long srclen, unsigned char *dst, long dstcap) {
    rnc_st s;
    huf raw, dist, len;
    long produced = 0, unpacked_size;
    int blocks, b;

    if (srclen < 18 || src[0] != 'R' || src[1] != 'N' || src[2] != 'C' || src[3] != 1)
        return -1;
    unpacked_size = ((long)src[4] << 24) | (src[5] << 16) | (src[6] << 8) | src[7];
    if (unpacked_size > dstcap) return -2;
    blocks = src[17];

    s.d = src; s.len = srclen; s.esi = 18; s.cnt = 0;
    s.lo = rnc_word(&s, 18); s.hi = 0;
    rnc_bits(&s, 2);                                   /* discard first two bits */

    for (b = 0; b < blocks; b++) {
        int count, first = 1;
        rnc_make_huffman(&s, &raw);
        rnc_make_huffman(&s, &dist);
        rnc_make_huffman(&s, &len);
        count = (int)rnc_bits(&s, 16);
        while (count > 0) {
            if (!first) {
                int distval = rnc_read_huffman(&s, &dist);
                int lenval  = rnc_read_huffman(&s, &len);
                long dd, mlen, i2;
                if (distval < 0 || lenval < 0) return -3;
                dd = distval + 1; mlen = lenval + 2;
                for (i2 = 0; i2 < mlen; i2++) {
                    if (produced < dd || produced >= dstcap) return -4;
                    dst[produced] = dst[produced - dd];
                    produced++;
                }
            }
            first = 0;
            {
                int litlen = rnc_read_huffman(&s, &raw), k;
                if (litlen < 0) return -3;
                for (k = 0; k < litlen; k++) {
                    if (produced >= dstcap || s.esi + k >= srclen) return -4;
                    dst[produced++] = src[s.esi + k];
                }
                if (litlen) { s.esi += litlen; rnc_resync(&s); }
            }
            count--;
        }
    }
    return produced < unpacked_size ? produced : unpacked_size;
}
