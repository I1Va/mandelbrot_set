#include <TXLib.h>
#include <math.h>
#include <immintrin.h>

#include "mandelbrot_set/display_funcs.h"

#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))

void move_cords(calc_info_t *calc_info, int dx, int dy) {
    assert(calc_info);

    calc_info->offset_x += dx * calc_info->scale;
    calc_info->offset_y += dy * calc_info->scale;
}

void zoom_cords(calc_info_t *calc_info, double scale, int anch_x, int anch_y) {
    assert(calc_info);

    double real_x = calc_info->offset_x + anch_x * calc_info->scale;
    double real_y = calc_info->offset_y + anch_y * calc_info->scale;

    double new_scale = calc_info->scale * scale;

    calc_info->offset_x = real_x - anch_x * new_scale;
    calc_info->offset_y = real_y - anch_y * new_scale;

    calc_info->scale = new_scale;
}

void update_display_info(calc_info_t *calc_info) {
    assert(calc_info);

    if (GetAsyncKeyState(VK_ESCAPE)) {
        calc_info->terminate_state = true;
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_LEFT)) {
        move_cords(calc_info, -CORD_DELTA, 0);
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_RIGHT)) {
        move_cords(calc_info, CORD_DELTA, 0);
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_UP)) {
        move_cords(calc_info, 0, -CORD_DELTA);
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_DOWN)) {
        move_cords(calc_info, 0, CORD_DELTA);
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_Z)) {
        zoom_cords(calc_info, SCALE_COEF, calc_info->screen_width / 2, calc_info->screen_height / 2);
        txSleep(KEY_PROC_SLEEP);
    }
    if (GetAsyncKeyState(VK_X)) {
        zoom_cords(calc_info, 1 / SCALE_COEF, calc_info->screen_width / 2, calc_info->screen_height / 2);
        txSleep(KEY_PROC_SLEEP);
    }
}

calc_info_t display_init(tx_window_info_t tx_window_info, const double scale, const double offset_x, const double offset_y, int screen_width, int screen_height) {

    calc_info_t calc_info = {};

    calc_info.tx_window_info = tx_window_info;

    calc_info.scale          = scale;
    calc_info.offset_x       = offset_x;
    calc_info.offset_y       = offset_y;
    calc_info.screen_width   = screen_width;
    calc_info.screen_height  = screen_height;

    return calc_info;
}

tx_window_info_t create_tx_window(const int screen_width, const int screen_height) {
    tx_window_info_t tx_window_info = {};
    txCreateWindow(screen_width, screen_height);
    tx_window_info.video_mem = (void *) txVideoMemory();

    return tx_window_info;
}

void normalize_color(color_t *color) {
    assert(color);

    color->red     = min(color->red, MAX_COLOR_VAL);
    color->green   = min(color->green, MAX_COLOR_VAL);
    color->blue    = min(color->blue, MAX_COLOR_VAL);

    color->red     = max(color->red, MIN_COLOR_VAL);
    color->green   = max(color->green, MIN_COLOR_VAL);
    color->blue    = max(color->blue, MIN_COLOR_VAL);
}

color_t default_color_func(int iterations) {
    color_t color = {};

    color.red     = (0.1f + (float) (iterations) * 0.03f * 0.2f) * MAX_COLOR_VAL;
    color.green   = (0.2f + (float) (iterations) * 0.03f * 0.3f) * MAX_COLOR_VAL;
    color.blue    = (0.3f + (float) (iterations) * 0.03f * 0.1f) * MAX_COLOR_VAL;

    return color;
}

color_t color_func_1(int iterations) {
    color_t color = {};

    color.red     = (float) (iterations % 255);
    color.green   = (float) (255 - iterations % 255);
    color.blue    = (float) (iterations) * 0.2f * 255;

    return color;
}

void display(calc_info_t *calc_info, int *iters_matrix, color_function_t color_func) {
    assert(calc_info);
    assert(iters_matrix);

    for (int iy = 0; iy < calc_info->screen_height; iy++) {
        for (int ix = 0; ix < calc_info->screen_width; ix++) {
            int iterations = *(iters_matrix + (ix + (calc_info->screen_height - iy - 1) * calc_info->screen_width));

            color_t color = color_func(iterations);
            normalize_color(&color);

            RGBQUAD rgb =
            {   (BYTE) (color.red),
                (BYTE) (color.green),
                (BYTE) (color.blue)
            };

            *((RGBQUAD*) (calc_info->tx_window_info.video_mem) + (ix + (calc_info->screen_height - iy - 1) * calc_info->screen_width)) = rgb;
        }
    }
}

