import glob, os, math
import matplotlib.pyplot as plt
import sys

stabilization_runs = 10
stabilization_duration = 30

runs_cnt = 1
duration = 50

stabilization_flags = f"-O=no --graphics=0 --runs={stabilization_runs} --duration={stabilization_duration}"
general_flags = f" --graphics=0 --runs={runs_cnt} --duration={duration}"

folder = "tests"
cur_res = folder + "/tmp.txt"
general_res = "general_res.txt"
all_opti_flags = [" -O=array ", " -O=no-optimization ", " -O=intrinsic ", " -O=unroll_2 "]


def build_plot(array):
    plot_name = "test"

    plot_path = "plots/" + plot_name

    plt.title(plot_name)
    plt.xlabel('iteration')
    plt.ylabel('fps')

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

    return sum(selected) / len(selected)

def stabilization():
    os.system("start /HIGH tests\Og_mandelbrot_set.exe " + stabilization_flags + " > tests\\tmp.txt")

def solution():
    # stabilization()

    if (os.path.exists(general_res)):
        os.remove(f"{general_res}")

    for fn in glob.glob(folder + "/*.exe"):
        for opti_flags in all_opti_flags:
            os.system("start /HIGH " + fn + general_flags + opti_flags + " > " + cur_res)
            # print(os.path.basename(fn) + general_flags + opti_flags)
            with open(cur_res, 'r') as res_file:
                array = res_file.readlines()
                opti_name = array[0][:-1]
                array = array[1:]
                for i in range(0, len(array)):
                    array[i] = float(array[i].split()[1])

                build_plot(array)
                break

                result_measure = get_measure(array)

                with open(general_res, "a") as file:
                    file.write(f"{os.path.basename(fn)} '{opti_name}'\n: {result_measure * runs_cnt}\n\n")
    os.remove(f"{cur_res}")

solution()