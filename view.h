#pragma once

// Use default palette from
// https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit

// Colour components to palette index
// 0 <= R, G, B < 6
#define RGB2P(r, g, b) (16 + (b) + (g)*6 + (r)*6*6)

// Greyscale to palette index
// 0 <= w < 24
#define GS2P(w) (232 + (w))

void view_init();
void view_destroy();
void view_splash();
