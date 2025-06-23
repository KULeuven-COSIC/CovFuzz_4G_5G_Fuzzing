#!/bin/bash
python3 ../helper_scripts/count_the_number_of_mutations_in_logs.py -i \
    paper_log/ul_no_cov_k_0.25 \
    paper_log/ul_srsue_beta_2_k_3 \
    paper_log/ul_srsue_beta_4_k_3 \
    paper_log/ul_srsue_beta_6_k_3 \
    paper_log/ul_srsue_beta_8_k_3 \
    -l \
    "RANDOM FUZZER (\$k\$=0.25)" \
    "BLACK-BOX UL FUZZER (\$\beta\$=2, \$k\$=3)" \
    "BLACK-BOX UL FUZZER (\$\beta\$=4, \$k\$=3)" \
    "BLACK-BOX UL FUZZER (\$\beta\$=6, \$k\$=3)" \
    "BLACK-BOX UL FUZZER (\$\beta\$=8, \$k\$=3)" \
    -d \
    UPLINK \
    -c \
    "orange" \
    "red" \
    "black" \
    "brown" \
    "aqua"