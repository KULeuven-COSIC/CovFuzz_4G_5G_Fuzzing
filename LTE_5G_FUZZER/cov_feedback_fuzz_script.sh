#!/bin/bash

none_cfg_path="configs/none_cfg.json"
random_cfg_path="configs/random_cfg.json"
# random_cfg_path_2="configs/random_cfg_2.json"
# for k in {2..5..2}
ks=(2)
for k in "${ks[@]}"
do
    b=2
    # for b in {4..5..2}
    # do
    covpath="coverage_log/ul_srsenb_cov_beta_${b}_k_${k}/"
    mkdir -p -m777 $covpath
    for i in {1..20}
    do
        python3 generate_fuzz_strategy_config.py -i 5 -o $none_cfg_path --strategy "NONE" \
                --covpath $covpath --use_cov_feedback --beta $b -k $k
        python3 generate_fuzz_strategy_config.py -i 2000 -o $random_cfg_path --strategy "RANDOM_FUZZING" \
                --covpath $covpath --use_cov_feedback --beta $b -k $k
        sudo ./build/LTE_5G_Fuzzer $none_cfg_path $random_cfg_path
        error_code=$?
        if [ $error_code -ne 0 ] && [ $error_code -ne 134 ]; then
            echo "Error code: $error_code"
            break
        fi
        sleep 15
    done
    # done
done