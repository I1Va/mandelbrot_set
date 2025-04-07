#include <TXLib.h>

#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>


#include "mandelbrot_set/display_funcs.h"
#include "mandelbrot_set/args_proc.h"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;
const int OFFSET_X = -3;
const int OFFSET_Y = -2;

int main (const int argc, const char *argv[]) {
    config_t config = {};
    config.duration = -1;
    config.runs = 1;
    config.draw_enable = 1;
    scan_argv(&config, argc, argv);

    tx_window_info_t tx_window_info = {};

    if (config.draw_enable) {
        tx_window_info = create_tx_window(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    calc_info_t calc_info = display_init(tx_window_info, SCALE_DEFAULT, OFFSET_X, OFFSET_Y, SCREEN_WIDTH, SCREEN_HEIGHT);
    calc_function_t calc_function = choose_calc_function(&config);

    clock_t start = clock();
    int *iters_matrix = (int *) calloc(calc_info.screen_height * calc_info.screen_width, sizeof(int));

    for (;;) {
        double frame_start = clock();

        update_display_info(&calc_info);

        if (calc_info.terminate_state ||
            ((clock() - start) / CLOCKS_PER_SEC > config.duration && config.duration > 0)) {
            break;
        }

        for (int i = 0; i < config.runs; i++) {
            calc_function(&calc_info, iters_matrix);
        }

        display(&calc_info, iters_matrix, default_color_func);

        double frame_end = clock();
        double duration = (frame_end - frame_start);
        printf("FPS %lf\t\t\r", CLOCKS_PER_SEC / duration);

    }

    free(iters_matrix);

    return 0;
}
