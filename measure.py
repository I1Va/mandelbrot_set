import glob, os, math

stabilization_runs = 10
stabilization_duration = 10

runs_cnt = 5
duration = 5

stabilization_flags = f" --graphics=1 --runs={stabilization_runs} --duration={stabilization_duration}"
general_flags = f" --graphics=1 --runs={runs_cnt} --duration={duration}"

folder = "tests"
cur_res = folder + "/tmp.txt"
general_res = "general_res.txt"
all_opti_flags = [" -O=array ", " -O=no-optimization ", " -O=intrinsic ", " -O=unroll_2 "]


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
    for fn in glob.glob(folder + "/*.exe"):
        os.system(fn + stabilization_flags)
        break

def solution():
    stabilization()

    if (os.path.exists(general_res)):
        os.remove(f"{general_res}")

    for fn in glob.glob(folder + "/*.exe"):
        for opti_flags in all_opti_flags:
            os.system(fn + general_flags + opti_flags + " > " + cur_res)
            with open(cur_res, 'r') as res_file:
                array = res_file.readlines()
                opti_name = array[0][:-1]
                array = array[1:]
                for i in range(0, len(array)):
                    array[i] = float(array[i].split()[1])

                result_measure = get_measure(array)

                with open(general_res, "a") as file:
                    file.write(f"{os.path.basename(fn)} '{opti_name}'\n: {result_measure * runs_cnt}\n\n")
    os.remove(f"{cur_res}")

solution()