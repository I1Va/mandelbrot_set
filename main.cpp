#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>

#include "TXLib.h"
#include "mandelbrot_set/display_funcs.h"
#include "mandelbrot_set/args_proc.h"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

int main (const int argc, const char *argv[]) {
    config_t config = {};
    config.duration = -1;

    scan_argv(&config, argc, argv);

    tx_window_info_t tx_window_info = create_tx_window(SCREEN_WIDTH, SCREEN_HEIGHT);
    display_info_t display_info = display_init(tx_window_info, SCALE_DEFAULT, -3, -2);

    clock_t start = clock();


    display_function_t display_function = choose_display_function(&config);

    for (;;) {
        if ((clock() - start) / CLOCKS_PER_SEC > config.duration && config.duration > 0) {
            break;
        }

        update_display_info(&display_info);
        printf("FPS %lf\t\t\r", txGetFPS());

        if (display_info.terminate_state) {
            break;
        }
        display_function(&display_info);


        // #ifdef INTRINSIC_OPTIMIZATION
        //     display_with_intrinsic_optimization(&display_info);
        // #elif ARR_OPTIMIZATION
        //     display_with_array_optimization(&display_info);
        // #else
        //     display_without_optimizations(&display_info);
        // #endif

    }

    return 0;
}
