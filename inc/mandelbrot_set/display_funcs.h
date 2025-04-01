#ifndef DISPPAY_FUNCS_H
#define DISPPAY_FUNCS_H

#include <string.h>
#include <immintrin.h>

const int MAX_ITERATIONS_CNT = 500;

const double STABLE_RADIUS = 16;
const int    COLOR_MAX_VAL = 255;

const int VK_Z = 0x5A;
const int VK_X = 0x58;

const double KEY_PROC_SLEEP = 50.0;
const double SCALE_COEF = 0.80f;
const double SCALE_DEFAULT = 1.0f / 180.0f;

const double CORD_DELTA = 100;

typedef struct {
    int screen_width;
    int screen_height;
    void *video_mem;
} tx_window_info_t;

typedef struct {
    bool terminate_state;

    tx_window_info_t tx_window_info;

    double offset_x, offset_y;
    double scale;

} display_info_t;

typedef void (*display_function_t) (display_info_t *display_info, bool draw_enable);

void move_cords(display_info_t *display_info, int dx, int dy);
void zoom_cords(display_info_t *display_info, double scale, int anch_x, int anch_y);

void update_display_info(display_info_t *display_info);
display_info_t display_init(tx_window_info_t tx_window_info, const double scale, const double x_origin, const double y_origin);
tx_window_info_t create_tx_window(const int screen_width, const int screen_height);

void put_canvas_dot(display_info_t *display_info, int ix, int iy, int iterations);

void display_without_optimizations(display_info_t *display_info, bool draw_enable);
void display_with_array_optimization(display_info_t *display_info, bool draw_enable);
void display_with_intrinsic_optimization(display_info_t *display_info, bool draw_enable);

#endif // DISPPAY_FUNCS_H