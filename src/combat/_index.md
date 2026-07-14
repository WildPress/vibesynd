# combat

Weapons, damage, projectiles, line-of-sight, targeting (the 0x34xxx cluster).

| addr | size | status | role |
|------|-----:|--------|------|
| `0002d358` | 96 | matched |  |
| `0002d3b8` | 109 | matched | shot-cursor commit |
| `0002d6c8` | 110 | matched | / 0x2d358 |
| `0002d738` | 98 | matched | / 0x2d6c8 / 0x2d358 |
| `0002d7a8` | 85 | unmatched | / 0x2d808 / 0x2d868 |
| `0002d808` | 84 | unmatched | / 0x2d868 |
| `0002d868` | 85 | unmatched |  |
| `0002def8` | 1284 | unmatched |  |
| `0002e4f8` | 132 | matched | 4-direction step search |
| `0002e5f8` | 519 | unmatched | / 0x2e808 |
| `0002e808` | 388 | unmatched | line-of-sight trace |
| `00030508` | 212 | matched | / 0x30708 |
| `00030708` | 142 | matched | entity |
| `00030868` | 261 | unmatched | (re)acquire+engage target |
| `00034048` | 56 | unmatched | snap direction toward target |
| `00034088` | 129 | unmatched |  |
| `00034118` | 66 | matched | / 0x34168 |
| `00034168` | 38 | matched | damage core (`health -= dmg`) |
| `00034198` | 453 | unmatched | shot |
| `00034608` | 590 | unmatched | pick passable direction |
| `00034858` | 964 | unmatched | top-level weapon fire (writes the `g_10b5e/5c/5a` accumulators) |
