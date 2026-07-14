# startup

Program & session startup: command-line/main, session init, timer & keyboard install.

| addr | size | status | role |
|------|-----:|--------|------|
| `00012ca8` | 239 | matched | session init (clears bit 4 in each of 0x80 records) |
| `00020fc8` | 1510 | unmatched | player/team record init (0x417 template records + `g_5788`/`g_539c` ta |
| `00022858` | 415 | unmatched | mission/map init sequence (builds the `g_5358` column table, 3 pools,  |
| `00024be8` | 1395 | matched | command-line / `main` arg loop — BIOS video-mode save, build command s |
| `000252d8` | 81 | matched | PIT timer setup + `d_setvec` |
