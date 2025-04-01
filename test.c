#include <math.h>
#include <immintrin.h>

extern __m128 _mm_move_ss (__m128 a, __m128 b);
extern __m128 _mm_add_ss (__m128 a, __m128 b);
extern __m128 _mm_load_ps (float const* mem_addr);

int main() {
    float a[] = {0, 0, 0, 0};
    float b[] = {1, 1, 1, 1};

    __m128 a_vec = _mm_load_ps(a);
    __m128 b_vec = _mm_load_ps(b);

    _mm_add_ss(a_vec, b_vec);
    

    return 0;
}