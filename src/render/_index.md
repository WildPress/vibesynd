# render

Low-level drawing primitives: blit, VGA, masked copy, gauge, lines.

| addr | size | status | role |
|------|-----:|--------|------|
| `00019318` | 585 | unmatched | circle outline |
| `0001ff98` | 113 | matched | gauge bar |
| `000263f8` | 176 | matched | masked blit |
| `00026778` | 544 | unmatched | dashed line |
| `00035538` | 72 | matched | / 0x35588 |
| `00035588` | 72 | matched | bulk copy |
| `000355d8` | 94 | matched | VGA blit |
