#!/bin/bash
python3 ../helper_scripts/coverage_dir_visualizer_by_time.py -i \
    paper_coverage_log/dl_no_fuzz_24h/ \
    paper_coverage_log/dl_no_cov_k_0.5_70000_it/ \
    paper_coverage_log/dl_srsue_cov_beta_4_k_3_2000_it_long_clip_1/ \
    paper_coverage_log/dl_srsenb_cov_beta_2_k_3_2000_it_long_clip_1_copy/ \
    -l \
    "BASELINE (NO FUZZING)" \
    "RANDOM FUZZER (\$k\$=0.5)" \
    "COVERAGE-BASED GREY-BOX FUZZER (\$\beta\$=4, \$k\$=3)" \
    "COVERAGE-BASED BLACK-BOX FUZZER (\$\beta\$=2, \$k\$=3)" \
    -c \
    "pink" \
    "orange" \
    "green" \
    "red" \
    -y \
    "SRSUE COVERAGE" \
    -s