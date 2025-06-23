import json
import os
import argparse

FUZZ_STRATEGIES = ["NONE", "RANDOM_FUZZING"]

parser = argparse.ArgumentParser(description='Create a fuzzing strategy config')
parser.add_argument('--iter_num', '-i', help='Number of fuzzing iterations per config file.')
parser.add_argument('-k', help='Initial prob mult factor')
parser.add_argument('--out', '-o', help='The output file.')
parser.add_argument('--strategy', '-s', help=f"The fuzzing strategy. One of the following values: {FUZZ_STRATEGIES}")
parser.add_argument('--covpath', '-c', help=f"The coverage output directory path.")
parser.add_argument('--use_cov_feedback', action=argparse.BooleanOptionalAction, default=False, help=f"Use coverage feedback to optimize fuzzing strategy.")
parser.add_argument('--beta', '-b', default=0, help=f"Value of Beta parameter. Makes sense only if use_cov_feedback is set.")


args = parser.parse_args()
iter_num = args.iter_num
out_file = args.out
fuzz_strategy = args.strategy
k = args.k
if not fuzz_strategy in FUZZ_STRATEGIES:
    print(f"Unknown fuzzing strategy: {fuzz_strategy}. Only the following values are supported: {FUZZ_STRATEGIES}")
    exit(-1)

covpath = args.covpath
use_cov_feedback = args.use_cov_feedback
beta = args.beta

dictionary = {
    "attach_attempts_per_iteration": int(iter_num),
    "fuzzing_strategies": [ fuzz_strategy ],
    "use_coverage_logging": True,
    "coverage_log_path": covpath,
    "use_coverage_feedback": use_cov_feedback,
    "restart_after_consecutive_successful_attaches": 10,
    "restart_after_consecutive_hangs": 3,
    "init_prob_mult_factor": float(k),
    "beta": float(beta),
    "mutation_prob": 100
}

json_object = json.dumps(dictionary, indent=4)

with open(out_file, "w") as f:
    f.write(json_object)