#include <immintrin.h>
#include <stdbool.h>
#include <assert.h>


#define STABLE_RADIUS 16.0
#define min(a, b)  (((a) < (b)) ? (a) : (b))



const int MAX_ITERATIONS_CNT = 500;
const int    COLOR_MAX_VAL = 255;

__m128 stable_radius_vec4 = _mm_set1_ps(STABLE_RADIUS);

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

} display_info_t;

// ===========================VOLATILES=================================

// ===========================VOLATILES=================================




// volatile int iters = 0;
// void display_without_optimizations(display_info_t *display_info, bool draw_enable) {
//     assert(display_info);

//     for (int iy = 0; iy < display_info->screen_height; iy++) {
//         double y0 = display_info->offset_y + iy * display_info->scale;

//         for (int ix = 0; ix < display_info->screen_width; ix++) {
//             double x0 = display_info->offset_x + ix * display_info->scale;

//             double x = 0;
//             double y = 0;
//             double x2 = 0;
//             double y2 = 0;
//             double xy = 0;

//             iters = 0;

//             while (iters < MAX_ITERATIONS_CNT && x2 + y2 < STABLE_RADIUS) {
//                 x2 = x * x;
//                 y2 = y * y;
//                 xy = x * y;

//                 x = x2 - y2 + x0;
//                 y = 2 * xy + y0;

//                 iters++;
//             }
//         }
//     }
// }

#define ARR_SZ 8
volatile int iters_arr[ARR_SZ] = {};

void display_with_array_optimization(display_info_t *display_info, bool draw_enable) {
    assert(display_info);

    for (int iy = 0; iy < display_info->screen_height; iy++) {
        float y0 = (float) (display_info->offset_y + iy * display_info->scale);

        for (int ix = 0; ix < display_info->screen_width; ix+=ARR_SZ) {
            float x0 = (float) (display_info->offset_x + ix * display_info->scale);
            float dx = (float) display_info->scale;


            float x0_arr[ARR_SZ] = {};
            float y0_arr[ARR_SZ] = {};
            float x_arr[ARR_SZ] = {};
            float y_arr[ARR_SZ] = {};
            float x2_arr[ARR_SZ] = {};
            float y2_arr[ARR_SZ] = {};
            float xy_arr[ARR_SZ] = {};
            float abs_arr[ARR_SZ] = {};

            float stable_radius_arr[ARR_SZ] = {};
            int dots_state_arr[ARR_SZ] = {};

            for (int i = 0; i < ARR_SZ; i++) {
                x0_arr[i] = x0 + i * dx;
                y0_arr[i] = y0;
                dots_state_arr[i] = 1;
                stable_radius_arr[i] = STABLE_RADIUS;
                iters_arr[i] = 0;
            }


            int iters = 0;
            while (iters < MAX_ITERATIONS_CNT) {
                for (int i = 0; i < ARR_SZ; i++) {
                    x2_arr[i] = x_arr[i] * x_arr[i];
                    y2_arr[i] = y_arr[i] * y_arr[i];
                    abs_arr[i] = x2_arr[i] + y2_arr[i];
                }

                for (int i = 0; i < ARR_SZ; i++) {
                    xy_arr[i] = x_arr[i] * y_arr[i];
                }

                for (int i = 0; i < ARR_SZ; i++) {
                    x_arr[i] = x2_arr[i] - y2_arr[i] + x0_arr[i];
                    y_arr[i] = 2 * xy_arr[i] + y0_arr[i];
                }

                for (int i = 0; i < ARR_SZ; i++) {
                    x_arr[i] = min(x_arr[i], stable_radius_arr[i]);
                    y_arr[i] = min(y_arr[i], stable_radius_arr[i]);
                }

                for (int i = 0; i < ARR_SZ; i++) {
                    dots_state_arr[i] = (int) (abs_arr[i] <= STABLE_RADIUS);
                }

                int mask = 0;
                for (int i = 0; i < ARR_SZ; i++) {
                    mask =  (mask | (dots_state_arr[i] << i));
                }

                if ((!mask)) {
                    break;
                }

                for (int i = 0; i < ARR_SZ; i++) {
                    iters_arr[i] += dots_state_arr[i];
                }
                iters++;
            }
        }
    }
}


// void display_with_intrinsic_optimization(display_info_t *display_info, bool draw_enable) {
//     assert(display_info);
//     for (int iy = 0; iy < display_info->screen_height; iy++) {
//         float y0 = (float) (display_info->offset_y + iy * display_info->scale);

//         __m128 y0_vec4 = _mm_set1_ps(y0);

//         for (int ix = 0; ix < display_info->screen_width; ix+=4) {
//             float x0 = (float) (display_info->offset_x + ix * display_info->scale);
//             float dx = (float) display_info->scale;

//             __m128 x0_vec4 = _mm_set_ps(x0 + 3 * dx, x0 + 2 * dx, x0 + dx, x0);

//             __m128 x_vec4  = _mm_set1_ps(0);
//             __m128 y_vec4  = _mm_set1_ps(0);

//             __m128 x2_vec4  = _mm_set1_ps(0);
//             __m128 y2_vec4  = _mm_set1_ps(0);
//             __m128 xy_vec4  = _mm_set1_ps(0);
//             __m128 abs_vec4 = _mm_set1_ps(0);

//             __m128i dots_states_vec4 = _mm_set1_epi32(1);
//             __m128i iters_vec4       = _mm_set1_epi32(0);

//             iters_arr4[0] = 0;
//             iters_arr4[1] = 1;
//             iters_arr4[2] = 2;
//             iters_arr4[3] = 3;

