import pandas as pd
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import os
from scipy.stats import t
import sys
import random
import math
import argparse

default_colors = ["pink", "orange", "purple", "red", "green", "black", "grey", "brown", "blue", "aqua", "yellow"]
font = {   
            'weight' : 'normal',
            'size' : '12'
       }

color_linestyles = {
    "pink": (0, (1,3)), # Loosely dotted
    "orange": 'solid',
    "purple": 'dashed',
    "red": 'dashdot',
    "green":(0, (3,1,1,1,1,1)), # Densely dashdotdotted
    "black": (0, (3,1,1,1)), # Densly dashdotted
    "grey": (0, (1,1)), # Dotted
    "brown": (0, (3,1,1,1,1,1,1,1)),
    "aqua": (5, (10, 3)) # Long dash with offset
}

fig, ax = plt.subplots(nrows=1, ncols=1)
def plot_statistics(ARR, loc_ax, color, linestyle, label, x_axis_label, y_axis_label):

    lens = [len(i) for i in ARR]
    max_len = np.max(lens)
    arr = np.ma.empty((max_len, len(ARR)))
    arr.mask = True
    for i, l in enumerate(ARR):
        arr[:len(l),i] = l
    MAX_ARR = arr.max(axis = -1)
    MEAN_ARR = arr.mean(axis = -1)
    STD_ARR = arr.std(axis = -1)
    print(MEAN_ARR[-1])

    print(max_len)

    loc_ax.fill_between(np.arange(1, max_len + 1), (MEAN_ARR - STD_ARR), (MEAN_ARR + STD_ARR), color = color, alpha = .3)
    loc_ax.set_xticks(np.arange(0, 2000 + 1, 2000 / 10), minor=False)
    loc_ax.set_xlabel(x_axis_label, fontsize=14)
    loc_ax.set_ylabel(y_axis_label, fontsize=14)
    loc_ax.plot(MEAN_ARR, color = color, label = label, linestyle=linestyle, linewidth=2.0)

def main():

    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--input', nargs='+', required=True)
    parser.add_argument('-l', '--labels', nargs='+', default=[])
    parser.add_argument('-c', '--colors', nargs='+', default=[])
    parser.add_argument('-x', default="NUMBER OF FUZZING ITERATIONS")
    parser.add_argument('-y', default="CODE COVERAGE")
    parser.add_argument('-s', '--use_line_styles', action='store_true', help="Use different linestyles for different colors")
    parser.add_argument('-o', '--output', required=False, help="Output filename")

    args = parser.parse_args()

    cov_dir_names = args.input
    labels = args.labels
    colors = args.colors
    x_axis_label = args.x
    y_axis_label = args.y
    use_line_styles = args.use_line_styles
    output = args.output

    print(f"Cov dir names: {cov_dir_names}")

    if len(labels) == 0:
        labels = cov_dir_names
    else:
        if len(cov_dir_names) != len(labels):
            print("Please specify the labels for all (or none) of the coverage dir names")
            exit(-1)

    if len(colors) == 0:
        colors = default_colors
    else:
        if len(colors) != len(cov_dir_names):
            print("Please specify the colors for all (or none) of the coverage dir names")
            exit(-1)

    j = 0
    for i in range(len(cov_dir_names)):
        try:
            directory_name = str(cov_dir_names[i])
        except:
            print(f"Directory name not recognized: {directory_name}")
            continue
        print(directory_name)
        directory = os.fsencode(directory_name)

        ARR_BUCKETS = []
        ARR_TOTAL = []
        ARR_CURRENT = []

        for file in os.listdir(directory):
            filename = os.fsdecode(file)
            try:
                table = pd.read_csv(os.path.join(directory_name, filename), sep=',', header=None,  usecols=[0, j*3 + 1, j*3 + 2, j*3 + 3])
            except:
                print(f"Could not read csv: {filename}")
            table = table.ffill()

            ARR_BUCKETS.append(table.iloc[:, -1].values.tolist())
            ARR_TOTAL.append(table.iloc[:, -2].values.tolist())
            ARR_CURRENT.append(table.iloc[:, -3].values.tolist())           

        print(f'THERE ARE {len(ARR_TOTAL)} SAMPLES IN DIRECTORY {directory_name}')
        color = colors[i] if len(colors) > i else list(np.random.choice(range(256), size=3))
        name = labels[i]
        linestyle = 'solid'
        if use_line_styles:
            if color in color_linestyles.keys():
                linestyle = color_linestyles[color]
        plot_statistics(ARR_BUCKETS, ax, color, linestyle, name, x_axis_label, y_axis_label)
        #plot_statistics(ARR_TOTAL, ax, color, linestyle, name, x_axis_label, y_axis_label)
        #plot_statistics(ARR_CURRENT, 2, color, name)

    # ax.title.set_text("FILLED BUCKETS")
    # ax[1].title.set_text("ACCUMULATIVE EDGE COVERAGE")
    # ax[2].title.set_text("EDGE COVERAGE")
    fig.set_figwidth(10)
    fig.set_figheight(6)
    plt.legend(loc="lower right", prop=font, framealpha=0.25, handlelength=4)
    if output:
        plt.savefig(output, bbox_inches='tight')
    else:
        plt.show()

if __name__ == "__main__":
    main()
