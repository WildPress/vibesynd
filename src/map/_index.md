# map

Map / tile / isometric passability, column lookup, minimap & radar render.

| addr | size | status | role |
|------|-----:|--------|------|
| `00019608` | 3474 | unmatched | the full radar/tactical-map renderer |
| `0001a8c8` | 72 | matched | / 0x1a918 |
| `0001a918` | 236 | matched | grid fill |
| `00020d18` | 52 | unmatched | column-table relocate |
| `00028ec8` | 218 | matched | column lookup |
| `0002d468` | 265 | unmatched | path/passability probe twins (16-entry tile-class jump table) |
| `0002d5b8` | 259 | unmatched | / 0x2d468 |
| `00033c38` | 180 | matched | (x) / 0x33cf8 (y) |
| `00033cf8` | 181 | matched | (y) |
| `00033fb8` | 137 | matched | map-passability check |
| `00034368` | 666 | unmatched |  |
