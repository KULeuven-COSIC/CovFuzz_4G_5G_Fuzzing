#!/bin/bash
python3 ../helper_scripts/coverage_dir_visualizer.py -i \
    paper_coverage_log/srsenb_no_fuzzing \
    paper_coverage_log/ul_no_cov_k_0.25 \
    paper_coverage_log/ul_srsue_cov_beta_2_k_3 \
    paper_coverage_log/ul_srsue_cov_beta_4_k_3 \
    paper_coverage_log/ul_srsue_cov_beta_6_k_3 \
    paper_coverage_log/ul_srsue_cov_beta_8_k_3 \
    -l \
    "BASELINE (NO FUZZING)" \
    "RANDOM FUZZER (\$k\$=0.25)" \
    "BLACK-BOX UL FUZZER (\$\beta\$=2, \$k\$=3)" \
    "BLACK-BOX UL FUZZER (\$\beta\$=4, \$k\$=3)" \
    "BLACK-BOX UL FUZZER (\$\beta\$=6, \$k\$=3)" \
    "BLACK-BOX UL FUZZER (\$\beta\$=8, \$k\$=3)" \
    -y \
    "SRSENB COVERAGE" \
    -c \
    "pink" \
    "orange" \
    "red" \
    "black" \
    "brown" \
    "aqua" \
    -s