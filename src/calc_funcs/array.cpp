#include <assert.h>
#include "mandelbrot_set/calc_funcs.h"
#include "mandelbrot_set/general.h"

#define ARR_SZ 8
#define ALIGN_VALUE 256

#define DOT_ARRAYS_INIALIZE(align_val, x0_arr, y0_arr, x_arr, y_arr, x2_arr, y2_arr, xy_arr, abs_arr)   \
    float x0_arr[ARR_SZ]    __attribute__ ((aligned (align_val))) = {};                                 \
    float y0_arr[ARR_SZ]    __attribute__ ((aligned (align_val))) = {};                                 \
    float x_arr[ARR_SZ]     __attribute__ ((aligned (align_val))) = {};                                 \
    float y_arr[ARR_SZ]     __attribute__ ((aligned (align_val))) = {};                                 \
    float x2_arr[ARR_SZ]    __attribute__ ((aligned (align_val))) = {};                                 \
    float y2_arr[ARR_SZ]    __attribute__ ((aligned (align_val))) = {};                                 \
    float xy_arr[ARR_SZ]    __attribute__ ((aligned (align_val))) = {};                                 \
    float abs_arr[ARR_SZ]   __attribute__ ((aligned (align_val))) = {};                                 \

#define DOT_ARRAYS_CALCULATE(x0_arr, y0_arr, x_arr, y_arr, x2_arr, y2_arr, xy_arr, abs_arr) \
    for (int i = 0; i < ARR_SZ; i++) {                                                      \
        x2_arr[i] = x_arr[i] * x_arr[i];                                                    \
        y2_arr[i] = y_arr[i] * y_arr[i];                                                    \
        abs_arr[i] = x2_arr[i] + y2_arr[i];                                                 \
    }                                                                                       \
                                                                                            \
    for (int i = 0; i < ARR_SZ; i++) {                                                      \
        xy_arr[i] = x_arr[i] * y_arr[i];                                                    \
    }                                                                                       \
                                                                                            \
    for (int i = 0; i < ARR_SZ; i++) {                                                      \
        x_arr[i] = x2_arr[i] - y2_arr[i] + x0_arr[i];                                       \
        y_arr[i] = 2 * xy_arr[i] + y0_arr[i];                                               \
    }                                                                                       \
                                                                                            \
    for (int i = 0; i < ARR_SZ; i++) {                                                      \
        x_arr[i] = min(x_arr[i], stable_radius_arr[i]);                                     \
        y_arr[i] = min(y_arr[i], stable_radius_arr[i]);                                     \
    }                                                                                       \
                                                                                            \
    for (int i = 0; i < ARR_SZ; i++) {                                                      \
        dots_state_arr[i] = (int) (abs_arr[i] <= STABLE_RADIUS);                            \
    }

void calc_with_array_optimization(calc_info_t *calc_info, int *iters_matrix) {
    assert(calc_info);
    assert(iters_matrix);

    for (int iy = 0; iy < calc_info->screen_height; iy++) {
        float y0 = (float) (calc_info->offset_y + iy * calc_info->scale);
        for (int ix = 0; ix < calc_info->screen_width; ix+=ARR_SZ) {
            float x0 = (float) (calc_info->offset_x + ix * calc_info->scale);
            float dx = (float) calc_info->scale;

            DOT_ARRAYS_INIALIZE(ALIGN_VALUE, x0_arr, y0_arr, x_arr, y_arr, x2_arr, y2_arr, xy_arr, abs_arr)

            float stable_radius_arr[ARR_SZ] __attribute__ ((aligned (ALIGN_VALUE))) = {};
            int iters_arr[ARR_SZ]           __attribute__ ((aligned (ALIGN_VALUE))) = {};
            int dots_state_arr[ARR_SZ]      __attribute__ ((aligned (ALIGN_VALUE))) = {};

            for (int i = 0; i < ARR_SZ; i++) {
                x0_arr[i] = (float) (x0 + (float) (i) * dx);
                y0_arr[i] = y0;
                dots_state_arr[i] = 1;
                stable_radius_arr[i] = STABLE_RADIUS;
            }

            int iters = 0;
            while (iters < MAX_ITERATIONS_CNT) {
                DOT_ARRAYS_CALCULATE(x0_arr, y0_arr, x_arr, y_arr, x2_arr, y2_arr, xy_arr, abs_arr)

                long long mask = 0;
                for (int i = 0; i < ARR_SZ; i++) {
                    mask =  (mask | (((long long) dots_state_arr[i]) << i));
                }

                if ((!mask)) {
                    break;
                }

                for (int i = 0; i < ARR_SZ; i++) {
                    iters_arr[i] += dots_state_arr[i];
                }
                iters++;
            }

            for (int i = 0; i < ARR_SZ; i++) {
                int x_pixel = ix + i;
                int y_pixel = (calc_info->screen_height - iy - 1);
                *(iters_matrix + x_pixel + y_pixel * calc_info->screen_width) = iters_arr[i];
            }
        }
    }
}

#undef ARR_SZ
#undef ALIGN_VALUE