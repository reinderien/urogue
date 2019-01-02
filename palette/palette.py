#!/usr/bin/env python3

from PIL import Image

def equal():
    """
    Here, we only support 256 or greater colour indices. If
    we support even distribution of channels,

       2^(8/3) ~ 6.3
       6^3 = 216
    """
    im = Image.new('RGB', (6, 6*7))
    scale = 255/5
    for r in range(6):
        for g in range(6):
            for b in range(6):
                x = r
                y = g + 7*b
                colour = tuple(int(scale*v) for v in (r,g,b))
                im.putpixel((x, y), colour)
    return im

def bitwise():
    """
    Here, rather than an equal distribution of 6 values per channel, we find the
    number of bits allocatable for each division then make a compromise:

       8/3 = 2.7
       8 = 3 + 3 + 2
    """
    lims = 8, 8, 4
    rm, gm, bm = lims
    im = Image.new('RGB', (rm, (gm+1)*bm))
    for r in range(rm):
        for g in range(gm):
            for b in range(bm):
                x = r
                y = g + (1+gm)*b
                colour = tuple(int(255*v/(m-1.))
                               for v, m in zip((r,g,b), lims))
                im.putpixel((x, y), colour)
    return im

bitwise().save('palette.png')