void calc_without_optimizations(calc_info_t *calc_info, int *iters_matrix) {
    assert(calc_info);
    assert(iters_matrix);

    for (int iy = 0; iy < calc_info->screen_height; iy++) {
        double y0 = calc_info->offset_y + iy * calc_info->scale;

        for (int ix = 0; ix < calc_info->screen_width; ix++) {
            double x0 = calc_info->offset_x + ix * calc_info->scale;
            double x = 0;
            double y = 0;
            double x2 = 0;
            double y2 = 0;
            double xy = 0;

            int iters = 0;
            while (iters < MAX_ITERATIONS_CNT && x2 + y2 < STABLE_RADIUS) {
                x2 = x * x;
                y2 = y * y;
                xy = x * y;

                x = x2 - y2 + x0;
                y = 2 * xy + y0;

                iters++;
            }

            *(iters_matrix + (ix + (calc_info->screen_height - iy - 1) * calc_info->screen_width)) = iters;
        }
    }
}

#define ARR_SZ 4
void calc_with_array_optimization(calc_info_t *calc_info, int *iters_matrix) {
    assert(calc_info);
    assert(iters_matrix);

    for (int iy = 0; iy < calc_info->screen_height; iy++) {
        float y0 = (float) (calc_info->offset_y + iy * calc_info->scale);

        for (int ix = 0; ix < calc_info->screen_width; ix+=ARR_SZ) {
            float x0 = (float) (calc_info->offset_x + ix * calc_info->scale);
            float dx = (float) calc_info->scale;


            float x0_arr[ARR_SZ] = {};
            float y0_arr[ARR_SZ] = {};
            float x_arr[ARR_SZ] = {};
            float y_arr[ARR_SZ] = {};
            float x2_arr[ARR_SZ] = {};
            float y2_arr[ARR_SZ] = {};
            float xy_arr[ARR_SZ] = {};
            float abs_arr[ARR_SZ] = {};

            float stable_radius_arr[ARR_SZ] = {};
            int iters_arr[ARR_SZ] = {};
            int dots_state_arr[ARR_SZ] = {};

            for (int i = 0; i < ARR_SZ; i++) {
                x0_arr[i] = (float) (x0 + (float) (i) * dx);
                y0_arr[i] = y0;
                dots_state_arr[i] = 1;
                stable_radius_arr[i] = STABLE_RADIUS;
            }


            int iters = 0;
            while (iters < MAX_ITERATIONS_CNT) {
                for (int i = 0; i < ARR_SZ; i++) {
                    x2_arr[i] = x_arr[i] * x_arr[i];
                    y2_arr[i] = y_arr[i] * y_arr[i];
                    abs_arr[i] = x2_arr[i] + y2_arr[i];
                }

                for (int i = 0; i < ARR_SZ; i++) {
                    xy_arr[i] = x_arr[i] * y_arr[i];
                }

                for (int i = 0; i < ARR_SZ; i++) {
                    x_arr[i] = x2_arr[i] - y2_arr[i] + x0_arr[i];
                    y_arr[i] = 2 * xy_arr[i] + y0_arr[i];
                }

                for (int i = 0; i < ARR_SZ; i++) {
                    x_arr[i] = min(x_arr[i], stable_radius_arr[i]);
                    y_arr[i] = min(y_arr[i], stable_radius_arr[i]);
                }

                for (int i = 0; i < ARR_SZ; i++) {
                    dots_state_arr[i] = (int) (abs_arr[i] <= STABLE_RADIUS);
                }

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
                int x_pixel = min(ix + i, calc_info->screen_width - 1);
                int y_pixel = (calc_info->screen_height - iy - 1);
                *(iters_matrix + x_pixel + y_pixel * calc_info->screen_width) = iters_arr[i];
            }
        }
    }
}

