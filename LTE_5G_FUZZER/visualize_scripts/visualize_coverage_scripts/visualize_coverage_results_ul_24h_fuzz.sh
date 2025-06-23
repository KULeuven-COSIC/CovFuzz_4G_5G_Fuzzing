#!/bin/bash
python3 ../helper_scripts/coverage_dir_visualizer_by_time.py -i \
    paper_coverage_log/ul_no_fuzz_24h/ \
    paper_coverage_log/ul_no_cov_k_0.25_70000_it_copy/ \
    paper_coverage_log/ul_srsenb_cov_beta_2_k_3_2000_it_long_clip_1/ \
    paper_coverage_log/ul_srsenb_cov_beta_2_k_3_2000_it_long_clip_1_resets_5_10/ \
    paper_coverage_log/ul_srsue_cov_beta_6_k_3_2000_it_long_clip_1_copy/ \
    paper_coverage_log/ul_srsue_cov_beta_6_k_3_2000_it_long_clip_1_resets_5_10_copy \
    -l \
    "BASELINE (NO FUZZING)" \
    "RANDOM FUZZER (\$k\$=0.25)" \
    "COVERAGE-BASED GREY-BOX FUZZER (\$\beta\$=2, \$k\$=3)" \
    "COVERAGE-BASED GREY-BOX FUZZER (\$\beta\$=2, \$k\$=3) WITH RESETS" \
    "COVERAGE-BASED BLACK-BOX FUZZER (\$\beta\$=2, \$k\$=3)" \
    "COVERAGE-BASED BLACK-BOX FUZZER (\$\beta\$=2, \$k\$=3) WITH RESETS" \
    -c \
    "pink" \
    "orange" \
    "green" \
    "black" \
    "red" \
    "brown" \
    -y \
    "SRSENB COVERAGE" \
    -s