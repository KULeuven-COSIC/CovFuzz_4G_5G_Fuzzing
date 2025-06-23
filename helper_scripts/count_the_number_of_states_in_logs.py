import sys
import matplotlib.pyplot as plt
import os
import json
import argparse

def calculate_means(a,b):
    min_len = min(len(a), len(b))

    means = [(a[i] + b[i]) / 2 for i in range(min_len)]

    if len(a) > len(b):
        means.extend(a[min_len:])
    else:
        means.extend(b[min_len:])

    return means

# COV_LOG_PATH = "Coverage log path:"
NEW_FUZZ_ITER = "START OF A NEW FUZZING ITERATION"
DIRECTION = "Direction:"

parser = argparse.ArgumentParser()
parser.add_argument('-i', '--input', nargs='+', required=True, help="Input directories")
parser.add_argument('-l', '--labels', nargs='+', default=[])
args = parser.parse_args()

dir_names = args.input
labels = args.labels

if len(labels) == 0:
    labels = dir_names
else:
    if len(dir_names) != len(labels):
        print("Please specify the labels for all (or none) of the coverage dir names")
        exit(-1)

for label, log_dir_path in zip(labels, dir_names):
    result_map = {}

    all_states_sum = 0
    num_of_iters = 0

    all_lists = []
    for filename in os.listdir(log_dir_path):
        log_file_path = os.path.join(log_dir_path, filename)
        with open(log_file_path, 'r') as log_file:
            cur_cov_log_path = filename
            state_counter = 0
            iter_result_list = []
            for line in log_file:
                if len(cur_cov_log_path) == 0 and COV_LOG_PATH in line:
                    cur_cov_log_path = line[len(COV_LOG_PATH) : ]

                if NEW_FUZZ_ITER in line:
                    if len(cur_cov_log_path) == 0:
                        break
                    iter_result_list.append(state_counter)
                    all_states_sum += state_counter
                    num_of_iters += 1
                    state_counter = 0
                elif DIRECTION in line:
                    state_counter += 1
                
            if len(iter_result_list) != 0:
                if cur_cov_log_path in result_map.keys():
                    cur_list_for_cov_log_path = result_map[cur_cov_log_path]
                    result_map[cur_cov_log_path] = calculate_means(iter_result_list, cur_list_for_cov_log_path)
                else:
                    result_map[cur_cov_log_path] = iter_result_list
                
                if len(iter_result_list) < 2000:
                    continue
                all_lists.append(iter_result_list)

    avg_state_num = all_states_sum / num_of_iters

    print(f"Avg state number: {avg_state_num}")

    # with open(result_file_path, 'a+') as result_file:
    #     json.dump(result_map, result_file)

    avg_list = [sum(sublist[i] for sublist in all_lists) / len(all_lists) for i in range(2000)]

    # for key, value in result_map.items():

    plt.plot(avg_list, label=label)

plt.legend()
plt.title("Average number of packets")

plt.show()