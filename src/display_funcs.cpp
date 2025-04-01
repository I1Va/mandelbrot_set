#include <math.h>
#include <immintrin.h>

#include "mandelbrot_set/display_funcs.h"
#include "TXLib.h"

void update_display_info(display_into_t *display_info) {
    assert(display_info);

    if (GetAsyncKeyState(VK_ESCAPE)) {
        display_info->terminate_state = true;
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_LEFT)) {
        display_info->dx -= CORD_DELTA;
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_RIGHT)) {
        display_info->dx += CORD_DELTA;
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_UP)) {
        display_info->dy -= CORD_DELTA;
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_DOWN)) {
        display_info->dy += CORD_DELTA;
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_Z)) {
        display_info->scale *= SCALE_COEF;
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_X)) {
        display_info->scale /= SCALE_COEF;
        txSleep(KEY_PROC_SLEEP);
    }
}

display_into_t display_init(tx_window_info_t tx_window_info, const double scale, const double x_origin, const double y_origin) {

    display_into_t display_info = {};

    display_info.tx_window_info = tx_window_info;

    display_info.scale      = scale;
    display_info.x_origin   = x_origin;
    display_info.y_origin   = y_origin;

    return display_info;
}

tx_window_info_t create_tx_window(const int screen_width, const int screen_height) {
    tx_window_info_t tx_window_info = {};
    txCreateWindow(screen_width, screen_height);

    tx_window_info.screen_width = screen_width;
    tx_window_info.screen_height = screen_height;
    tx_window_info.video_mem = (void *) txVideoMemory();

    return tx_window_info;
}

void display_without_optimizations(display_into_t *display_info) {
    assert(display_info);

    for (int iy = 0; iy < display_info->tx_window_info.screen_height; iy++) {
        for (int ix = 0; ix < display_info->tx_window_info.screen_width; ix++) {
            double x0 = ((double) ix + display_info->dx * (double) (display_info->tx_window_info.screen_height) / 2.0f + display_info->x_origin)
                        * display_info->scale;
            double y0 = ((double) iy + display_info->dy * (double) (display_info->tx_window_info.screen_width) / 2.0f + display_info->y_origin)
                        * display_info->scale;
            double x = 0;
            double y = 0;
            double x2 = 0;
            double y2 = 0;
            double xy = 0;

            size_t iters = 0;
            while (iters < MAX_ITERATIONS_CNT && x2 + y2 < (double) STABLE_RADIUS) {
                x2 = x * x;
                y2 = y * y;
                xy = x * y;

                x = x2 - y2 + x0;
                y = 2 * xy + y0;

                iters++;
            }

            double red_coef     = 0.1f + (double) iters * 0.03f * 0.2f;
            double green_coef   = 0.2f + (double) iters * 0.03f * 0.3f;
            double blue_coef    = 0.3f + (double) iters * 0.03f * 0.1f;

            RGBQUAD rgb =
            {   (BYTE) (red_coef   * 255),
                (BYTE) (green_coef * 255),
                (BYTE) (blue_coef  * 255)
            };

            *((RGBQUAD*) (display_info->tx_window_info.video_mem) + (ix + (display_info->tx_window_info.screen_height - iy - 1) * display_info->tx_window_info.screen_width)) = rgb;
        }
    }
}

