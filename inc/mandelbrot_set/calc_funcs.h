#ifndef CALC_FUNCS_H
#define CALC_FUNCS_H

const int MAX_ITERATIONS_CNT = 255;
const double STABLE_RADIUS = 16;

const double SCALE_COEF = 0.80f;
const double SCALE_DEFAULT = 1.0f / 180.0f;
const double CORD_DELTA = 100;

typedef struct {
    double offset_x, offset_y;
    double scale;

    int screen_width;
    int screen_height;
} calc_info_t;

typedef void (*calc_function_t) (calc_info_t *calc_info, int *iters_matrix);

void calc_without_optimizations(calc_info_t *calc_info, int *iters_matrix);
void calc_with_array_optimization(calc_info_t *calc_info, int *iters_matrix);
void calc_with_intrinsic_optimization(calc_info_t *calc_info, int *iters_matrix);
void calc_with_intrinsic_optimization_unroll_2(calc_info_t *calc_info, int *iters_matrix);

#endif // CALC_FUNCS_H