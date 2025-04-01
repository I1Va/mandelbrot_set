#include <math.h>
#include <immintrin.h>

#include "mandelbrot_set/display_funcs.h"
#include "TXLib.h"

__m128 stable_radius_vec4 = _mm_set1_ps((float) STABLE_RADIUS * 2);


void cords_scale(display_info_t *display_info, double delta_scale) {
    assert(display_info);

    double old_scale = display_info->scale;
    double new_scale = old_scale * delta_scale;

    int screen_width = display_info->tx_window_info.screen_width;
    int screen_height = display_info->tx_window_info.screen_height;


    int     anchor_pixel_x = screen_width / 2;
    int     anchor_pixel_y = screen_height / 2;

    double  anchor_real_x = (anchor_pixel_x + display_info->dx * screen_width) * old_scale + display_info->x_origin;
    double  anchor_real_y = (anchor_pixel_y + display_info->dy * screen_height) * old_scale + display_info->y_origin;

    display_info->scale = new_scale;

    display_info->x_origin = anchor_real_x - (anchor_pixel_x + display_info->dx * screen_width) * new_scale;
    display_info->y_origin = anchor_real_y - (anchor_pixel_y + display_info->dy * screen_height) * new_scale;
}

void update_display_info(display_info_t *display_info) {
    assert(display_info);

    if (GetAsyncKeyState(VK_ESCAPE)) {
        display_info->terminate_state = true;
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_LEFT)) {
        display_info->x_origin -= CORD_DELTA * display_info->scale;
        display_info->dx -= CORD_DELTA;
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_RIGHT)) {
        display_info->x_origin += CORD_DELTA * display_info->scale;
        display_info->dx += CORD_DELTA;
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_UP)) {
        display_info->y_origin -= CORD_DELTA * display_info->scale;
        display_info->dy -= CORD_DELTA;
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_DOWN)) {
        display_info->y_origin = CORD_DELTA * display_info->scale;
        display_info->dy += CORD_DELTA;
        txSleep(KEY_PROC_SLEEP);
    }

    if (GetAsyncKeyState(VK_Z)) {
        cords_scale(display_info, SCALE_COEF);
        txSleep(KEY_PROC_SLEEP);
    }
    if (GetAsyncKeyState(VK_X)) {
        cords_scale(display_info, 1 / SCALE_COEF);
        txSleep(KEY_PROC_SLEEP);
    }

}

display_info_t display_init(tx_window_info_t tx_window_info, const double scale, const double x_origin, const double y_origin) {

    display_info_t display_info = {};

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

    *((RGBQUAD*) (display_info->tx_window_info.video_mem) + (ix + (display_info->tx_window_info.screen_height - iy - 1) * display_info->tx_window_info.screen_width)) = rgb;
}

void display_without_optimizations(display_info_t *display_info) {
    assert(display_info);

    for (int iy = 0; iy < display_info->tx_window_info.screen_height; iy++) {
        double y0 = (iy + display_info->dy * display_info->tx_window_info.screen_width) * display_info->scale + display_info->y_origin;

        for (int ix = 0; ix < display_info->tx_window_info.screen_width; ix++) {
            double x0 = (ix + display_info->dx * display_info->tx_window_info.screen_height) * display_info->scale + display_info->x_origin;

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

            put_canvas_dot(display_info, ix, iy, iters);
        }
    }
}

void display_with_array_optimization(display_info_t *display_info) {
    assert(display_info);

    for (int iy = 0; iy < display_info->tx_window_info.screen_height; iy++) {
        double y0 = (iy + display_info->dy * display_info->tx_window_info.screen_width) * display_info->scale + display_info->y_origin;

        for (int ix = 0; ix < display_info->tx_window_info.screen_width; ix+=4) {
            double x0 = (ix + display_info->dx * display_info->tx_window_info.screen_height) * display_info->scale + display_info->x_origin;
            double dx = display_info->scale;

            double x0_arr4[4] = {x0, x0 + dx, x0 + 2 * dx, x0 + 3 * dx};
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
                put_canvas_dot(display_info, ix, iy, iters_arr4[i]);
            }
        }
    }
}

void display_with_intrinsic_optimization(display_info_t *display_info) {
    assert(display_info);
    for (int iy = 0; iy < display_info->tx_window_info.screen_height; iy++) {
        float y0 = (float) ((iy + display_info->dy * display_info->tx_window_info.screen_width) * display_info->scale + display_info->y_origin);
        __m128 y0_vec4 = _mm_set1_ps(y0);

        for (int ix = 0; ix < display_info->tx_window_info.screen_width; ix+=4) {
            float x0 = (float) ((ix + display_info->dx * display_info->tx_window_info.screen_height) * display_info->scale + display_info->x_origin);
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
                put_canvas_dot(display_info, ix + i, iy, iters_arr4[i]);
            }
        }
    }
}
