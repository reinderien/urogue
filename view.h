#pragma once

// Channel resolutions for palette
#define RRES 6
#define GRES 7
#define BRES 6
#define PALSIZE (RRES*GRES*BRES)

// Colour components to palette index
// 0 <= R < RRES
// 0 <= G < GRES
// 0 <= B < BRES
#define C2P(r, g, b) (r + (g)*RRES + (b)*RRES*GRES)

// Form a pair ID from back and fore palette indices
#define FBPAIR(f, b) (1 + f + PALSIZE*(b))

void view_init();
void view_destroy();
void view_splash();