const __m256 stable_radius_vec8 = _mm256_set1_ps(STABLE_RADIUS);
void calc_with_intrinsic_optimization(calc_info_t *calc_info, int *iters_matrix) {
    assert(calc_info);
    assert(iters_matrix);

    for (int iy = 0; iy < calc_info->screen_height; iy++) {
        float y0 = (float)(calc_info->offset_y + iy * calc_info->scale);
        __m256 y0_vec8 = _mm256_set1_ps(y0);

        for (int ix = 0; ix < calc_info->screen_width; ix += 8) {
            float x0_base = (float)(calc_info->offset_x + ix * calc_info->scale);
            float dx = (float)calc_info->scale;

            __m256 x0_vec8 = _mm256_set_ps(
                x0_base + 7 * dx, x0_base + 6 * dx, x0_base + 5 * dx, x0_base + 4 * dx,
                x0_base + 3 * dx, x0_base + 2 * dx, x0_base + 1 * dx, x0_base
            );

            __m256 x_vec8 = _mm256_setzero_ps();
            __m256 y_vec8 = _mm256_setzero_ps();
            __m256 x2_vec8 = _mm256_setzero_ps();
            __m256 y2_vec8 = _mm256_setzero_ps();
            __m256 xy_vec8 = _mm256_setzero_ps();
            __m256 abs_vec8 = _mm256_setzero_ps();

            __m256i dots_states_vec8 = _mm256_set1_epi32(1);
            __m256i iters_vec8 = _mm256_setzero_si256();

            int iters = 0;

            while (iters < MAX_ITERATIONS_CNT) {
                x2_vec8 = _mm256_mul_ps(x_vec8, x_vec8);
                y2_vec8 = _mm256_mul_ps(y_vec8, y_vec8);
                abs_vec8 = _mm256_add_ps(x2_vec8, y2_vec8);

                xy_vec8 = _mm256_mul_ps(x_vec8, y_vec8);

                x2_vec8 = _mm256_sub_ps(x2_vec8, y2_vec8);
                x_vec8 = _mm256_add_ps(x2_vec8, x0_vec8);
                xy_vec8 = _mm256_add_ps(xy_vec8, xy_vec8);
                y_vec8 = _mm256_add_ps(xy_vec8, y0_vec8);

                x_vec8 = _mm256_min_ps(x_vec8, stable_radius_vec8);
                y_vec8 = _mm256_min_ps(y_vec8, stable_radius_vec8);


                __m256 cmp_res_vec8 = _mm256_cmp_ps(abs_vec8, stable_radius_vec8, _CMP_LT_OS);
                dots_states_vec8 = _mm256_castps_si256(cmp_res_vec8);

                if (_mm256_testz_si256(dots_states_vec8, dots_states_vec8)) {
                    break;
                }

                iters_vec8 = _mm256_sub_epi32(iters_vec8, dots_states_vec8);
                iters++;
            }

            int iters_arr8[8];
            _mm256_storeu_si256((__m256i*)iters_arr8, iters_vec8);

            for (int i = 0; i < 8; i++) {
                int x_pixel = min(ix + i, calc_info->screen_width - 1);
                int y_pixel = (calc_info->screen_height - iy - 1);
                *(iters_matrix + x_pixel + y_pixel * calc_info->screen_width) = iters_arr8[i];
            }
        }
    }
}

