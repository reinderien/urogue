#include <float.h>     // FLT_ROUNDS
#include <math.h>      // sin, sqrt, etc.
#include <stdlib.h>    // rand
#include <sys/param.h> // MIN, MAX
#include <time.h>      // clock_gettime
#include <unistd.h>    // usleep
#include <ncursesw/ncurses.h>

#include "error.h"
#include "util.h"
#include "view.h"

// See https://www.gnu.org/software/libc/manual/html_node/Floating-Point-Parameters.html
#define FLT_ROUNDS_NEAREST 1

const char title[] = "UROGUE";
const int title_len = sizeof(title) - 1;

typedef struct {
    WINDOW *win;
    NCURSES_SIZE_T Y, X, title_left, title_pitch;
    NCURSES_PAIRS_T wo, wn, to, tn, po, pn;
} Wave;


static void noise(const Wave *w) {
    // Fill screen with random characters
    assert_n(wmove(w->win, 0, 0), "move cursor home");

    const char cmin = '!', cmax = '~';

    for (int i = 0; i < w->X*w->Y - 1; i++) {
        char c = cmin + rand()%(cmax - cmin + 1);
        assert_n(waddch(w->win, c), "output char");
    }
}

static void show_title(Wave *w) {
    const NCURSES_SIZE_T width = w->X/3,
                         y = w->Y/2;
    w->title_pitch = width/title_len;
    w->title_left = (w->X - (title_len - 1)*w->title_pitch)/2;

    for (int i = 0; i < title_len; i++) {
        NCURSES_SIZE_T x = w->title_left + i*w->title_pitch;
        assert_n(mvwaddch(w->win, y, x, title[i]), "output title");
    }
}

static void wave_palette(Wave *w) {
    // Black through blue through white
    const int N = 2*NC - 1; // number of colours
    NCURSES_COLOR_T scale[N];
    for (int i = 0; i < NC; i++)
        scale[i] = RGB2P(0, 0, i);
    for (int i = NC; i < N; i++)
        scale[i] = RGB2P(i-NC+1, i-NC+1, NC-1);

    // Set up pairs for "point": varying foreground, black background
    w->po = 1; // point pair offset
    w->pn = N; // point pair count
    for (NCURSES_PAIRS_T i = 0; i < w->pn; i++)
        assert_n(init_pair(i + w->po, scale[i], scale[0]), "init pair");

    // Set up pairs for "wave": varying foreground, dimmer background
    const NCURSES_PAIRS_T off = 1; // offset between fore and back
    w->wo = w->po + w->pn;         // wave pair offset
    w->wn = N - off;               // wave pair count
    for (NCURSES_PAIRS_T i = 0; i < w->wn; i++)
        assert_n(init_pair(i + w->wo, scale[i+off], scale[i]), "init pair");

    // Set up pairs for "title": white foreground, varying background
    w->to = w->wo + w->wn;
    w->tn = N;
    for (NCURSES_PAIRS_T i = 0; i < w->tn; i++)
        assert_n(init_pair(i + w->to, scale[N-1], scale[i]), "init pair");
}

static void wave_point(const Wave *w) {
    const NCURSES_SIZE_T ym = w->Y/2, xm = w->X/2;

    for (NCURSES_PAIRS_T c = 0; c < w->pn; c++) {
        assert_n(wattron(w->win, COLOR_PAIR(c + w->po)), "switch colour");
        assert_n(mvwaddch(w->win, ym, xm, '.'), "output char");
        assert_n(wrefresh(w->win), "refresh");
        assert_c(usleep(100000), "sleep between frames");
    }
}

static NCURSES_PAIRS_T wave_c_from_xy(NCURSES_SIZE_T y, NCURSES_SIZE_T x,
                                      float t, const Wave *w) {

    const float size = MAX(w->X, w->Y),     // largest screen dimension in chars
                xn = (x - w->X/2.)/size, // Normalize, center, correct for aspect
                yn = (y - w->Y/2.)/size / ASPECT,
                tfac = 10,            // time scale factor
                r = sqrt(xn*xn + yn*yn)/t / tfac;

    float z;
    if (r > M_PI)
        z = 0;
    else
        z = sin(r);

    NCURSES_PAIRS_T c = lrintf(z*(w->wn - 1));
    if (y == w->Y/2) {
        short i = x - w->title_left;
        if (i >= 0 &&
            i < w->title_pitch * title_len &&
            !(i % w->title_pitch))
            return c + w->to; // use title offset
    }
    return c + w->wo; // use wave offset
}

static void wave_explode(const Wave *w) {
    // Radially symmetrical, piecewise sine
    // Increasing the frequency any further causes page tearing.
    const float fmax = 20,        // max update freq, Hz
                tmin = 1/fmax,    // min update period, s
                trun = 3;         // total run time, s

    // Otherwise lrintf won't behave like we expect
    assert_b(FLT_ROUNDS == FLT_ROUNDS_NEAREST, "support float rounding");

    struct timespec start;
    assert_c(clock_gettime(CLOCK_MONOTONIC, &start), "get time");

    float t;
    do {
        struct timespec frame_start;
        assert_c(clock_gettime(CLOCK_MONOTONIC, &frame_start), "get time");
        t = time_delta(start, frame_start)/trun;

        for (NCURSES_SIZE_T y = 0; y < w->Y; y++) {
            for (NCURSES_SIZE_T x = 0; x < w->X; x++) {
                NCURSES_PAIRS_T c = wave_c_from_xy(y, x, t, w);
                assert_n(mvwchgat(w->win, y, x, 1, A_NORMAL, c, NULL),
                         "change colour");
            }
        }
        assert_n(wrefresh(w->win), "refresh");

        struct timespec frame_mid;
        assert_c(clock_gettime(CLOCK_MONOTONIC, &frame_mid), "get time");
        float remain = tmin - time_delta(frame_start, frame_mid);
        if (remain > 0)
            assert_c(usleep(1e6*remain), "usleep");
    } while (t < 1);
}

void wave() {
    // Draw an animated wave emanating from the center outwards, and filling
    // from black to a blue pattern

    // Offsets and counts for point, wave and title graphics
    Wave w = {.win = view.win,
              .Y = getmaxy(view.win),
              .X = getmaxx(view.win)};
    wave_palette(&w);

    wave_point(&w);
    noise(&w);

    show_title(&w);
    wave_explode(&w);
}
