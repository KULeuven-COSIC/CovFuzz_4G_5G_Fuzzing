#!/bin/bash
python3 ../helper_scripts/count_the_number_of_mutations_in_logs.py -i \
    paper_log/dl_no_cov_k_0.5 \
    paper_log/dl_srsue_beta_2_k_3 \
    paper_log/dl_srsue_beta_4_k_3 \
    paper_log/dl_srsue_beta_6_k_3 \
    paper_log/dl_srsue_beta_8_k_3 \
    -l \
    "RANDOM FUZZER (\$k\$=0.5)" \
    "GREY-BOX DL FUZZER (\$\beta\$=2, \$k\$=3)" \
    "GREY-BOX DL FUZZER (\$\beta\$=4, \$k\$=3)" \
    "GREY-BOX DL FUZZER (\$\beta\$=6, \$k\$=3)" \
    "GREY-BOX DL FUZZER (\$\beta\$=8, \$k\$=3)" \
    -d \
    DOWNLINK \
    -c \
    "orange" \
    "red" \
    "black" \
    "brown" \
    "aqua"