# entity

Entity/agent/vehicle pool + spatial-grid threading, spawns, chain walks.

| addr | size | status | role |
|------|-----:|--------|------|
| `00013bc8` | 97 | matched | , 0x376f8, 0x37738, 0x37778, 0x37a48, |
| `0001c178` | 119 | matched | into `g_15e70` |
| `00022b38` | 109 | matched | free-slot scan of `g_15e70` |
| `00022ba8` | 107 | matched |  |
| `00026c78` | 292 | unmatched | move entity to (x,y,z) (unlink old cell → insert new) |
| `00026da8` | 106 | unmatched | unlink from cell |
| `00026e18` | 130 | matched | head-insert into a cell list |
| `0002fbc8` | 211 | unmatched | detach from |
| `00036c28` | 69 | matched | , 0x36c78 |
| `00036c78` | 145 | matched |  |
| `00037658` | 148 | matched | / 0x37878 |
| `000376f8` | 59 | matched | , 0x37738, 0x37778, 0x37a48, |
| `00037738` | 50 | matched | , 0x37778, 0x37a48, |
| `00037778` | 64 | matched | , 0x37a48, |
| `00037878` | 155 | matched |  |
| `00037918` | 304 | unmatched | drop/scatter carried items |
| `00037a48` | 64 | matched | , |
