# sound

Game-side sound system: driver load/init, XMIDI init, channel select.

| addr | size | status | role |
|------|-----:|--------|------|
| `00035d08` | 346 | unmatched | sound-driver load+init |
| `00038cf8` | 741 | unmatched | XMIDI music-system init |
| `00038fe8` | 154 | matched | sound |
