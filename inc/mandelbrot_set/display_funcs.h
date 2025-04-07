#ifndef DISPPAY_FUNCS_H
#define DISPPAY_FUNCS_H

#include <string.h>
#include <immintrin.h>

const int MAX_ITERATIONS_CNT = 500;

const double STABLE_RADIUS = 16;


const int VK_Z = 0x5A;
const int VK_X = 0x58;

const double KEY_PROC_SLEEP = 50.0;
const double SCALE_COEF = 0.80f;
const double SCALE_DEFAULT = 1.0f / 180.0f;

const double CORD_DELTA = 100;

const float MAX_COLOR_VAL = 255;
const float MIN_COLOR_VAL = 0;

typedef struct {
    void *video_mem;
} tx_window_info_t;

typedef struct {
    bool terminate_state;

    tx_window_info_t tx_window_info;

    double offset_x, offset_y;
    double scale;

    int screen_width;
    int screen_height;

} calc_info_t;

typedef struct {
    float red;
    float green;
    float blue;
} color_t;

typedef void (*calc_function_t) (calc_info_t *calc_info, int *iters_matrix);
typedef color_t (*color_function_t) (int iterations);

color_t default_color_func(int iterations);
color_t color_func_1(int iterations);

void normalize_color(color_t *color);

void move_cords(calc_info_t *calc_info, int dx, int dy);
void zoom_cords(calc_info_t *calc_info, double scale, int anch_x, int anch_y);

void update_display_info(calc_info_t *calc_info);
calc_info_t display_init(tx_window_info_t tx_window_info, const double scale,
    const double x_origin, const double y_origin, int screen_width, int screen_height);

tx_window_info_t create_tx_window(const int screen_width, const int screen_height);

void display(calc_info_t *calc_info, int *iters_matrix, color_function_t color_func);

void calc_without_optimizations(calc_info_t *calc_info, int *iters_matrix);
void calc_with_array_optimization(calc_info_t *calc_info, int *iters_matrix);
void calc_with_intrinsic_optimization(calc_info_t *calc_info, int *iters_matrix);
void calc_with_intrinsic_optimization_unroll_2(calc_info_t *calc_info, int *iters_matrix);

#endif // DISPPAY_FUNCS_H