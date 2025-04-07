#ifndef ARGS_PROC_H
#define ARGS_PROC_H

#include <string.h>
#include <stdio.h>

#include "mandelbrot_set\display_funcs.h"

const size_t MAX_OPT_NAME_SZ = 64;
const size_t MAX_CONFIG_NAME_SIZE = 64;


typedef struct {
    const char *short_name;
    const char *long_name;

    const char *fmt;
    void *val_ptr;

    bool exist;
} opt_data;

typedef struct {
    int duration;
    char optimization[MAX_CONFIG_NAME_SIZE];
    char mode[MAX_CONFIG_NAME_SIZE];
    int draw_enable;
    int runs;
} config_t;

calc_function_t choose_calc_function(config_t *config);
void scan_argv(config_t *config, const int argc, const char *argv[]);
void opt_data_ctor(opt_data *option, const char *const short_name_src, const char *const long_name_src,
    const char *const fmt_src, void *val_ptr_src);
void opt_data_dtor(opt_data *option);
opt_data *option_list_ptr(const char *name, opt_data opts[], const size_t n_opts);
void get_options(const int argc, const char* argv[], opt_data opts[], const size_t n_opts);
void example_config_ctor(config_t *conf);
void example_config_dtor(config_t *conf);
void example_config_print(FILE *stream, config_t *conf);

#endif // ARGS_PROC_H
