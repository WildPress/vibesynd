# economy

Economy, equipment, research, funding, target claiming, save-game.

| addr | size | status | role |
|------|-----:|--------|------|
| `00012da8` | 1188 | unmatched | build one |
| `00015f58` | 957 | unmatched | daily economy tick (busy-wait on game speed, funding commit, 50-region |
| `000164c8` | 303 | matched | per-player target reassignment sweep |
| `000165f8` | 51 | matched | / 0x16638 |
| `00016638` | 55 | matched | claim-record scans |
| `000223c8` | 724 | unmatched | apply an equip template row |
| `000264a8` | 297 | unmatched | slot-claim eligibility |
| `00033568` | 866 | unmatched | funding-entry commit |
| `00035b68` | 279 | matched | save-game (taxes |
