#!/usr/bin/env python3

from PIL import Image

def pal_colours(*args, pm):
    rm, gm, bm = args

    for p in range(rm*gm*bm):
        ri =  p             % rm
        gi = (p //  rm    ) % gm
        bi = (p // (rm*gm)) % bm
        r = pm * ri // (rm-1)
        g = pm * gi // (gm-1)
        b = pm * bi // (bm-1)
        yield ri, gi, bi, r, g, b

def pal_image(*args):
    rm, gm, bm = args
    im = Image.new('RGB', (gm, (rm+1)*bm))
    for ri, gi, bi, r, g, b in pal_colours(*args, pm=255):
        x = gi
        y = ri + (1+rm)*bi
        im.putpixel((x, y), (r, g, b))
    return im

def pal_dump(*args):
    rm, gm, bm = args
    pm = 1000
    for p, colours in zip(range(rm*gm*bm), pal_colours(*args, pm=pm)):
        r, g, b = colours[3:]
        pt = (
             r    * (rm-1)         +
            (g+1) * (gm-1) * rm    +
             b    * (bm-1) * rm*gm
        ) // pm

        assert(p == pt)

        print('%3d %4d %4d %4d' % (p, r, g, b))

# Alternatives:
# 6, 6, 6 - perfect uniformity but poor usage
# 8, 8, 4 - perfect usage but poor uniformity
test_colours = 6, 7, 6
pal_image(*test_colours).save('palette.png')
pal_dump(*test_colours)
