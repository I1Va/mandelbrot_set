#include <assert.h>
#include "mandelbrot_set/calc_funcs.h"
#include "mandelbrot_set/general.h"

#define INITIALIZE_DOUBLE_DOT_CORDS(x, y, x2, y2, xy)   \
    double x    = 0;                                    \
    double y    = 0;                                    \
    double x2   = 0;                                    \
    double y2   = 0;                                    \
    double xy   = 0;

#define CALCULATE_DOUBLE_DOT_CORDS(x, y, x2, y2, xy)    \
    x2  = x * x;                                        \
    y2  = y * y;                                        \
    xy  = x * y;                                        \
    x   = x2 - y2 + x0;                                 \
    y   = 2 * xy + y0;

void calc_without_optimizations(calc_info_t *calc_info, int *iters_matrix) {
    assert(calc_info);
    assert(iters_matrix);

    for (int iy = 0; iy < calc_info->screen_height; iy++) {
        double y0 = calc_info->offset_y + iy * calc_info->scale;
        for (int ix = 0; ix < calc_info->screen_width; ix++) {
            double x0 = calc_info->offset_x + ix * calc_info->scale;
            INITIALIZE_DOUBLE_DOT_CORDS(x, y, x2, y2, xy)

            int iters = 0;
            while (iters < MAX_ITERATIONS_CNT && x2 + y2 < STABLE_RADIUS) {
                CALCULATE_DOUBLE_DOT_CORDS(x, y, x2, y2, xy)
                iters++;
            }

            *(iters_matrix + (ix + (calc_info->screen_height - iy - 1) * calc_info->screen_width)) = iters;
        }
    }
}

#undef INITIALIZE_DOUBLE_DOT_CORDS