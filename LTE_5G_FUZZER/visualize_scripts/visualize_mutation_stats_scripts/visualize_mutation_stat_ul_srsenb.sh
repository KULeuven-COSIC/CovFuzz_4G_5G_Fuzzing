#!/bin/bash
python3 ../helper_scripts/count_the_number_of_mutations_in_logs.py -i \
    paper_log/ul_no_cov_k_0.25 \
    paper_log/ul_srsenb_beta_1_k_3 \
    paper_log/ul_srsenb_beta_2_k_3 \
    paper_log/ul_srsenb_beta_3_k_3 \
    paper_log/ul_srsenb_beta_4_k_3 \
    paper_log/ul_srsenb_beta_5_k_3 \
    -l \
    "RANDOM FUZZER (\$k\$=0.25)" \
    "GREY-BOX UL FUZZER (\$\beta\$=1, \$k\$=3)" \
    "GREY-BOX UL FUZZER (\$\beta\$=2, \$k\$=3)" \
    "GREY-BOX UL FUZZER (\$\beta\$=3, \$k\$=3)" \
    "GREY-BOX UL FUZZER (\$\beta\$=4, \$k\$=3)" \
    "GREY-BOX UL FUZZER (\$\beta\$=5, \$k\$=3)" \
    -d \
    UPLINK