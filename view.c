#include <float.h>     // FLT_ROUNDS
#include <locale.h>    // setlocale
#include <math.h>      // sin, sqrt, etc.
#include <sys/param.h> // MIN, MAX
#include <time.h>      // clock_gettime
#include <unistd.h>    // usleep
#include <ncursesw/ncurses.h>

#include "error.h"
#include "view.h"

#define INVIS_CUR 0   // For curs_set

// Character aspect ratio. This is basically a guess
#define ASPECT 0.5

// See https://www.gnu.org/software/libc/manual/html_node/Floating-Point-Parameters.html
#define FLT_ROUNDS_NEAREST 1

struct {
    WINDOW *win;
} static view;


static float timeDelta(struct timespec t1, struct timespec t2) {
    return t2.tv_sec - t1.tv_sec + 1e-9*(t2.tv_nsec - t1.tv_nsec);
}

void view_init() {
    // Set entire locale based on environment
    // Recommended by man ncurses(3X)
    assert_p(setlocale(LC_ALL, ""), "set the locale");

    view.win = initscr();
    assert_p(view.win, "start ncurses");

    assert_n(start_color(), "enable colour mode");
    assert_b(has_colors(), "check color support");
    assert_b(COLOR_PAIRS >= 256, "support enough colour pairs");
    assert_b(COLORS >= 256, "support enough colours");

    assert_n(cbreak(), "disable buffering for immediate input");
    assert_n(noecho(), "disable user input echo");
    assert_n(nonl(), "disable newline translation");
    assert_n(curs_set(INVIS_CUR), "hide the cursor");
    assert_n(keypad(view.win, true), "enable translation of arrow keys");
}

void view_destroy() {
    if (!isendwin())
        check_n(endwin(), "end ncurses");
}

static void wave_palette(NCURSES_PAIRS_T *po, int *pn,
                         NCURSES_PAIRS_T *wo, int *wn) {
    // Black through blue through white
    const int N = 2*NC - 1; // number of colours
    NCURSES_COLOR_T scale[N];
    for (int i = 0; i < NC; i++)
        scale[i] = RGB2P(0, 0, i);
    for (int i = NC; i < N; i++)
        scale[i] = RGB2P(i-NC+1, i-NC+1, NC-1);

    // Set up pairs for "point"
    *po = 1; // point pair offset
    *pn = N; // point pair count
    for (int i = 0; i < *pn; i++) {
        NCURSES_COLOR_T b = scale[0], // back
                        f = scale[i]; // fore
        NCURSES_PAIRS_T p = i + *po;  // pair ID
        assert_n(init_pair(p, f, b), "init pair");
    }

    // Set up pairs for "wave"
    const int off = 1; // offset between fore and back
    *wo = *po + *pn;   // wave pair offset
    *wn = N - off;     // wave pair count
    for (int i = 0; i < *wn; i++) {
        NCURSES_COLOR_T b = scale[i],     // back
                        f = scale[i+off]; // fore
        NCURSES_PAIRS_T p = i + *wo;      // pair ID
        assert_n(init_pair(p, f, b), "init pair");
    }
}

static void wave_point(NCURSES_PAIRS_T po, int pn, int Y, int X) {
    const int ym = Y/2, xm = X/2;

    for (int c = 0; c < pn; c++) {
        assert_n(wattron(view.win, COLOR_PAIR(c + po)), "switch colour");
        assert_n(mvwaddch(view.win, ym, xm, '.'), "output char");
        assert_n(wrefresh(view.win), "refresh");
        assert_c(usleep(200000), "usleep");
    }
}

static void wave_explode(NCURSES_PAIRS_T wo, int wn, int Y, int X) {
    // Radially symmetrical, piecewise sine
    const float fmax = 30,        // max update freq
                tmin = 1/fmax,    // min update period
                trun = 6,         // total run time, s
                tfac = 16,        // time scale factor
                size = MIN(X, Y), // smallest screen dimension
                final = 0.3;      // final z level after wave settle

    struct timespec start, prev;
    assert_c(clock_gettime(CLOCK_MONOTONIC, &start), "get time");
    prev = start;

    // Otherwise lrintf won't behave like we expect
    assert_b(FLT_ROUNDS == FLT_ROUNDS_NEAREST, "support float rounding");

    float t;
    do {
        t = timeDelta(start, prev)/trun;

        for (int y = 0; y < Y; y++) {
            // Normalize coordinates, center, correct for aspect ratio
            float yn = (y - Y/2.)/size / ASPECT;
            for (int x = 0; x < X; x++) {
                float xn = (x - X/2.)/size,
                      r = sqrt(xn*xn + yn*yn)/t / tfac,
                      z;
                if (r > M_PI)
                    z = 0;
                else {
                    z = sin(r);
                    if (r < M_PI_2)
                        z = z*(1 - final) + final;
                }

                NCURSES_PAIRS_T c = wo + lrintf(z*(wn - 1));

                assert_n(mvwchgat(view.win, y, x, 1, A_NORMAL, c, NULL),
                         "change colour");
            }
        }
        assert_n(wrefresh(view.win), "refresh");

        struct timespec now;
        assert_c(clock_gettime(CLOCK_MONOTONIC, &now), "get time");
        float remain = tmin - timeDelta(prev, now);
        if (remain > 0)
            assert_c(usleep(1e6*remain), "usleep");
        prev = now;

    } while (t < 1);
}

static void wave() {
    // Draw an animated wave emanating from the center outwards, and filling
    // from black to a grey pattern

    NCURSES_PAIRS_T po, wo; // point and wave offsets
    int pn, wn;             // point and wave counts
    wave_palette(&po, &pn, &wo, &wn);

    int Y, X;
    getmaxyx(view.win, Y, X);

    wave_point(po, pn, Y, X);
    wave_explode(wo, wn, Y, X);
}

void view_splash() {
    wave();
}