//             int iters = 0;

//             while (iters < MAX_ITERATIONS_CNT) {
//                 x2_vec4 = _mm_mul_ps(x_vec4, x_vec4);
//                 y2_vec4 = _mm_mul_ps(y_vec4, y_vec4);
//                 abs_vec4 = _mm_add_ps(x2_vec4, y2_vec4);

//                 xy_vec4 = _mm_mul_ps(x_vec4, y_vec4);

//                 x2_vec4 = _mm_sub_ps(x2_vec4, y2_vec4); // x_arr4[i] = x2_arr4[i] - y2_arr4[i] + x0_arr4[i];
//                 x_vec4 = _mm_add_ps(x2_vec4, x0_vec4);

//                 xy_vec4 = _mm_add_ps(xy_vec4, xy_vec4); // y_arr4[i] = 2 * xy_arr4[i] + y0_arr4[i];
//                 y_vec4 = _mm_add_ps(xy_vec4, y0_vec4);


//                 x_vec4 = _mm_min_ps (x_vec4, stable_radius_vec4);
//                 y_vec4 = _mm_min_ps (y_vec4, stable_radius_vec4);

//                 __m128 cmp_res_vec4 = _mm_cmp_ps(abs_vec4, stable_radius_vec4, _CMP_LT_OS); // dots_state_arr4[i] = (int) (abs_arr4[i] <= STABLE_RADIUS);
//                 dots_states_vec4 = _mm_castps_si128(cmp_res_vec4);

//                 if (_mm_testz_si128(dots_states_vec4, dots_states_vec4)) { // if dots_states_vec4 == 0
//                     break;
//                 }

//                 iters_vec4 = _mm_sub_epi32(iters_vec4, dots_states_vec4);
//                 iters++;
//             }

//             iters_arr4[0] = _mm_cvtsi128_si32(_mm_shuffle_epi32(iters_vec4, _MM_SHUFFLE(0, 0, 0, 0)));
//             iters_arr4[1] = _mm_cvtsi128_si32(_mm_shuffle_epi32(iters_vec4, _MM_SHUFFLE(0, 0, 0, 1)));
//             iters_arr4[2] = _mm_cvtsi128_si32(_mm_shuffle_epi32(iters_vec4, _MM_SHUFFLE(0, 0, 0, 2)));
//             iters_arr4[3] = _mm_cvtsi128_si32(_mm_shuffle_epi32(iters_vec4, _MM_SHUFFLE(0, 0, 0, 3)));
//         }
//     }
// }

// volatile int iters_arr8[8] = {};
// const __m256 stable_radius_vec8 = _mm256_set1_ps(STABLE_RADIUS);
// void display_with_intrinsic_optimization(display_info_t *display_info, bool draw_enable) {
//     assert(display_info);


//     for (int iy = 0; iy < display_info->screen_height; iy++) {
//         float y0 = (float)(display_info->offset_y + iy * display_info->scale);
//         __m256 y0_vec8 = _mm256_set1_ps(y0);

//         for (int ix = 0; ix < display_info->screen_width; ix += 8) {
//             float x0_base = (float)(display_info->offset_x + ix * display_info->scale);
//             float dx = (float)display_info->scale;

//             __m256 x0_vec8 = _mm256_set_ps(
//                 x0_base + 7 * dx, x0_base + 6 * dx, x0_base + 5 * dx, x0_base + 4 * dx,
//                 x0_base + 3 * dx, x0_base + 2 * dx, x0_base + 1 * dx, x0_base
//             );

//             __m256 x_vec8 = _mm256_setzero_ps();
//             __m256 y_vec8 = _mm256_setzero_ps();
//             __m256 x2_vec8 = _mm256_setzero_ps();
//             __m256 y2_vec8 = _mm256_setzero_ps();
//             __m256 xy_vec8 = _mm256_setzero_ps();
//             __m256 abs_vec8 = _mm256_setzero_ps();

//             __m256i dots_states_vec8 = _mm256_set1_epi32(1);
//             __m256i iters_vec8 = _mm256_setzero_si256();

//             int iters = 0;

//             while (iters < MAX_ITERATIONS_CNT) {
//                 x2_vec8 = _mm256_mul_ps(x_vec8, x_vec8);
//                 y2_vec8 = _mm256_mul_ps(y_vec8, y_vec8);
//                 abs_vec8 = _mm256_add_ps(x2_vec8, y2_vec8);

//                 xy_vec8 = _mm256_mul_ps(x_vec8, y_vec8);

//                 x2_vec8 = _mm256_sub_ps(x2_vec8, y2_vec8);
//                 x_vec8 = _mm256_add_ps(x2_vec8, x0_vec8);
//                 xy_vec8 = _mm256_add_ps(xy_vec8, xy_vec8);
//                 y_vec8 = _mm256_add_ps(xy_vec8, y0_vec8);

//                 x_vec8 = _mm256_min_ps(x_vec8, stable_radius_vec8);
//                 y_vec8 = _mm256_min_ps(y_vec8, stable_radius_vec8);


//                 __m256 cmp_res_vec8 = _mm256_cmp_ps(abs_vec8, stable_radius_vec8, _CMP_LT_OS);
//                 dots_states_vec8 = _mm256_castps_si256(cmp_res_vec8);

//                 if (_mm256_testz_si256(dots_states_vec8, dots_states_vec8)) {
//                     break;
//                 }

//                 iters_vec8 = _mm256_sub_epi32(iters_vec8, dots_states_vec8);
//                 iters++;
//             }


//             _mm256_storeu_si256((__m256i*)iters_arr8, iters_vec8);

//         }
//     }
// }