void display_with_array_optimization(display_into_t *display_info) {
    assert(display_info);

    for (int iy = 0; iy < display_info->tx_window_info.screen_height; iy++) {
        for (int ix = 0; ix < display_info->tx_window_info.screen_width; ix+=4) {
            double x0 = ((double) ix + display_info->dx * (double) (display_info->tx_window_info.screen_height) / 2.0f + display_info->x_origin)
                        * display_info->scale;
            double x1 = ((double) (ix + 1) + display_info->dx * (double) (display_info->tx_window_info.screen_height) / 2.0f + display_info->x_origin)
                        * display_info->scale;
            double x2 = ((double) (ix + 2) + display_info->dx * (double) (display_info->tx_window_info.screen_height) / 2.0f + display_info->x_origin)
                        * display_info->scale;
            double x3 = ((double) (ix + 3) + display_info->dx * (double) (display_info->tx_window_info.screen_height) / 2.0f + display_info->x_origin)
                        * display_info->scale;

            double y0 = ((double) iy + display_info->dy * (double) (display_info->tx_window_info.screen_width) / 2.0f + display_info->y_origin)
                        * display_info->scale;

            double x0_arr4[4] = {x0, x1, x2, x3};
            double y0_arr4[4] = {y0, y0, y0, y0};

            double x_arr4[4]         = {0, 0, 0, 0};
            double y_arr4[4]         = {0, 0, 0, 0};
            double x2_arr4[4]        = {0, 0, 0, 0};
            double y2_arr4[4]        = {0, 0, 0, 0};
            double xy_arr4[4]        = {0, 0, 0, 0};
            double abs_arr4[4]       = {0, 0, 0, 0};

            int iters_arr4[4]       = {0, 0, 0, 0};
            int dots_state_arr4[4]  = {1, 1, 1, 1};


            int iters = 0;

            while (iters < MAX_ITERATIONS_CNT) {
                for (int i = 0; i < 4; i++) {
                    x2_arr4[i] = x_arr4[i] * x_arr4[i];
                    y2_arr4[i] = y_arr4[i] * y_arr4[i];
                    abs_arr4[i] = x2_arr4[i] + y2_arr4[i];
                }

                for (int i = 0; i < 4; i++) {
                    xy_arr4[i] = x_arr4[i] * y_arr4[i];
                }

                for (int i = 0; i < 4; i++) {
                    if (abs_arr4[i] > STABLE_RADIUS + 10) {
                        dots_state_arr4[i] = 0;
                        continue;
                    }
                    x_arr4[i] = x2_arr4[i] - y2_arr4[i] + x0_arr4[i];
                    y_arr4[i] = 2 * xy_arr4[i] + y0_arr4[i];
                }

                for (int i = 0; i < 4; i++) {
                    dots_state_arr4[i] = (int) (abs_arr4[i] <= STABLE_RADIUS);
                }

                int mask = 0;
                for (int i = 0; i < 4; i++) {
                    mask =  (mask | (dots_state_arr4[i] << i));
                }

                if ((!mask)) {
                    break;
                }

                for (int i = 0; i < 4; i++) {
                    iters_arr4[i] += dots_state_arr4[i];
                }
                iters++;
            }

            for (int i = 0; i < 4; i++) {
                double red_coef     = 0.1f + (double) (iters_arr4[i]) * 0.03f * 0.2f;
                double green_coef   = 0.2f + (double) (iters_arr4[i]) * 0.03f * 0.3f;
                double blue_coef    = 0.3f + (double) (iters_arr4[i]) * 0.03f * 0.1f;

                RGBQUAD rgb =
                {   (BYTE) (red_coef * 255),
                    (BYTE) (green_coef * 255),
                    (BYTE) (blue_coef * 255)
                };

                *((RGBQUAD*) (display_info->tx_window_info.video_mem) + (ix + i + (display_info->tx_window_info.screen_height - iy - 1) * display_info->tx_window_info.screen_width)) = rgb;
            }
        }
    }
}

