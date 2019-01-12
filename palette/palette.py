#!/usr/bin/env python3

from PIL import Image

def pal_colours(*args, pm):
    rm, gm, bm = args

    # For ANSI compatibility:
    # From fastest to slowest axis: B, R, G

    for p in range(rm*gm*bm):
        bi =  p             % bm
        ri = (p //  bm    ) % rm
        gi = (p // (bm*rm)) % gm
        r = pm * ri // (rm-1)
        g = pm * gi // (gm-1)
        b = pm * bi // (bm-1)
        yield ri, gi, bi, r, g, b

def pal_image(*args):
    rm, gm, bm = args
    im = Image.new('RGB', ((rm+1)*gm, bm))
    for ri, gi, bi, r, g, b in pal_colours(*args, pm=255):
        y = ri
        x = bi + (1+bm)*gi
        im.putpixel((x, y), (r, g, b))
    return im

def pal_dump(*args):
    rm, gm, bm = args
    pm = 1000
    for p, colours in zip(range(rm*gm*bm), pal_colours(*args, pm=pm)):
        r, g, b = colours[3:]
        pt = (
             b    * (bm-1)         +
            (r+1) * (rm-1) * bm    +
             g    * (gm-1) * bm*rm
        ) // pm

        assert(p == pt)

        print('%3d %4d %4d %4d' % (p, r, g, b))

# Alternatives:
# 8, 8, 4 - perfect usage but poor uniformity
# 6, 7, 6 - good balance of uniformity and usage

# 6, 6, 6 - perfect uniformity but poor usage
# Used by https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit
test_colours = 6, 6, 6
pal_image(*test_colours).save('palette.png')
pal_dump(*test_colours)
