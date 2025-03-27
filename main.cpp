#include <math.h>
#include "TXLib.h"

const size_t SCREEN_WIDTH = 1000;
const size_t SCREEN_HEIGHT = 600;

const size_t MAX_ITERATIONS_CNT = 500;
const float STABLE_RADIUS = 16;
const int    COLOR_MAX_VAL = 255;

void display() {
    float dx = 0;
    float dy = 0;
    float x_origin = - ((float) SCREEN_WIDTH) / 2;
    float y_origin = - ((float) SCREEN_HEIGHT) / 2;
    float scale = 1.0 / 180;

    for (int iy = 0; iy < SCREEN_HEIGHT; iy++) {

        for (int ix = 0; ix < SCREEN_WIDTH; ix++) {
            float x0 = (ix - 400.0 * dx + x_origin) * scale;
            float y0 = (iy - 600.0 * dy + y_origin) * scale;

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

            float red_coef     = 0.1 + iters * 0.03 * 0.2;
            float green_coef   = 0.2 + iters * 0.03 * 0.3;
            float blue_coef    = 0.3 + iters * 0.03 * 0.1;

            COLORREF color = RGB(red_coef * COLOR_MAX_VAL, green_coef * COLOR_MAX_VAL, blue_coef * COLOR_MAX_VAL);
            txSetPixel(ix, iy, color);
        }
    }
}

int main () {
    txCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
    for (;;) {
        display();
    }
    return 0;
}
