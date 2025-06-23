#!/bin/bash

none_cfg_path="configs/none_cfg.json"
random_cfg_path="configs/random_cfg.json"

# for k in {1..2}
# do
ks=(0.25)
for k in "${ks[@]}"
do
    covpath="coverage_log/ul_no_cov_k_${k}_10000_it/"
    mkdir -p $covpath
    for i in {1..5}
    do
        python3 generate_fuzz_strategy_config.py -i 5 -o $none_cfg_path --strategy "NONE" -k $k \
                --covpath $covpath
        python3 generate_fuzz_strategy_config.py -i 2000 -o $random_cfg_path --strategy "RANDOM_FUZZING" -k $k \
                --covpath $covpath
        sudo ./build/LTE_5G_Fuzzer $none_cfg_path $random_cfg_path
        error_code=$?
        if [ $error_code -ne 0 ] && [ $error_code -ne 134 ]; then
            echo "Error code: $error_code"
            break
        fi
        sleep 15
    done
done