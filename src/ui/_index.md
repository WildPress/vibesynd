# ui

Menus, panels, HUD, text engine (glyph/word-wrap), widgets.

| addr | size | status | role |
|------|-----:|--------|------|
| `0001bc28` | 1334 | unmatched | selection-marker dispatcher |
| `00020018` | 314 | unmatched | list select |
| `00020158` | 1028 | matched | item-detail panel |
| `000205f8` | 299 | unmatched | / 0x20728 |
| `00020728` | 310 | unmatched | menu list twins |
| `00025d58` | 1234 | unmatched | agent detail |
| `000265d8` | 331 | matched | stats-panel drawer |
| `00029ad8` | 372 | matched | status-line builder |
| `00029c58` | 238 | matched | icon/text selector |
| `0002a288` | 1427 | unmatched | radar panel |
| `00035638` | 129 | matched | message line |
| `000361a8` | 90 | matched | / 0x36208 / 0x36298 / 0x36338 |
| `00036208` | 129 | matched | / 0x36298 / 0x36338 |
| `00036298` | 148 | matched | / 0x36338 |
| `00036338` | 148 | matched |  |
| `000363d8` | 523 | unmatched | word-wrap |
| `000365e8` | 92 | unmatched | / 0x36648 |
| `00036648` | 79 | unmatched | width measure |
| `00036698` | 362 | unmatched | the core glyph drawer (6-byte font records) |
| `00036808` | 987 | unmatched | keyboard line-editor widget |
