#ifndef DISPPAY_FUNCS_H
#define DISPPAY_FUNCS_H

#include <string.h>
#include <immintrin.h>

const int MAX_ITERATIONS_CNT = 500;

const double STABLE_RADIUS = 16;
const int    COLOR_MAX_VAL = 255;

const int VK_Z = 0x5A;
const int VK_X = 0x58;

const double KEY_PROC_SLEEP = 100.0;
const double SCALE_COEF = 0.86f;
const double SCALE_DEFAULT = 1.0f / 180.0f;

const double CORD_DELTA = 0.5f;

typedef struct {
    int screen_width;
    int screen_height;
    void *video_mem;
} tx_window_info_t;

typedef struct {
    double dx;
    double dy;

    double scale;

    bool terminate_state;

    tx_window_info_t tx_window_info;

    double x_origin;
    double y_origin;
} display_info_t;

void update_display_info(display_info_t *display_info);
display_info_t display_init(tx_window_info_t tx_window_info, const double scale, const double x_origin, const double y_origin);
tx_window_info_t create_tx_window(const int screen_width, const int screen_height);

void put_canvas_dot(display_info_t *display_info, int ix, int iy, int iterations);

void display_without_optimizations(display_info_t *display_info);
void display_with_array_optimization(display_info_t *display_info);
void display_with_intrinsic_optimization(display_info_t *display_info);

#endif // DISPPAY_FUNCS_H