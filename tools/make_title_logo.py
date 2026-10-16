#!/usr/bin/env python3
"""
Converts a logo PNG into 256-colour (8bpp) tile data for the title screen's logo layer
and writes include/new/title_logo_data.h.  Needs Python 3 + Pillow (pip install pillow).

Why not grit?  A real FireRed-style logo uses far more than 16 colours, and the graphics/
folder's grit flags are 4bpp.  This writes exact data, so the result is predictable.

Usage (from the repo root):
    python tools/make_title_logo.py title_assets/logo_source.png

Options:
    --key R,G,B     colour that means "transparent" (default: palette index 0 / 0,255,41)
    --banner x0,y0,x1,y1
                    pixel box (inclusive) of the sub-title banner ("FireRed Version") in the
                    source. Two variants are written: with the banner, and without it (for the
                    editable runtime sub-title text). Default = auto-detect the lowest piece.
    --dx N --dy N   where to put the source's top-left on the 240x160 screen canvas
                    (default: wordmark centred horizontally, same vertical position)
    --pal-base N    first BG palette slot used by the logo (default 48; slots 0-15 = scene,
                    32-47 = text band, 16-31 = sub-title text are taken)
"""
import argparse, sys, os
from collections import deque
from PIL import Image

W, H = 240, 160

def parse_box(s): return tuple(int(v) for v in s.split(','))

def components(mask, w, h):
    seen = [[False]*w for _ in range(h)]; comps = []
    for y in range(h):
        for x in range(w):
            if mask[y][x] and not seen[y][x]:
                q = deque([(x, y)]); seen[y][x] = True; pts = []
                while q:
                    cx, cy = q.popleft(); pts.append((cx, cy))
                    for dy in (-1, 0, 1):
                        for dx in (-1, 0, 1):
                            nx, ny = cx+dx, cy+dy
                            if 0 <= nx < w and 0 <= ny < h and mask[ny][nx] and not seen[ny][nx]:
                                seen[ny][nx] = True; q.append((nx, ny))
                xs = [p[0] for p in pts]; ys = [p[1] for p in pts]
                comps.append((min(xs), min(ys), max(xs), max(ys), pts))
    return comps

