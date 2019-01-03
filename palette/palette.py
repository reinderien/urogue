#!/usr/bin/env python3

from PIL import Image

def palette(lims):
    rm, gm, bm = lims
    im = Image.new('RGB', (gm, (rm+1)*bm))
    for r in range(rm):
        for g in range(gm):
            for b in range(bm):
                x = g
                y = r + (1+rm)*b
                colour = tuple(int(256*v/m)
                               for v, m in zip((r,g,b), lims))
                im.putpixel((x, y), colour)
    return im

def equal():
    """
    Here, we only support 256 or greater colour indices. If
    we support even distribution of channels,

       2^(8/3) ~ 6.3
       6^3 = 216
    """
    return palette((6, 6, 6))

def green676():
    """
    Similar to equal, but use 7 values on the green axis; total values 6*7*6
    = 252
    """
    return palette((6, 7, 6))

def bitwise():
    """
    Here, rather than an equal distribution of 6 values per channel, we find the
    number of bits allocatable for each division then make a compromise:

       8/3 = 2.7
       8 = 3 + 3 + 2
    """
    return palette((8, 8, 4))

green676().save('palette.png')
