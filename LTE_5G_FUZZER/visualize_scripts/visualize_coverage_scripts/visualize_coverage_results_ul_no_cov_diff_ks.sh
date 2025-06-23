#!/bin/bash
python3 ../helper_scripts/coverage_dir_visualizer.py -i \
    paper_coverage_log/ul_no_cov_k_0.1 \
    paper_coverage_log/ul_no_cov_k_0.25 \
    paper_coverage_log/ul_no_cov_k_0.5 \
    paper_coverage_log/ul_no_cov_k_1 \
    paper_coverage_log/ul_no_cov_k_2 \
    paper_coverage_log/ul_no_cov_k_3 \
    -l \
    "RANDOM FUZZER (\$k\$=0.1)" \
    "RANDOM FUZZER (\$k\$=0.25)" \
    "RANDOM FUZZER (\$k\$=0.5)" \
    "RANDOM FUZZER (\$k\$=1)" \
    "RANDOM FUZZER (\$k\$=2)" \
    "RANDOM FUZZER (\$k\$=3)" \
    -y \
    "SRSENB COVERAGE" \
    -c \
    "pink" \
    "orange" \
    "green" \
    "black" \
    "brown" \
    "aqua" \
    -s