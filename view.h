#pragma once

// Use default palette from
// https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit

// Channel size, same for red, green and blue
#define NC 6

// Colour components to palette index
// 0 <= R, G, B < 6
#define RGB2P(r, g, b) (16 + (b) + (g)*NC + (r)*NC*NC)

// Greyscale to palette index
// 0 <= w < 24
#define GS2P(w) (232 + (w))

// Character aspect ratio. This is basically a guess
#define ASPECT 0.5

// Rather than including ncurses.h here
typedef struct _win_st WINDOW;

typedef struct {
    WINDOW *win;
} View;
extern View view;


void view_init();
void view_destroy();
void view_splash();
