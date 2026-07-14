# multiplayer

NetBIOS-over-DPMI session setup, NCB send/receive, sync barriers.

| addr | size | status | role |
|------|-----:|--------|------|
| `00014078` | 274 | matched |  |
| `000272b8` | 363 | matched | player-record sync barrier |
| `00027428` | 1482 | unmatched | session setup (player-count prompt, name broadcast, peer connect, read |
| `000279f8` | 141 | matched | far-ptr slot-table scan |
| `00027d88` | 230 | unmatched | NCB submit |
| `00028228` | 313 | unmatched | / 0x28368 |
| `00028368` | 311 | unmatched | session ops (0x91/0x90) |
| `000284a8` | 173 | matched | / 0x28558 |
| `00028558` | 198 | matched | NCB send/receive (DPMI mailbox |
| `00028878` | 114 | matched | / ~0x288f8 |
| `000288f8` | 140 | unmatched | chunked transfer |
