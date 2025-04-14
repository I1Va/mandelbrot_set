import os, math
import matplotlib.pyplot as plt

def generate_benchmark_makefile(compile_flags, launch_flags):
    content = f'CC = g++\n\
OUT_O_DIR = build\n\
OUTFILE_NAME = benchmark.out\n\
\n\
LAUNCH_FLAGS = {launch_flags}\n\
CFLAGS = -I./inc -march=native {compile_flags}\n\
\n\
CSRC = benchmarks.cpp src/args_proc.cpp src/control_funcs.cpp $(wildcard src/calc_funcs/*.cpp)\n\
COBJ = $(addprefix $(OUT_O_DIR)/,$(CSRC:.cpp=.o))\n\
\n\
all: $(OUT_O_DIR)/$(OUTFILE_NAME)\n\
\t@$(OUT_O_DIR)\\$(OUTFILE_NAME) $(LAUNCH_FLAGS)\n\
\t@del /S $(OUT_O_DIR)\*.out $(OUT_O_DIR)\*.o\n\
\n\
$(OUT_O_DIR)/$(OUTFILE_NAME): $(COBJ)\n\
\t@$(CC) $^ -o $@\n\
\n\
$(COBJ) : $(OUT_O_DIR)/%.o : %.cpp\n\
\t@$(CC) $(CFLAGS) -c $< -o $@\n\
'

    return content

def get_raw_benchmark_results(benchmark_res_file):
    res_list = []
    with open(benchmark_res_file, "r") as file:
        res_list = file.readlines()

    return [res_list[0],  list(map(int, res_list[1:]))]

def build_plot(array):
    plot_name = "test"

    plot_path = "plots/" + plot_name

    plt.title(plot_name)
    plt.xlabel('iteration')
    plt.ylabel('ticks')

    x_cords = [i for i in range(len(array))]
    y_cords = array

    plt.plot(x_cords, y_cords)

    plt.legend()
    plt.grid()
    plt.savefig(plot_path + ".png")

    plt.show()

def get_measure(array):
    n = len(array)

    average = sum(array) / n
    delta_s_2 = [(average - array[i]) ** 2 for i in range(n)]
    standard_deviation = math.sqrt(sum(delta_s_2) / n)

    selected = []
    for x in array:
        if abs(x - average) < 2 * standard_deviation:
            selected.append(x)

    return float(sum(selected)) / len(selected)

def launch_benchmark(compile_flags, launch_flags):
    print(f'launch benchmark : \n\
    compile_flags = <{compile_flags}>\n\
    launch_flags  = <{launch_flags}>\n')

    makefile_name = "build_benchmark"
    benchmark_res_file = "benchmark_res.out"

    launch_flags += f' > {benchmark_res_file}'

    makefile_content = generate_benchmark_makefile(compile_flags, launch_flags)
    with open(makefile_name, "w") as file:
        file.write(makefile_content)
    os.system(f'make -f {makefile_name}')

    benchmark_raw_res = get_raw_benchmark_results(benchmark_res_file)

    benchmark_measure = get_measure(benchmark_raw_res[1])
    benchmark_name = benchmark_raw_res[0].replace("\n", "")
    benchmark_name = f'{benchmark_name} <{compile_flags}>'

    os.remove(makefile_name)
    os.remove(benchmark_res_file)

    return [benchmark_name, benchmark_measure]

def generate_compile_flags_list():
    compile_flags_list = []

    for O_flag in ["-Og", "-O2", "-Ofast"]:
        for M_tune in ["", "-mtune=native"]:
            compile_flags_list.append(f'{O_flag} {M_tune}')

    return compile_flags_list

def generate_launch_flags_list(duration):
    launch_flags_list = []

    for O_flag in ["-O=no-optimization", "-O=array", "-O=intrinsic", "-O=unroll_2"]:
        launch_flags_list.append(f'{O_flag} -d={duration}')

    return launch_flags_list

if __name__ == "__main__":
    duration = 5

    compile_flags_list = generate_compile_flags_list()
    launch_flags_list = generate_launch_flags_list(duration)

    benchmark_general_res = []

    for compile_flags in compile_flags_list:
        for launch_flags in launch_flags_list:
            benchmark_current_res = launch_benchmark(compile_flags, launch_flags)
            benchmark_general_res.append(benchmark_current_res)

    benchmark_name_alignment = 60

    with open("benchmark.res", "w") as file:
        for result in benchmark_general_res:
            benchmark_name = str(result[0])
            benchmark_name += " " * (benchmark_name_alignment - len(benchmark_name))
            benchmark_measure = str(result[1])

            file.write(f'{benchmark_name} : {benchmark_measure}\n')