void calc_with_intrinsic_optimization_unroll_2(calc_info_t *calc_info, int *iters_matrix) {
    assert(calc_info);
    assert(iters_matrix);

    for (int iy = 0; iy < calc_info->screen_height; iy++) {
        float y0 = (float)(calc_info->offset_y + iy * calc_info->scale);
        __m256 y0_vec8 = _mm256_set1_ps(y0);
        __m256 y0_vec8_2 = _mm256_set1_ps(y0);

        for (int ix = 0; ix < calc_info->screen_width; ix += 16) {
            float x0_base = (float)(calc_info->offset_x + ix * calc_info->scale);
            float dx = (float)calc_info->scale;

            __m256 x0_vec8 = _mm256_set_ps(
                x0_base + 7 * dx, x0_base + 6 * dx, x0_base + 5 * dx, x0_base + 4 * dx,
                x0_base + 3 * dx, x0_base + 2 * dx, x0_base + 1 * dx, x0_base
            );
            __m256 x0_vec8_2 = _mm256_set_ps(
                x0_base + 15 * dx, x0_base + 14 * dx, x0_base + 13 * dx, x0_base + 12 * dx,
                x0_base + 11 * dx, x0_base + 10 * dx, x0_base + 9 * dx, x0_base + 8 * dx
            );

            __m256 x_vec8 = _mm256_setzero_ps();
            __m256 y_vec8 = _mm256_setzero_ps();
            __m256 x2_vec8 = _mm256_setzero_ps();
            __m256 y2_vec8 = _mm256_setzero_ps();
            __m256 xy_vec8 = _mm256_setzero_ps();
            __m256 abs_vec8 = _mm256_setzero_ps();
            __m256i dots_states_vec8 = _mm256_set1_epi32(1);
            __m256i iters_vec8 = _mm256_setzero_si256();

            __m256 x_vec8_2 = _mm256_setzero_ps();
            __m256 y_vec8_2 = _mm256_setzero_ps();
            __m256 x2_vec8_2 = _mm256_setzero_ps();
            __m256 y2_vec8_2 = _mm256_setzero_ps();
            __m256 xy_vec8_2 = _mm256_setzero_ps();
            __m256 abs_vec8_2 = _mm256_setzero_ps();
            __m256i dots_states_vec8_2 = _mm256_set1_epi32(1);
            __m256i iters_vec8_2 = _mm256_setzero_si256();

            int iters = 0;

            while (iters < MAX_ITERATIONS_CNT) {
                x2_vec8 = _mm256_mul_ps(x_vec8, x_vec8);
                y2_vec8 = _mm256_mul_ps(y_vec8, y_vec8);
                abs_vec8 = _mm256_add_ps(x2_vec8, y2_vec8);
                xy_vec8 = _mm256_mul_ps(x_vec8, y_vec8);
                x2_vec8 = _mm256_sub_ps(x2_vec8, y2_vec8);
                x_vec8 = _mm256_add_ps(x2_vec8, x0_vec8);
                xy_vec8 = _mm256_add_ps(xy_vec8, xy_vec8);
                y_vec8 = _mm256_add_ps(xy_vec8, y0_vec8);
                x_vec8 = _mm256_min_ps(x_vec8, stable_radius_vec8);
                y_vec8 = _mm256_min_ps(y_vec8, stable_radius_vec8);

                x2_vec8_2 = _mm256_mul_ps(x_vec8_2, x_vec8_2);
                y2_vec8_2 = _mm256_mul_ps(y_vec8_2, y_vec8_2);
                abs_vec8_2 = _mm256_add_ps(x2_vec8_2, y2_vec8_2);
                xy_vec8_2 = _mm256_mul_ps(x_vec8_2, y_vec8_2);
                x2_vec8_2 = _mm256_sub_ps(x2_vec8_2, y2_vec8_2);
                x_vec8_2 = _mm256_add_ps(x2_vec8_2, x0_vec8_2);
                xy_vec8_2 = _mm256_add_ps(xy_vec8_2, xy_vec8_2);
                y_vec8_2 = _mm256_add_ps(xy_vec8_2, y0_vec8_2);
                x_vec8_2 = _mm256_min_ps(x_vec8_2, stable_radius_vec8);
                y_vec8_2 = _mm256_min_ps(y_vec8_2, stable_radius_vec8);


                __m256 cmp_res_vec8 = _mm256_cmp_ps(abs_vec8, stable_radius_vec8, _CMP_LT_OS);
                dots_states_vec8 = _mm256_castps_si256(cmp_res_vec8);

                __m256 cmp_res_vec8_2 = _mm256_cmp_ps(abs_vec8_2, stable_radius_vec8, _CMP_LT_OS);
                dots_states_vec8_2 = _mm256_castps_si256(cmp_res_vec8_2);

                if (_mm256_testz_si256(dots_states_vec8, dots_states_vec8) &&
                    _mm256_testz_si256(dots_states_vec8_2, dots_states_vec8_2)) {
                    break;
                }

                iters_vec8 = _mm256_sub_epi32(iters_vec8, dots_states_vec8);
                iters_vec8_2 = _mm256_sub_epi32(iters_vec8_2, dots_states_vec8_2);
                iters++;
            }

            int iters_arr8[8] = {};
            int iters_arr8_2[8] = {};

            _mm256_storeu_si256((__m256i*)iters_arr8, iters_vec8);
            _mm256_storeu_si256((__m256i*)iters_arr8_2, iters_vec8_2);

            for (int i = 0; i < 8; i++) {
                int x_pixel = min(ix + i, calc_info->screen_width - 1);
                int y_pixel = (calc_info->screen_height - iy - 1);
                int x_pixel_2 = min(ix + i + 8, calc_info->screen_width - 1);
                int y_pixel_2 = (calc_info->screen_height - iy - 1);

                *(iters_matrix + x_pixel + y_pixel * calc_info->screen_width) = iters_arr8[i];
                *(iters_matrix + x_pixel_2 + y_pixel_2 * calc_info->screen_width) = iters_arr8_2[i];
            }
        }
    }
}