void display_with_intrinsik_optimization(display_into_t *display_info) {
    assert(display_info);

    float stable_radius_arr4[] = {STABLE_RADIUS, STABLE_RADIUS, STABLE_RADIUS, STABLE_RADIUS};
    __m128 stable_radius_vec4 = _mm_load_ps(stable_radius_arr4);

    for (int iy = 0; iy < display_info->tx_window_info.screen_height; iy++) {
        for (int ix = 0; ix < display_info->tx_window_info.screen_width; ix+=4) {
            float x0 = ((float) ix + display_info->dx * (float) (display_info->tx_window_info.screen_height) / 2.0f + display_info->x_origin)
                        * (float) display_info->scale;
            float x1 = ((float) (ix + 1) + display_info->dx * (float) (display_info->tx_window_info.screen_height) / 2.0f + display_info->x_origin)
                        * (float) display_info->scale;
            float x2 = ((float) (ix + 2) + display_info->dx * (float) (display_info->tx_window_info.screen_height) / 2.0f + display_info->x_origin)
                        * (float) display_info->scale;
            float x3 = ((float) (ix + 3) + display_info->dx * (float) (display_info->tx_window_info.screen_height) / 2.0f + display_info->x_origin)
                        * (float) display_info->scale;

            float y0 = ((float) iy + display_info->dy * (float) (display_info->tx_window_info.screen_width) / 2.0f + display_info->y_origin)
                        * (float) display_info->scale;

            __m128 x0_vec4 = _mm_set_ps(x3, x2, x1, x0);
            __m128 y0_vec4 = _mm_set_ps(y0, y0, y0, y0);
            __m128 x_vec4  = _mm_set_ps(0, 0, 0, 0);
            __m128 y_vec4  = _mm_set_ps(0, 0, 0, 0);

            __m128 x2_vec4  = _mm_set_ps(0, 0, 0, 0);
            __m128 y2_vec4  = _mm_set_ps(0, 0, 0, 0);
            __m128 xy_vec4  = _mm_set_ps(0, 0, 0, 0);
            __m128 abs_vec4  = _mm_set_ps(0, 0, 0, 0);

            __m128i dots_states_vec4 = _mm_set_epi32(1, 1, 1, 1);
            __m128i iters_vec4       = _mm_set_epi32(0, 0, 0, 0);

            int iters = 0;

            while (iters < MAX_ITERATIONS_CNT) {
                x2_vec4 = _mm_mul_ps(x_vec4, x_vec4);
                y2_vec4 = _mm_mul_ps(y_vec4, y_vec4);
                abs_vec4 = _mm_add_ps(x2_vec4, y2_vec4);

                xy_vec4 = _mm_mul_ps(x_vec4, y_vec4);

                x2_vec4 = _mm_sub_ps(x2_vec4, y2_vec4); // x_arr4[i] = x2_arr4[i] - y2_arr4[i] + x0_arr4[i];
                x_vec4 = _mm_add_ps(x2_vec4, x0_vec4);

                xy_vec4 = _mm_add_ps(xy_vec4, xy_vec4); // y_arr4[i] = 2 * xy_arr4[i] + y0_arr4[i];
                y_vec4 = _mm_add_ps(xy_vec4, y0_vec4);

                __m128 cmp_res_vec4 = _mm_cmp_ps(abs_vec4, stable_radius_vec4, _CMP_LE_OS); // dots_state_arr4[i] = (int) (abs_arr4[i] <= STABLE_RADIUS);
                dots_states_vec4 = _mm_castps_si128(cmp_res_vec4);

                if (_mm_testc_si128(dots_states_vec4, dots_states_vec4)) { // if dots_states_vec4 == 0
                    break;
                }

                iters_vec4 = _mm_add_epi32(iters_vec4, dots_states_vec4);
                iters++;
            }


            int32_t iters_arr4[4] = {};
            iters_arr4[0] = _mm_extract_epi32(iters_vec4, 0);
            iters_arr4[1] = _mm_extract_epi32(iters_vec4, 1);
            iters_arr4[2] = _mm_extract_epi32(iters_vec4, 2);
            iters_arr4[3] = _mm_extract_epi32(iters_vec4, 3);

            for (int i = 0; i < 4; i++) {
                float red_coef     = 0.1f + (float) (iters_arr4[i]) * 0.03f * 0.2f;
                float green_coef   = 0.2f + (float) (iters_arr4[i]) * 0.03f * 0.3f;
                float blue_coef    = 0.3f + (float) (iters_arr4[i]) * 0.03f * 0.1f;

                RGBQUAD rgb =
                {   (BYTE) (red_coef * 255),
                    (BYTE) (green_coef * 255),
                    (BYTE) (blue_coef * 255)
                };

                *((RGBQUAD*) (display_info->tx_window_info.video_mem) + (ix + i + (display_info->tx_window_info.screen_height - iy - 1) * display_info->tx_window_info.screen_width)) = rgb;
            }
        }
    }
}

