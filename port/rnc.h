/* rnc.h -- RNC ProPack method-1 depacker (native port). */
#ifndef PORT_RNC_H
#define PORT_RNC_H

/* Decompress an RNC-1 stream in `src` (srclen bytes) into `dst` (dstcap bytes).
 * Returns the number of bytes produced, or a negative error:
 *   -1 not RNC-1 / too short   -2 output would overflow dstcap
 *   -3 corrupt Huffman stream  -4 bad back-reference / overflow
 */
long rnc_unpack(const unsigned char *src, long srclen, unsigned char *dst, long dstcap);

#endif
