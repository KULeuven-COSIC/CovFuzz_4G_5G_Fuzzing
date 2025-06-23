import pandas as pd
import os
import sys

if len(sys.argv) < 2:
    print("Please provide a directory name")
    exit(-1)

for i in range(1,len(sys.argv)):
    directory_name = str(sys.argv[i])
    directory = os.fsencode(directory_name)

    for file in os.listdir(directory):
        filename = os.fsdecode(file)

        table = pd.read_csv(directory_name + '/' + filename, sep=',', header=None, on_bad_lines='skip', usecols=[0,1,2,3,4,5,6], names=[i for i in range(7)])
        table.dropna()

        cols = list(table.columns)

        def swap(i, j):
            cols[i], cols[j] = cols[j], cols[i]

        swap(1,4)
        swap(2,5)
        swap(3,6)

        table = table[cols]
        table.to_csv(directory_name + '/' + filename, encoding='utf-8', index=False, header=False)