#include <math.h>

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

void print_arr(size_t n, double *a) {
    printf("arr : ");
    for (int i = 0; i < n; i++) {
        printf("%f ", a[i]);
    }
    printf("\n");
}


void display_with_array_optimization(display_into_t *display_info) {
    assert(display_info);

    for (int iy = 0; iy < display_info->tx_window_info.screen_height; iy++) {
        for (int ix = 0; ix < display_info->tx_window_info.screen_width; ix+=4) { // FIXME: width может не быть кратна 4
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