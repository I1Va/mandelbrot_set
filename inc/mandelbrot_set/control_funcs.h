#ifndef CONTROL_FUNCS_H
#define CONTROL_FUNCS_H

#include <string.h>
#include "calc_funcs.h"

const int VK_Z = 0x5A;
const int VK_X = 0x58;

const double KEY_PROC_SLEEP = 50.0;

const float MAX_COLOR_VAL = 255;
const float MIN_COLOR_VAL = 0;

typedef struct {
    void *video_mem;
} tx_window_info_t;

typedef struct {
    float red;
    float green;
    float blue;
} color_t;

typedef color_t (*color_function_t) (int iterations);
color_t default_color_func(int iterations);
void normalize_color(color_t *color);
tx_window_info_t create_tx_window(const int screen_width, const int screen_height);

void display(calc_info_t *calc_info, int *iters_matrix, color_function_t color_func, void *video_mem);
void move_cords(calc_info_t *calc_info, int dx, int dy);
void zoom_cords(calc_info_t *calc_info, double scale, int anch_x, int anch_y);
void update_calc_info(calc_info_t *calc_info, int *terminate_state);
calc_info_t calc_info_init(const double scale,
    const double x_origin, const double y_origin, int screen_width, int screen_height);


#endif // CONTROL_FUNCS_H