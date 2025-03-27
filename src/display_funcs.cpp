#include "mandelbrot_set/display_funcs.h"
#include "TXLib.h"

void update_display_info( display_into_t *display_info) {
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

void display( display_into_t *display_info) {
    assert(display_info);

    for (int iy = 0; iy < display_info->tx_window_info.screen_height; iy++) {
        for (int ix = 0; ix < display_info->tx_window_info.screen_width; ix++) {
            float x0 = ((float) ix + display_info->dx * (float) (display_info->tx_window_info.screen_height) / 2.0f + display_info->x_origin)
                        * display_info->scale;
            float y0 = ((float) iy + display_info->dy * (float) (display_info->tx_window_info.screen_width) / 2.0f + display_info->y_origin)
                        * display_info->scale;

            float x = 0;
            float y = 0;
            float x2 = 0;
            float y2 = 0;
            float xy = 0;

            size_t iters = 0;
            while (iters < MAX_ITERATIONS_CNT && x2 + y2 < (float) STABLE_RADIUS) {
                x2 = x * x;
                y2 = y * y;
                xy = x * y;

                x = x2 - y2 + x0;
                y = 2 * xy + y0;

                iters++;
            }

            float red_coef     = 0.1f + (float) iters * 0.03f * 0.2f;
            float green_coef   = 0.2f + (float) iters * 0.03f * 0.3f;
            float blue_coef    = 0.3f + (float) iters * 0.03f * 0.1f;

            RGBQUAD rgb =
            {   (BYTE) (red_coef   * 255),
                (BYTE) (green_coef * 255),
                (BYTE) (blue_coef  * 255)
            };

            *((RGBQUAD*) (display_info->tx_window_info.video_mem) + (ix + (display_info->tx_window_info.screen_height - iy - 1) * display_info->tx_window_info.screen_width)) = rgb;
        }
    }
}

display_into_t display_init(tx_window_info_t tx_window_info, const float scale, const float x_origin, const float y_origin) {

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

