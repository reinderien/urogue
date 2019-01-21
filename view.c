#include <float.h>     // FLT_ROUNDS
#include <locale.h>    // setlocale
#include <math.h>      // sin, sqrt, etc.
#include <stdlib.h>    // rand
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


static float time_delta(struct timespec t1, struct timespec t2) {
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

static void noise(NCURSES_SIZE_T Y, NCURSES_SIZE_T X) {
    // Fill screen with random characters
    assert_n(wmove(view.win, 0, 0), "move cursor home");

    const char cmin = '!', cmax = '~';

    for (int i = 0; i < X*Y - 1; i++) {
        char c = cmin + rand()%(cmax - cmin + 1);
        assert_n(waddch(view.win, c), "output char");
    }
}

static void show_title(const char *title, int titlesz,
    NCURSES_SIZE_T Y, NCURSES_SIZE_T X, NCURSES_SIZE_T *spots) {
    const NCURSES_SIZE_T width = X/3,
                         pitch = width/titlesz,
                         y = Y/2,
                         xleft = (X - (titlesz - 1)*pitch)/2;

    for (int i = 0; i < titlesz; i++) {
        NCURSES_SIZE_T x = xleft + i*pitch;
        spots[i] = x;
        assert_n(mvwaddch(view.win, y, x, title[i]), "output title");
    }
}

static void wave_palette(NCURSES_PAIRS_T *po, NCURSES_PAIRS_T *pn,
                         NCURSES_PAIRS_T *wo, NCURSES_PAIRS_T *wn,
                         NCURSES_PAIRS_T *to, NCURSES_PAIRS_T *tn) {
    // Black through blue through white
    const int N = 2*NC - 1; // number of colours
    NCURSES_COLOR_T scale[N];
    for (int i = 0; i < NC; i++)
        scale[i] = RGB2P(0, 0, i);
    for (int i = NC; i < N; i++)
        scale[i] = RGB2P(i-NC+1, i-NC+1, NC-1);

    // Set up pairs for "point": varying foreground, black background
    *po = 1; // point pair offset
    *pn = N; // point pair count
    for (NCURSES_PAIRS_T i = 0; i < *pn; i++)
        assert_n(init_pair(i + *po, scale[i], scale[0]), "init pair");

    // Set up pairs for "wave": varying foreground, dimmer background
    const NCURSES_PAIRS_T off = 1; // offset between fore and back
    *wo = *po + *pn;               // wave pair offset
    *wn = N - off;                 // wave pair count
    for (NCURSES_PAIRS_T i = 0; i < *wn; i++)
        assert_n(init_pair(i + *wo, scale[i+off], scale[i]), "init pair");

    // Set up pairs for "title": white foreground, varying background
    *to = *wo + *wn;
    *tn = N;
    for (NCURSES_PAIRS_T i = 0; i < *tn; i++)
        assert_n(init_pair(i + *to, scale[N-1], scale[i]), "init pair");
}

static void wave_point(NCURSES_PAIRS_T po, NCURSES_PAIRS_T pn,
                       NCURSES_SIZE_T Y, NCURSES_SIZE_T X) {
    const NCURSES_SIZE_T ym = Y/2, xm = X/2;

    for (NCURSES_PAIRS_T c = 0; c < pn; c++) {
        assert_n(wattron(view.win, COLOR_PAIR(c + po)), "switch colour");
        assert_n(mvwaddch(view.win, ym, xm, '.'), "output char");
        assert_n(wrefresh(view.win), "refresh");
        assert_c(usleep(100000), "usleep");
    }
}

static void wave_explode(NCURSES_PAIRS_T wo, NCURSES_PAIRS_T wn,
                         NCURSES_PAIRS_T to, NCURSES_PAIRS_T tn,
                         const NCURSES_SIZE_T *spots, int titlesz,
                         NCURSES_SIZE_T Y, NCURSES_SIZE_T X) {
    // Radially symmetrical, piecewise sine
    // Increasing the frequency any further causes page tearing.
    const float fmax = 20,        // max update freq, Hz
                tmin = 1/fmax,    // min update period, s
                trun = 3,         // total run time, s
                tfac = 10,        // time scale factor
                size = MAX(X, Y); // largest screen dimension in chars

    // Otherwise lrintf won't behave like we expect
    assert_b(FLT_ROUNDS == FLT_ROUNDS_NEAREST, "support float rounding");

    struct timespec start;
    assert_c(clock_gettime(CLOCK_MONOTONIC, &start), "get time");

    float t;
    do {
        struct timespec frame_start;
        assert_c(clock_gettime(CLOCK_MONOTONIC, &frame_start), "get time");
        t = time_delta(start, frame_start)/trun;

        for (NCURSES_SIZE_T y = 0; y < Y; y++) {
            // Normalize coordinates, center, correct for aspect ratio
            float yn = (y - Y/2.)/size / ASPECT;
            for (NCURSES_SIZE_T x = 0; x < X; x++) {
                float xn = (x - X/2.)/size,
                      r = sqrt(xn*xn + yn*yn)/t / tfac,
                      z;
                if (r > M_PI)
                    z = 0;
                else
                    z = sin(r);

                NCURSES_PAIRS_T c = lrintf(z*(wn - 1));
                if (y == Y/2) {
                    for (int i = 0;;) {
                        if (spots[i] == x) {
                            c += to; // use title offset
                            break;
                        }
                        if (++i >= titlesz) {
                            c += wo; // non-title; use wave offset
                            break;
                        }
                    }
                }
                else c += wo;

                assert_n(mvwchgat(view.win, y, x, 1, A_NORMAL, c, NULL),
                         "change colour");
            }
        }
        assert_n(wrefresh(view.win), "refresh");

        struct timespec frame_mid;
        assert_c(clock_gettime(CLOCK_MONOTONIC, &frame_mid), "get time");
        float remain = tmin - time_delta(frame_start, frame_mid);
        if (remain > 0)
            assert_c(usleep(1e6*remain), "usleep");

    } while (t < 1);
}

static void wave() {
    // Draw an animated wave emanating from the center outwards, and filling
    // from black to a blue pattern

    // Offsets and counts for point, wave and title graphics
    NCURSES_PAIRS_T po, wo, to, pn, wn, tn;
    wave_palette(&po, &pn, &wo, &wn, &to, &tn);

    const NCURSES_SIZE_T Y = getmaxy(view.win),
                         X = getmaxx(view.win);

    wave_point(po, pn, Y, X);
    noise(Y, X);

    const char title[] = "UROGUE";
    const int titlesz = sizeof(title) - 1;
    NCURSES_SIZE_T spots[titlesz];
    show_title(title, titlesz, Y, X, spots);

    wave_explode(wo, wn, to, tn, spots, titlesz, Y, X);
}

void view_splash() {
    wave();
}