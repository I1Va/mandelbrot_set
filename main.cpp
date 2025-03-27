#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include "TXLib.h"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

const int SCREEN_WIDTH_HALF = SCREEN_WIDTH / 2;
const int SCREEN_HEIGHT_HALF = SCREEN_HEIGHT / 2;

const size_t MAX_ITERATIONS_CNT = 500;
const float  STABLE_RADIUS = 16;
const int    COLOR_MAX_VAL = 255;

const float  X_ORIGIN = - ((float) SCREEN_WIDTH_HALF);
const float  Y_ORIGIN = - ((float) SCREEN_HEIGHT_HALF);

const int VK_Z = 0x5A;
const int VK_X = 0x58;

const double KEY_PROC_SLEEP = 100.0;
const float SCALE_COEF = 0.8f;
const float SCALE_DEFAULT = 1.0f / 180.0f;

const float CORD_DELTA = 0.5f;

struct display_into_t {
    float dx;
    float dy;
    float scale;

    bool terminate_state;
};

void display(struct display_into_t *display_info);
void update_display_info(struct display_into_t *display_info);

void display(struct display_into_t *display_info) {
    assert(display_info);

    POINT screen_sz = txGetExtent();

    for (int iy = 0; iy < SCREEN_HEIGHT; iy++) {
        for (int ix = 0; ix < SCREEN_WIDTH; ix++) {
            float x0 = ((float) ix + display_info->dx * SCREEN_WIDTH_HALF + X_ORIGIN) * display_info->scale;
            float y0 = ((float) iy + display_info->dy * SCREEN_HEIGHT_HALF + Y_ORIGIN) * display_info->scale;

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

            *(txVideoMemory() + (ix + (screen_sz.y - iy - 1) * screen_sz.x)) = rgb;
        }
    }
}

void update_display_info(struct display_into_t *display_info) {
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

int main () {
    struct display_into_t display_info = {0, 0, SCALE_DEFAULT, 0};

    txCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT);

    for (;;) {
        update_display_info(&display_info);
        printf ("FPS %lf\t\t\r", txGetFPS());

        if (display_info.terminate_state) {
            break;
        }

        display(&display_info);
    }

    return 0;
}
