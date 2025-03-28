#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include "TXLib.h"

#include "mandelbrot_set/display_funcs.h"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

int main () {
    tx_window_info_t tx_window_info = create_tx_window(SCREEN_WIDTH, SCREEN_HEIGHT);
    display_into_t display_info = display_init(tx_window_info, SCALE_DEFAULT, -SCREEN_WIDTH / 2.0f, -SCREEN_HEIGHT / 2.0f);

    for (;;) {
        update_display_info(&display_info);
        printf("FPS %lf\t\t\r", txGetFPS());

        if (display_info.terminate_state) {
            break;
        }

        display(&display_info);
    }

    return 0;
}
