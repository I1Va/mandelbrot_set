#include <cstddef>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include "mandelbrot_set\args_proc.h"
#include "mandelbrot_set\general.h"

display_function_t choose_display_function(config_t *config) {
    if (strcmp(config->optimization, "array") == 0) {
        printf("array optimization\n");
        return display_with_array_optimization;
    }
    if (strcmp(config->optimization, "intrinsic") == 0) {
        printf("intrinsic optimization\n");
        return display_with_intrinsic_optimization;
    }
    printf("without optimization\n");
    return display_without_optimizations;
}

void scan_argv(config_t *config, const int argc, const char *argv[]) {
    assert(config);
    opt_data options[] =
    {
        {"-d", "--duration", "%d", &config->duration},
        {"-O", "--optimization", "%s", config->optimization},
        {"-m", "--mode", "%s", &config->mode},
        {"-g", "--graphics", "%d", &config->draw_enable},
        {"-r", "--runs", "%d", &config->runs},
    };

    size_t n_options = sizeof(options) / sizeof(opt_data);

    get_options(argc, argv, options, n_options);
}

opt_data *option_list_ptr(const char *name, opt_data opts[], const size_t n_opts) {
    for (size_t i = 0; i < n_opts; i++) {
        if (strcmp(name, opts[i].short_name) == 0 || strcmp(name, opts[i].long_name) == 0) {
            return &opts[i];
        }
    }
    return NULL;
}

void get_options(const int argc, const char* argv[], opt_data opts[], const size_t n_opts) {
    assert(argc >= 0);

    for (int i = 1; i < argc; i++) {
        char name[MAX_OPT_NAME_SZ];
        char value[MAX_OPT_NAME_SZ];
        sscanf(argv[i], "%[^=]%s", name, value);

        opt_data *ptr = option_list_ptr(name, opts, n_opts);

        if (ptr != NULL) {
            sscanf(value + 1, (ptr->fmt), ptr->val_ptr);
            ptr->exist = true;
        }
    }
}

// // MODES/CONFIG ZONE

void example_config_print(FILE *stream, config_t *conf) {
    fprintf_red(stream, RED "config_t: \n");
    fprintf(stream, "duration     : %d\n", conf->duration);
    fprintf(stream, "optimization : %s\n", conf->optimization);
    fprintf(stream, "mode         : %s\n", conf->mode);
}
