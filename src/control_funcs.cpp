#include <TXLib.h>
#include <math.h>
#include <immintrin.h>

#include "mandelbrot_set/control_funcs.h"
#include "mandelbrot_set/general.h"

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

void update_calc_info(calc_info_t *calc_info, int *terminate_state) {
    assert(calc_info);
    assert(terminate_state);

    if (GetAsyncKeyState(VK_ESCAPE)) {
        *terminate_state = 1;
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

calc_info_t calc_info_init(const double scale, const double offset_x, const double offset_y, int screen_width, int screen_height) {
    calc_info_t calc_info = {};

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

void display(calc_info_t *calc_info, int *iters_matrix, color_function_t color_func, void *video_mem) {
    assert(calc_info);
    assert(iters_matrix);

    for (int iy = 0; iy < calc_info->screen_height; iy++) {
        for (int ix = 0; ix < calc_info->screen_width; ix++) {
            int iterations = *(iters_matrix + (ix + (calc_info->screen_height - iy - 1) * calc_info->screen_width));

            color_t color = color_func(iterations);

            RGBQUAD rgb =
            {   (BYTE) (color.red),
                (BYTE) (color.green),
                (BYTE) (color.blue)
            };

            *((RGBQUAD*) (video_mem) + (ix + (calc_info->screen_height - iy - 1) * calc_info->screen_width)) = rgb;
        }
    }
}

// FIXME: вынести функции в разные .cpp
// FIXME: пустые строки

// добавить align 'float x0_arr[ARR_SZ] = {};'
// FIXME: убрать пустые строки


