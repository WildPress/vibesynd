# sys

Memory, file I/O, decompression, DPMI/DOS services (game infrastructure).

| addr | size | status | role |
|------|-----:|--------|------|
| `00017998` | 86 | matched | buffered-read helper |
| `000179f8` | 321 | matched | container-file total-size scan |
| `00017b48` | 1433 | unmatched | the matching container LOADER |
| `000180f8` | 85 | matched |  |
| `00018158` | 191 | unmatched | alloc/init |
| `000184b8` | 803 | matched | DOS/DPMI memory-block (re)allocator |
| `00018828` | 78 | matched | open(path,0x200) |
| `000188e8` | 99 | matched | load+unpack |
| `00018958` | 195 | matched | read |
| `00027e78` | 94 | unmatched | / 0x28728 |
| `00028728` | 187 | unmatched | far (conventional-memory) allocation via DPMI int 0x31 |
