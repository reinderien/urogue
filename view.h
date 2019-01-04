#pragma once

// Channel resolutions for palette
#define RRES 6
#define GRES 7
#define BRES 6

// Colour components to palette index
// 0 <= R < RRES
// 0 <= G < GRES
// 0 <= B < BRES
#define C2P(r, g, b) (r + g*RRES + b*RRES*GRES)

void view_init();
void view_destroy();
void view_splash();
