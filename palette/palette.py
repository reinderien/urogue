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
    im = Image.new('RGB', ((bm+1)*gm, rm))
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
             b * (bm-1)         +
             r * (rm-1) * bm    +
             g * (gm-1) * bm*rm
        ) // pm

        print('%3d %3d %4d %4d %4d' % (p, pt, r, g, b))
        # assert(p == pt)


# Alternatives:
# 884 or 488 - perfect usage but poor uniformity
# 676 - good balance of uniformity and usage

# 666 - perfect uniformity but poor usage
# Used by https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit
test_depths = 6, 6, 6
pal_image(*test_depths).save('palette.png')
pal_dump(*test_depths)
