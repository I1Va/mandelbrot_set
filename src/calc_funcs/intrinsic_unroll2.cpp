#include <assert.h>
#include <immintrin.h>
#include "mandelbrot_set/calc_funcs.h"
#include "mandelbrot_set/general.h"

inline static void write_dot_iters_m256i_vector_to_matrix(const calc_info_t *calc_info, int *iters_matrix,
                                                    const int ix, const int iy, __m256i vec) {

    int y_offset = (calc_info->screen_height - iy - 1) * calc_info->screen_width;
    _mm256_store_si256((__m256i*) (iters_matrix + y_offset + ix), vec);
}

const __m256 stable_radius_vec8 = _mm256_set1_ps(STABLE_RADIUS);

void calc_with_intrinsic_optimization_unroll_2(const calc_info_t *calc_info, int *iters_matrix) {
    assert(calc_info);
    assert(iters_matrix);

    for (int iy = 0; iy < calc_info->screen_height; iy++) {
        float y0 = (float)(calc_info->offset_y + iy * calc_info->scale);
        __m256 y0_vec8 = _mm256_set1_ps(y0);
        __m256 y0_vec8_2 = _mm256_set1_ps(y0);

        for (int ix = 0; ix < calc_info->screen_width; ix += 16) {
            float x0_base = (float)(calc_info->offset_x + ix * calc_info->scale);
            float dx = (float)calc_info->scale;

            __m256 x0_vec8 = _mm256_set_ps(
                x0_base + 7 * dx, x0_base + 6 * dx, x0_base + 5 * dx, x0_base + 4 * dx,
                x0_base + 3 * dx, x0_base + 2 * dx, x0_base + 1 * dx, x0_base
            );
            __m256 x0_vec8_2 = _mm256_set_ps(
                x0_base + 15 * dx, x0_base + 14 * dx, x0_base + 13 * dx, x0_base + 12 * dx,
                x0_base + 11 * dx, x0_base + 10 * dx, x0_base + 9 * dx, x0_base + 8 * dx
            );

            __m256 x_vec8 = _mm256_setzero_ps();
            __m256 y_vec8 = _mm256_setzero_ps();
            __m256 x2_vec8 = _mm256_setzero_ps();
            __m256 y2_vec8 = _mm256_setzero_ps();
            __m256 xy_vec8 = _mm256_setzero_ps();
            __m256 abs_vec8 = _mm256_setzero_ps();
            __m256i dots_states_vec8 = _mm256_set1_epi32(1);
            __m256i iters_vec8 = _mm256_setzero_si256();

            __m256 x_vec8_2 = _mm256_setzero_ps();
            __m256 y_vec8_2 = _mm256_setzero_ps();
            __m256 x2_vec8_2 = _mm256_setzero_ps();
            __m256 y2_vec8_2 = _mm256_setzero_ps();
            __m256 xy_vec8_2 = _mm256_setzero_ps();
            __m256 abs_vec8_2 = _mm256_setzero_ps();
            __m256i dots_states_vec8_2 = _mm256_set1_epi32(1);
            __m256i iters_vec8_2 = _mm256_setzero_si256();

            int iters = 0;

            while (iters < MAX_ITERATIONS_CNT) {
                x2_vec8 = _mm256_mul_ps(x_vec8, x_vec8);
                y2_vec8 = _mm256_mul_ps(y_vec8, y_vec8);
                abs_vec8 = _mm256_add_ps(x2_vec8, y2_vec8);
                xy_vec8 = _mm256_mul_ps(x_vec8, y_vec8);
                x2_vec8 = _mm256_sub_ps(x2_vec8, y2_vec8);
                x_vec8 = _mm256_add_ps(x2_vec8, x0_vec8);
                xy_vec8 = _mm256_add_ps(xy_vec8, xy_vec8);
                y_vec8 = _mm256_add_ps(xy_vec8, y0_vec8);
                x_vec8 = _mm256_min_ps(x_vec8, stable_radius_vec8);
                y_vec8 = _mm256_min_ps(y_vec8, stable_radius_vec8);

                x2_vec8_2 = _mm256_mul_ps(x_vec8_2, x_vec8_2);
                y2_vec8_2 = _mm256_mul_ps(y_vec8_2, y_vec8_2);
                abs_vec8_2 = _mm256_add_ps(x2_vec8_2, y2_vec8_2);
                xy_vec8_2 = _mm256_mul_ps(x_vec8_2, y_vec8_2);
                x2_vec8_2 = _mm256_sub_ps(x2_vec8_2, y2_vec8_2);
                x_vec8_2 = _mm256_add_ps(x2_vec8_2, x0_vec8_2);
                xy_vec8_2 = _mm256_add_ps(xy_vec8_2, xy_vec8_2);
                y_vec8_2 = _mm256_add_ps(xy_vec8_2, y0_vec8_2);
                x_vec8_2 = _mm256_min_ps(x_vec8_2, stable_radius_vec8);
                y_vec8_2 = _mm256_min_ps(y_vec8_2, stable_radius_vec8);


                __m256 cmp_res_vec8 = _mm256_cmp_ps(abs_vec8, stable_radius_vec8, _CMP_LT_OS);
                dots_states_vec8 = _mm256_castps_si256(cmp_res_vec8);

                __m256 cmp_res_vec8_2 = _mm256_cmp_ps(abs_vec8_2, stable_radius_vec8, _CMP_LT_OS);
                dots_states_vec8_2 = _mm256_castps_si256(cmp_res_vec8_2);

                if (_mm256_testz_si256(dots_states_vec8, dots_states_vec8) &&
                    _mm256_testz_si256(dots_states_vec8_2, dots_states_vec8_2)) {
                    break;
                }

                iters_vec8 = _mm256_sub_epi32(iters_vec8, dots_states_vec8);
                iters_vec8_2 = _mm256_sub_epi32(iters_vec8_2, dots_states_vec8_2);
                iters++;
            }

            int iters_arr8[8] = {};
            int iters_arr8_2[8] = {};

            _mm256_store_si256((__m256i*)iters_arr8, iters_vec8);
            _mm256_store_si256((__m256i*)iters_arr8_2, iters_vec8_2);

            write_dot_iters_m256i_vector_to_matrix(calc_info, iters_matrix, ix, iy, iters_vec8);
            write_dot_iters_m256i_vector_to_matrix(calc_info, iters_matrix, ix + 8, iy, iters_vec8_2);
        }
    }
}
