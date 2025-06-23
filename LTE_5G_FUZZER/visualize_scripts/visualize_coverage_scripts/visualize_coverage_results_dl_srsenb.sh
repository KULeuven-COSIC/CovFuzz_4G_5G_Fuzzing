#!/bin/bash
python3 ../helper_scripts/coverage_dir_visualizer.py -i \
    paper_coverage_log/srsenb_no_fuzzing \
    paper_coverage_log/dl_no_cov_k_0.5 \
    paper_coverage_log/dl_srsenb_cov_beta_1_k_3 \
    paper_coverage_log/dl_srsenb_cov_beta_2_k_3 \
    paper_coverage_log/dl_srsenb_cov_beta_3_k_3 \
    paper_coverage_log/dl_srsenb_cov_beta_4_k_3 \
    paper_coverage_log/dl_srsenb_cov_beta_5_k_3 \
    -l \
    "BASELINE (NO FUZZING)" \
    "RANDOM FUZZER (\$k\$=0.5)" \
    "BLACK-BOX DL FUZZER (\$\beta\$=1, \$k\$=3)" \
    "BLACK-BOX DL FUZZER (\$\beta\$=2, \$k\$=3)" \
    "BLACK-BOX DL FUZZER (\$\beta\$=3, \$k\$=3)" \
    "BLACK-BOX DL FUZZER (\$\beta\$=4, \$k\$=3)" \
    "BLACK-BOX DL FUZZER (\$\beta\$=5, \$k\$=3)" \
    -y \
    "SRSUE COVERAGE" \
    -s