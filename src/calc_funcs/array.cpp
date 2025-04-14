#include <assert.h>
#include "mandelbrot_set/calc_funcs.h"
#include "mandelbrot_set/general.h"

#define ARR_SZ 8
#define ALIGN_VALUE 256

inline static void calc_square_float_array(float sq_arr[], float arr[], size_t arr_sz) {
    for (size_t i = 0; i < arr_sz; i++) {
        sq_arr[i] = arr[i] * arr[i];
    }
}

inline static void calc_sum_float_array(float sum_arr[], float arr1[], float arr2[], size_t arr_sz) {
    for (size_t i = 0; i < arr_sz; i++) {
        sum_arr[i] = arr1[i] + arr2[i];
    }
}

inline static void calc_sum_int_array(int sum_arr[], int arr1[], int arr2[], size_t arr_sz) {
    for (size_t i = 0; i < arr_sz; i++) {
        sum_arr[i] = arr1[i] + arr2[i];
    }
}

inline static void calc_mult_float_array(float sum_arr[], float arr1[], float arr2[], size_t arr_sz) {
    for (size_t i = 0; i < arr_sz; i++) {
        sum_arr[i] = arr1[i] * arr2[i];
    }
}

inline static void fill_float_array(float arr[], const float val, const size_t arr_sz) {
    for (size_t i = 0; i < arr_sz; i++) {
        arr[i] = val;
    }
}

inline static void fill_int_array(int arr[], const int val, const size_t arr_sz) {
    for (size_t i = 0; i < arr_sz; i++) {
        arr[i] = val;
    }
}

inline static void fill_x0_array(float x0_arr[], const float x0, const float dx, const size_t arr_sz) {
    for (size_t i = 0; i < arr_sz; i++) {
        x0_arr[i] = (float) (x0 + (float) (i) * dx);
    }
}

inline static void calc_min_float_array(float min_arr[], float arr1[], float arr2[], size_t arr_sz) {
    for (size_t i = 0; i < arr_sz; i++) {
        min_arr[i] = (((arr1[i]) < (arr2[i])) ? (arr1[i]) : (arr2[i]));
    }
}

inline static void write_dot_iters_array_to_matrix(const calc_info_t *calc_info, int *iters_matrix,
                                                    const int ix, const int iy, const int iters_arr[], const size_t sz) {

    int y_offset = (calc_info->screen_height - iy - 1) * calc_info->screen_width;
    memcpy(iters_matrix + ix + y_offset, iters_arr, sz * sizeof(int));
}

inline static bool check_zero_array(const int arr[], const size_t arr_sz) {
    long long mask = 0;
    for (int i = 0; i < arr_sz; i++) {
        mask =  (mask | (((long long) arr[i]) << i));
    }

    return !mask;
}

void calc_with_array_optimization(const calc_info_t *calc_info, int *iters_matrix) {
    assert(calc_info);
    assert(iters_matrix);

    for (int iy = 0; iy < calc_info->screen_height; iy++) {
        float y0 = (float) (calc_info->offset_y + iy * calc_info->scale);
        for (int ix = 0; ix < calc_info->screen_width; ix+=ARR_SZ) {
            float x0 = (float) (calc_info->offset_x + ix * calc_info->scale);
            float dx = (float) calc_info->scale;

            alignas(ALIGN_VALUE) float x0_arr[ARR_SZ]               = {};
            alignas(ALIGN_VALUE) float y0_arr[ARR_SZ]               = {};
            alignas(ALIGN_VALUE) float x_arr[ARR_SZ]                = {};
            alignas(ALIGN_VALUE) float y_arr[ARR_SZ]                = {};
            alignas(ALIGN_VALUE) float x2_arr[ARR_SZ]               = {};
            alignas(ALIGN_VALUE) float y2_arr[ARR_SZ]               = {};
            alignas(ALIGN_VALUE) float xy_arr[ARR_SZ]               = {};
            alignas(ALIGN_VALUE) float abs_arr[ARR_SZ]              = {};

            alignas(ALIGN_VALUE) float stable_radius_arr[ARR_SZ]    = {};
            alignas(ALIGN_VALUE) int iters_arr[ARR_SZ]              = {};
            alignas(ALIGN_VALUE) int dots_state_arr[ARR_SZ]         = {};

            fill_x0_array(x0_arr, x0, dx, ARR_SZ);

            fill_float_array(y0_arr, y0, ARR_SZ);
            fill_int_array(dots_state_arr, 1, ARR_SZ);
            fill_float_array(stable_radius_arr, STABLE_RADIUS, ARR_SZ);

            int iters = 0;
            while (iters < MAX_ITERATIONS_CNT) {
                calc_square_float_array(x2_arr, x_arr, ARR_SZ);
                calc_square_float_array(y2_arr, y_arr, ARR_SZ);
                calc_sum_float_array(abs_arr, x2_arr, y2_arr, ARR_SZ);

                calc_mult_float_array(xy_arr, x_arr, y_arr, ARR_SZ);

                for (int i = 0; i < ARR_SZ; i++) { // complex array square calculation
                    x_arr[i] = x2_arr[i] - y2_arr[i] + x0_arr[i];
                    y_arr[i] = 2 * xy_arr[i] + y0_arr[i];
                }

                calc_min_float_array(x_arr, x_arr, stable_radius_arr, ARR_SZ);
                calc_min_float_array(y_arr, y_arr, stable_radius_arr, ARR_SZ);

                for (int i = 0; i < ARR_SZ; i++) {
                    dots_state_arr[i] = (int) (abs_arr[i] <= STABLE_RADIUS);
                }

                if (check_zero_array(dots_state_arr, ARR_SZ)) break;

                calc_sum_int_array(iters_arr, iters_arr, dots_state_arr, ARR_SZ);
                iters++;
            }

            write_dot_iters_array_to_matrix(calc_info, iters_matrix, ix, iy, iters_arr, ARR_SZ);
        }
    }
}

#undef ARR_SZ
#undef ALIGN_VALUE