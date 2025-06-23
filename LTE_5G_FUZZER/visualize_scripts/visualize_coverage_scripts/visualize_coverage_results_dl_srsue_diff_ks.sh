#!/bin/bash
python3 ../helper_scripts/coverage_dir_visualizer.py -i \
    paper_coverage_log/dl_no_cov_k_0.5 \
    paper_coverage_log/dl_srsue_cov_beta_4_k_0.5 \
    paper_coverage_log/dl_srsue_cov_beta_4_k_1 \
    paper_coverage_log/dl_srsue_cov_beta_4_k_2 \
    paper_coverage_log/dl_srsue_cov_beta_4_k_3 \
    paper_coverage_log/dl_srsue_cov_beta_4_k_4 \
    -l \
    "RANDOM FUZZER (\$k\$=0.5)" \
    "GREY-BOX DL FUZZER (\$\beta\$=4, \$k\$=0.5)" \
    "GREY-BOX DL FUZZER (\$\beta\$=4, \$k\$=1)" \
    "GREY-BOX DL FUZZER (\$\beta\$=4, \$k\$=2)" \
    "GREY-BOX DL FUZZER (\$\beta\$=4, \$k\$=3)" \
    "GREY-BOX DL FUZZER (\$\beta\$=4, \$k\$=4)" \
    -y \
    "SRSUE COVERAGE" \
    -s