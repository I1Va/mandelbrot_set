#include <TXLib.h>
#include <math.h>
#include <immintrin.h>

#include "mandelbrot_set/display_funcs.h"

#define min(a, b)  (((a) < (b)) ? (a) : (b))

void move_cords(display_info_t *display_info, int dx, int dy) {
    assert(display_info);

    display_info->offset_x += dx * display_info->scale;
    display_info->offset_y += dy * display_info->scale;
}

void zoom_cords(display_info_t *display_info, double scale, int anch_x, int anch_y) {
    assert(display_info);

    double real_x = display_info->offset_x + anch_x * display_info->scale;
    double real_y = display_info->offset_y + anch_y * display_info->scale;

    double new_scale = display_info->scale * scale;

    display_info->offset_x = real_x - anch_x * new_scale;
    display_info->offset_y = real_y - anch_y * new_scale;

    display_info->scale = new_scale;
}

void update_display_info(display_info_t *display_info) {
    assert(display_info);

    if (GetAsyncKeyState(VK_ESCAPE)) {
        display_info->terminate_state = true;
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_LEFT)) {
        move_cords(display_info, -CORD_DELTA, 0);
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_RIGHT)) {
        move_cords(display_info, CORD_DELTA, 0);
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_UP)) {
        move_cords(display_info, 0, -CORD_DELTA);
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_DOWN)) {
        move_cords(display_info, 0, CORD_DELTA);
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_Z)) {
        zoom_cords(display_info, SCALE_COEF, display_info->screen_width / 2, display_info->screen_height / 2);
        txSleep(KEY_PROC_SLEEP);
    }
    if (GetAsyncKeyState(VK_X)) {
        zoom_cords(display_info, 1 / SCALE_COEF, display_info->screen_width / 2, display_info->screen_height / 2);
        txSleep(KEY_PROC_SLEEP);
    }
}

display_info_t display_init(tx_window_info_t tx_window_info, const double scale, const double offset_x, const double offset_y, int screen_width, int screen_height) {

    display_info_t display_info = {};

    display_info.tx_window_info = tx_window_info;

    display_info.scale          = scale;
    display_info.offset_x       = offset_x;
    display_info.offset_y       = offset_y;
    display_info.screen_width   = screen_width;
    display_info.screen_height  = screen_height;

    return display_info;
}

tx_window_info_t create_tx_window(const int screen_width, const int screen_height) {
    tx_window_info_t tx_window_info = {};
    txCreateWindow(screen_width, screen_height);
    tx_window_info.video_mem = (void *) txVideoMemory();

    return tx_window_info;
}

void put_canvas_dot(display_info_t *display_info, int ix, int iy, int iterations) {
    assert(display_info);

    float red_coef     = 0.1f + (float) (iterations) * 0.03f * 0.2f;
    float green_coef   = 0.2f + (float) (iterations) * 0.03f * 0.3f;
    float blue_coef    = 0.3f + (float) (iterations) * 0.03f * 0.1f;

    RGBQUAD rgb =
    {   (BYTE) (red_coef * 255),
        (BYTE) (green_coef * 255),
        (BYTE) (blue_coef * 255)
    };

    *((RGBQUAD*) (display_info->tx_window_info.video_mem) + (ix + (display_info->screen_height - iy - 1) * display_info->screen_width)) = rgb;
}

void display_without_optimizations(display_info_t *display_info, bool draw_enable) {
    assert(display_info);

    for (int iy = 0; iy < display_info->screen_height; iy++) {
        double y0 = display_info->offset_y + iy * display_info->scale;

        for (int ix = 0; ix < display_info->screen_width; ix++) {
            double x0 = display_info->offset_x + ix * display_info->scale;

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
            if (draw_enable) {
                put_canvas_dot(display_info, ix, iy, iters);
            }
        }
    }
}

#define ARR_SZ 8
void display_with_array_optimization(display_info_t *display_info, bool draw_enable) {
    assert(display_info);

    for (int iy = 0; iy < display_info->screen_height; iy++) {
        float y0 = (float) (display_info->offset_y + iy * display_info->scale);

        for (int ix = 0; ix < display_info->screen_width; ix+=ARR_SZ) {
            float x0 = (float) (display_info->offset_x + ix * display_info->scale);
            float dx = (float) display_info->scale;


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
                x0_arr[i] = (float) (x0 + i * dx);
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
                if (draw_enable) {
                    put_canvas_dot(display_info, ix + i, iy, iters_arr[i]);
                }

            }
        }
    }
}

const __m256 stable_radius_vec8 = _mm256_set1_ps(STABLE_RADIUS);
void display_with_intrinsic_optimization(display_info_t *display_info, bool draw_enable) {
    assert(display_info);


    for (int iy = 0; iy < display_info->screen_height; iy++) {
        float y0 = (float)(display_info->offset_y + iy * display_info->scale);
        __m256 y0_vec8 = _mm256_set1_ps(y0);

        for (int ix = 0; ix < display_info->screen_width; ix += 8) {
            float x0_base = (float)(display_info->offset_x + ix * display_info->scale);
            float dx = (float)display_info->scale;

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
                if (ix + i >= display_info->screen_width) {
                    break;
                }
                if (draw_enable) {
                    put_canvas_dot(display_info, ix + i, iy, iters_arr8[i]);
                }
            }
        }
    }
}
