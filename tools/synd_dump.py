#!/usr/bin/env python3
"""Bulk-extract the Syndicate game data to a browsable folder (local, gitignored).

Converts what we understand to viewable/listenable form and dumps the rest raw:
  sprites  HSPR/MSPR .TAB+.DAT   -> PNG contact sheets
  screens  raw 320x200 images    -> PNG
  sound    SOUND/ISNDS/GSOUND    -> WAV (raw 8-bit unsigned PCM, best guess rate)
  music    .XMI                  -> copied (XMIDI, convert separately)
  raw      everything decoded    -> .bin, plus an INDEX

Reads the game's own files, writes only into the output dir. Nothing here is committed.

  python3 tools/synd_dump.py "<GOG DATA dir>" extracted
"""
import os, sys, struct, glob
import rnc
import synd_sprites as sp
import synd_tiles as st

RATE = 11025  # sound sample rate guess (adjust if pitch is off)


def load(path):
    d = open(path, "rb").read()
    packed = d[:3] == b"RNC"
    try:
        return (rnc.unpack(d) if packed else d), packed
    except Exception:
        return d, packed


def write_wav(path, pcm8):
    n = len(pcm8)
    with open(path, "wb") as f:
        f.write(b"RIFF")
        f.write(struct.pack("<I", 36 + n))
        f.write(b"WAVEfmt ")
        f.write(struct.pack("<IHHIIHH", 16, 1, 1, RATE, RATE, 1, 8))
        f.write(b"data")
        f.write(struct.pack("<I", n))
        f.write(pcm8)


def write_png_indexed(path, w, h, pixels, palette):
    rgb = [palette[p] for p in pixels]
    sp.write_png(path, w, h, rgb)


def main():
    src, out = sys.argv[1], sys.argv[2]
    for sub in ("sprites", "screens", "sound", "music", "raw"):
        os.makedirs(os.path.join(out, sub), exist_ok=True)
    palette = sp.load_palette(os.path.join(src, "HPALETTE.DAT"))
    screen_pal = sp.load_palette(os.path.join(src, "MSELECT.PAL"))  # full 256-colour palette
    os.makedirs(os.path.join(out, "tiles"), exist_ok=True)
    index = []

    # tiles (HBLK map-block files)
    for f in sorted(glob.glob(os.path.join(src, "HBLK*.DAT"))):
        name = os.path.basename(f)[:-4].lower()
        png = os.path.join(out, "tiles", name + ".png")
        try:
            sys.argv = ["", f, os.path.join(src, "HPALETTE.DAT"), png]
            st.main()
            index.append(("tiles/%s.png" % name, "256 isometric map tiles"))
        except Exception as e:
            index.append(("tiles/%s" % name, "FAILED %s" % e))

    files = sorted(glob.glob(os.path.join(src, "*")))
    # sprite banks: match *SPR*.DAT with a sibling .TAB
    for dat in files:
        b = os.path.basename(dat).upper()
        if b.endswith(".DAT") and "SPR" in b:
            tab = dat[:-4] + ".TAB"
            if os.path.exists(tab):
                name = b[:-4].lower()
                png = os.path.join(out, "sprites", name + ".png")
                # 8-bit menu sprites (MSPR) use the full menu palette; 4-bit game
                # sprites (HSPR) use the 16-colour sprite palette.
                pal_file = "MSELECT.PAL" if "MSPR" in b else "HPALETTE.DAT"
                try:
                    ents, data = sp.load_sprites(tab, dat)
                    sys.argv = ["", tab, dat, os.path.join(src, pal_file), png, "0", str(len(ents)), "24", "1"]
                    sp.main()
                    index.append(("sprites/%s.png" % name, "%d sprites" % (len(ents) - 1)))
                except Exception as e:
                    index.append(("sprites/%s" % name, "FAILED %s" % e))

    for f in files:
        b = os.path.basename(f)
        bu = b.upper()
        if not bu.endswith(".DAT"):
            if bu.endswith(".XMI"):
                data, _ = load(f)
                open(os.path.join(out, "music", b), "wb").write(data)
                index.append(("music/%s" % b, "XMIDI music, %d bytes" % len(data)))
            continue
        if "SPR" in bu:
            continue
        data, packed = load(f)
        n = len(data)
        raw_path = os.path.join(out, "raw", b + ".bin")
        open(raw_path, "wb").write(data)
        tag = "raw %d bytes%s" % (n, " (was RNC)" if packed else "")
        # screens: raw 320x200 indexed
        if n == 64000:
            png = os.path.join(out, "screens", b[:-4] + ".png")
            try:
                write_png_indexed(png, 320, 200, data, screen_pal)
                index.append(("screens/%s.png" % b[:-4], "320x200 screen"))
                continue
            except Exception as e:
                tag += " (screen render failed: %s)" % e
        # sound banks -> WAV
        if any(k in bu for k in ("SOUND", "ISNDS", "GSOUND")) and n > 2000:
            wav = os.path.join(out, "sound", b[:-4] + ".wav")
            write_wav(wav, data)
            index.append(("sound/%s.wav" % b[:-4], "raw 8-bit PCM @ %dHz, %d bytes" % (RATE, n)))
            continue
        index.append(("raw/%s.bin" % b, tag))

    with open(os.path.join(out, "INDEX.md"), "w", encoding="utf-8") as fh:
        fh.write("# Syndicate asset dump\n\nExtracted locally from the game files. Not for redistribution.\n\n")
        for path, desc in sorted(index):
            fh.write("- `%s` %s\n" % (path, desc))
    print("dumped %d items -> %s (see INDEX.md)" % (len(index), out))


if __name__ == "__main__":
    main()
