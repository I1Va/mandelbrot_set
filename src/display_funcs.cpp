#include <TXLib.h>
#include <math.h>
#include <immintrin.h>

#include "mandelbrot_set/display_funcs.h"

#define min(a, b)  (((a) < (b)) ? (a) : (b))

__m128 stable_radius_vec4 = _mm_set1_ps((float) STABLE_RADIUS * 2);
const float stable_radius_arr4[4] = {STABLE_RADIUS, STABLE_RADIUS, STABLE_RADIUS, STABLE_RADIUS};

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

void display_with_array_optimization(display_info_t *display_info, bool draw_enable) {
    assert(display_info);

    for (int iy = 0; iy < display_info->screen_height; iy++) {
        float y0 = (float) (display_info->offset_y + iy * display_info->scale);

        for (int ix = 0; ix < display_info->screen_width; ix+=4) {
            float x0 = (float) (display_info->offset_x + ix * display_info->scale);
            float dx = (float) display_info->scale;

            float x0_arr4[4] = {x0, x0 + dx, x0 + 2 * dx, x0 + 3 * dx};
            float y0_arr4[4] = {y0, y0, y0, y0};

            float x_arr4[4]         = {0, 0, 0, 0};
            float y_arr4[4]         = {0, 0, 0, 0};
            float x2_arr4[4]        = {0, 0, 0, 0};
            float y2_arr4[4]        = {0, 0, 0, 0};
            float xy_arr4[4]        = {0, 0, 0, 0};
            float abs_arr4[4]       = {0, 0, 0, 0};

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
                    x_arr4[i] = x2_arr4[i] - y2_arr4[i] + x0_arr4[i];
                    y_arr4[i] = 2 * xy_arr4[i] + y0_arr4[i];
                }

                for (int i = 0; i < 4; i++) {
                    x_arr4[i] = min(x_arr4[i], stable_radius_arr4[i]);
                    y_arr4[i] = min(y_arr4[i], stable_radius_arr4[i]);
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
                if (draw_enable) {
                    put_canvas_dot(display_info, ix + i, iy, iters_arr4[i]);
                }

            }
        }
    }
}

void display_with_intrinsic_optimization(display_info_t *display_info, bool draw_enable) {
    assert(display_info);
    for (int iy = 0; iy < display_info->screen_height; iy++) {
        float y0 = (float) (display_info->offset_y + iy * display_info->scale);

        __m128 y0_vec4 = _mm_set1_ps(y0);

        for (int ix = 0; ix < display_info->screen_width; ix+=4) {
            float x0 = (float) (display_info->offset_x + ix * display_info->scale);
            float dx = (float) display_info->scale;

            __m128 x0_vec4 = _mm_set_ps(x0 + 3 * dx, x0 + 2 * dx, x0 + dx, x0);

            __m128 x_vec4  = _mm_set1_ps(0);
            __m128 y_vec4  = _mm_set1_ps(0);

            __m128 x2_vec4  = _mm_set1_ps(0);
            __m128 y2_vec4  = _mm_set1_ps(0);
            __m128 xy_vec4  = _mm_set1_ps(0);
            __m128 abs_vec4 = _mm_set1_ps(0);

            __m128i dots_states_vec4 = _mm_set1_epi32(1);
            __m128i iters_vec4       = _mm_set1_epi32(0);

            int iters_arr4[4] = {};
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


                x_vec4 = _mm_min_ps (x_vec4, stable_radius_vec4);
                y_vec4 = _mm_min_ps (y_vec4, stable_radius_vec4);

                __m128 cmp_res_vec4 = _mm_cmp_ps(abs_vec4, stable_radius_vec4, _CMP_LT_OS); // dots_state_arr4[i] = (int) (abs_arr4[i] <= STABLE_RADIUS);
                dots_states_vec4 = _mm_castps_si128(cmp_res_vec4);

                if (_mm_testz_si128(dots_states_vec4, dots_states_vec4)) { // if dots_states_vec4 == 0
                    break;
                }

                iters_vec4 = _mm_sub_epi32(iters_vec4, dots_states_vec4);
                iters++;
            }

            iters_arr4[0] = _mm_cvtsi128_si32(_mm_shuffle_epi32(iters_vec4, _MM_SHUFFLE(0, 0, 0, 0)));
            iters_arr4[1] = _mm_cvtsi128_si32(_mm_shuffle_epi32(iters_vec4, _MM_SHUFFLE(0, 0, 0, 1)));
            iters_arr4[2] = _mm_cvtsi128_si32(_mm_shuffle_epi32(iters_vec4, _MM_SHUFFLE(0, 0, 0, 2)));
            iters_arr4[3] = _mm_cvtsi128_si32(_mm_shuffle_epi32(iters_vec4, _MM_SHUFFLE(0, 0, 0, 3)));

            for (int i = 0; i < 4; i++) {
                if (draw_enable) {
                    put_canvas_dot(display_info, ix + i, iy, iters_arr4[i]);
                }
            }
        }
    }
}
