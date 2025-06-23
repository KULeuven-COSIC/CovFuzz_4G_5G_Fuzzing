import sys
import matplotlib
import matplotlib.pyplot as plt
import os
import json
import statistics
import argparse


# matplotlib.use("pgf")
# matplotlib.rcParams.update({
#     "pgf.texsystem": "pdflatex",
#     'font.family': 'serif',
#     'text.usetex': True,
#     'pgf.rcfonts': False
# })

plt.figure(figsize=(10,6))

default_colors = ["orange", "purple", "red", "green", "black", "grey", "brown", "blue", "aqua", "yellow"]
font = {   
            'weight' : 'normal',
            'size' : '14'
       }

def calculate_means(a,b):
    min_len = min(len(a), len(b))

    means = [(a[i] + b[i]) / 2 for i in range(min_len)]

    if len(a) > len(b):
        means.extend(a[min_len:])
    else:
        means.extend(b[min_len:])

    return means

COV_LOG_PATH = "Coverage log path:"
NEW_FUZZ_ITER = "START OF A NEW FUZZING ITERATION"
MUTATION = "Mutator:"

parser = argparse.ArgumentParser()
parser.add_argument('--direction', '-d', required=True, help="Direction: UPLINK or DOWNLINK")
parser.add_argument('-i', '--input', nargs='+', required=True, help="Input directories")
parser.add_argument('-l', '--labels', nargs='+', default=[])
parser.add_argument('-c', '--colors', nargs='+', default=[])
parser.add_argument('-o', '--output', required=False, help="Output filename")
args = parser.parse_args()

user_direction = args.direction
dir_names = args.input
labels = args.labels
colors = args.colors
output = args.output

if len(labels) == 0:
    labels = dir_names
else:
    if len(dir_names) != len(labels):
        print("Please specify the labels for all (or none) of the coverage dir names")
        exit(-1)

if len(colors) == 0:
    colors = default_colors
else:
    if len(colors) != len(dir_names):
        print("Please specify the colors for all (or none) of the coverage dir names")
        exit(-1)

UPLINK = "UPLINK"
DOWNLINK = "DOWNLINK"
assert(user_direction == UPLINK or user_direction == DOWNLINK)
DIRECTION = "Direction: " + user_direction

for color, label, log_dir_path in zip(colors, labels, dir_names):
    result_map = {}

    all_states_sum = 0
    num_of_iters = 0

    all_lists = []
    result_map = {}

    all_values_sum = 0
    all_values_num = 0
    all_msg_num = 0
    it_msg_num = 0

    for filename in os.listdir(log_dir_path):
        log_file_path = os.path.join(log_dir_path, filename)
        with open(log_file_path, 'r') as log_file:
            cur_cov_log_path = filename
            mut_counter = 0
            iter_result_list = []
            for line in log_file:
                if len(cur_cov_log_path) == 0 and COV_LOG_PATH in line:
                    cur_cov_log_path = line[len(COV_LOG_PATH) : ]

                if DIRECTION in line:
                    all_msg_num += 1
                    it_msg_num += 1

                if NEW_FUZZ_ITER in line:
                    if len(cur_cov_log_path) == 0:
                        break
                    if it_msg_num != 0:
                        iter_result_list.append(mut_counter / it_msg_num)
                    else:
                        assert(mut_counter == 0)
                        iter_result_list.append(mut_counter)
                    mut_counter = 0
                    it_msg_num = 0
                elif MUTATION in line:
                    mut_counter += 1
                
            if len(iter_result_list) != 0:
                all_values_sum += sum(iter_result_list)
                all_values_num += len(iter_result_list)
                if cur_cov_log_path in result_map.keys():
                    cur_list_for_cov_log_path = result_map[cur_cov_log_path]
                    result_map[cur_cov_log_path] = calculate_means(iter_result_list, cur_list_for_cov_log_path)
                else:
                    result_map[cur_cov_log_path] = iter_result_list
                
                if len(iter_result_list) < 2000:
                    continue
                all_lists.append(iter_result_list)
            
    avg_list = [sum(sublist[i] for sublist in all_lists) / len(all_lists) for i in range(2000)]
    plt.plot(avg_list, color=color, label=label)

# with open(result_file_path, 'w') as result_file:
#     json.dump(result_map, result_file)

# for key, value in result_map.items():
#     plt.plot(value, label=key)
# plt.legend()
# plt.show()

# avg_value_per_iter = all_values_sum / all_values_num
# avg_value_per_msg  = all_values_sum / all_msg_num
# avg_msg_num_per_iter = avg_value_per_iter / avg_value_per_msg

# print(f"Average value of mutated fields per iteration: {avg_value_per_iter}")
# print(f"Average value of mutated fields per message: {avg_value_per_msg}")
# print(f"Average number of mutated messages per iterantion: {avg_msg_num_per_iter}")

plt.xlabel("NUMBER OF FUZZING ITERATIONS", fontsize=14)
plt.ylabel("NUMBER OF MUTATIONS", fontsize=14)
plt.legend(prop=font)
#plt.title("AVERAGE NUMBER OF MUTATIONS PER PACKET")
if output:
    plt.savefig(output, bbox_inches='tight')
else:
    plt.show()