def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('png'); ap.add_argument('--key'); ap.add_argument('--banner')
    ap.add_argument('--dx', type=int); ap.add_argument('--dy', type=int, default=0)
    ap.add_argument('--pal-base', type=int, default=48)
    ap.add_argument('--out', default='include/new/title_logo_data.h')
    a = ap.parse_args()

    im = Image.open(a.png).convert('RGBA'); sw, sh = im.size
    px = im.load()
    if a.key: key = tuple(int(v) for v in a.key.split(','))
    else:
        p = Image.open(a.png)
        key = tuple(p.convert('RGB').getpixel((0, 0)))     # top-left pixel = background
    def opaque(x, y):
        r, g, b, al = px[x, y]
        return al > 0 and (r, g, b) != key
    mask = [[opaque(x, y) for x in range(sw)] for y in range(sh)]
    comps = [c for c in components(mask, sw, sh) if len(c[4]) > 15]
    if not comps: sys.exit("no artwork found (check --key)")

    if a.banner: bx0, by0, bx1, by1 = parse_box(a.banner)
    else:
        bx0, by0, bx1, by1 = max(comps, key=lambda c: c[3])[:4]     # lowest piece
        print("auto-detected banner box:", bx0, by0, bx1, by1)
    banner_pts = set()
    for c in comps:
        if (c[0], c[1], c[2], c[3]) == (bx0, by0, bx1, by1): banner_pts = set(c[4])
    if not banner_pts:   # explicit box that is not exactly one piece: take everything inside it
        banner_pts = {(x, y) for y in range(by0, by1+1) for x in range(bx0, bx1+1) if mask[y][x]}

    wx0 = min(c[0] for c in comps if (c[0], c[1]) != (bx0, by0)); wx1 = max(c[2] for c in comps if (c[0], c[1]) != (bx0, by0))
    dx = a.dx if a.dx is not None else ((W - (wx1 - wx0 + 1)) // 2 - wx0)
    dy = a.dy
    print(f"source {sw}x{sh}, wordmark x {wx0}-{wx1}; placing at dx={dx} dy={dy}")

    # palette (15-bit, merged) -> slot numbers pal_base+1...
    def c15(rgb): return (rgb[0] >> 3) | ((rgb[1] >> 3) << 5) | ((rgb[2] >> 3) << 10)
    colours = {}
    order = []
    for y in range(sh):
        for x in range(sw):
            if mask[y][x]:
                v = c15(px[x, y][:3])
                if v not in colours: colours[v] = len(order) + 1; order.append(v)
    if a.pal_base + len(order) > 255: sys.exit(f"too many colours ({len(order)}) for palette slots {a.pal_base}..255")
    print(f"{len(order)} distinct 15-bit colours -> slots {a.pal_base+1}..{a.pal_base+len(order)}")

    def build(include_banner):
        canvas = [[0]*W for _ in range(H)]
        for y in range(sh):
            for x in range(sw):
                if not mask[y][x]: continue
                if not include_banner and (x, y) in banner_pts: continue
                X, Y = x + dx, y + dy
                if 0 <= X < W and 0 <= Y < H:
                    canvas[Y][X] = a.pal_base + colours[c15(px[x, y][:3])]
                else: sys.exit(f"artwork falls off the {W}x{H} screen at ({X},{Y}); adjust --dx/--dy")
        tiles = [bytes(64)]; lookup = {bytes(64): 0}; tmap = []
        for ty in range(H // 8):
            for tx in range(32):
                if tx >= W // 8: tmap.append(0); continue
                t = bytes(canvas[ty*8 + yy][tx*8 + xx] for yy in range(8) for xx in range(8))
                if t not in lookup:
                    lookup[t] = len(tiles); tiles.append(t)
                tmap.append(lookup[t])
        if len(tiles) > 256: sys.exit(f"{len(tiles)} unique tiles; the logo layer holds at most 256. Simplify the art.")
        return canvas, tiles, tmap

    out = ["#pragma once", "",
           "// GENERATED by tools/make_title_logo.py from the logo PNG - regenerate, do not edit by hand.",
           "// 8bpp (256-colour) BG tiles. Pixel value = BG palette slot; 0 = transparent.", "",
           f"#define TITLE_LOGO_PAL_BASE  {a.pal_base}",
           f"#define TITLE_LOGO_PAL_BYTES {(len(order)+1)*2}", ""]
    pal = [0] + order
    out.append(f"static const u16 sTitleLogoPal[{len(pal)}] =\n{{")
    for i in range(0, len(pal), 8): out.append("    " + ", ".join(f"0x{v:04X}" for v in pal[i:i+8]) + ",")
    out.append("};\n")
    for name, inc in (("Full", True), ("Word", False)):
        canvas, tiles, tmap = build(inc)
        print(f"variant {name}: {len(tiles)} tiles")
        out.append(f"#define TITLE_LOGO_{name.upper()}_TILE_WORDS {len(tiles)*16}")
        out.append(f"static const u32 sTitleLogo{name}Tiles[{len(tiles)*16}] =\n{{")
        for t in tiles:
            words = [int.from_bytes(t[i:i+4], 'little') for i in range(0, 64, 4)]
            out.append("    " + ", ".join(f"0x{w:08X}" for w in words) + ",")
        out.append("};")
        out.append(f"static const u16 sTitleLogo{name}Map[32 * 20] =\n{{")
        for r in range(0, 640, 32): out.append("    " + ", ".join(str(v) for v in tmap[r:r+32]) + ",")
        out.append("};\n")
    os.makedirs(os.path.dirname(a.out), exist_ok=True)
    open(a.out, 'w').write("\n".join(out) + "\n")
    print("wrote", a.out)

if __name__ == '__main__': main()
