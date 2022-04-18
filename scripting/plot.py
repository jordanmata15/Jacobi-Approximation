#!/bin/python3

import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np
import os
import pandas as pd
import seaborn as sns

curr_dir = os.path.dirname(os.path.realpath(__file__))
data_dir = os.path.join(os.path.join(curr_dir, ".."), "data")
file_path = os.path.join(data_dir, 'output.csv')

# do we want to write it out to a file? (if false, display on screen directly)
write_out_plots = True


def plot_2d(output_df):
    """ Plot a square 2D dataframe as a 2D heatmap.

    :param: output_df - The 2D dataframe to plot.
    """
    x = y = np.arange(0, 12, 1)
    X, Y = np.meshgrid(x, y)
    Z = output_df
    ax = sns.heatmap(output_df, cmap=cm.coolwarm)

    if (write_out_plots):
        plt.savefig(os.path.join(data_dir, "2d_plot.pdf"))
    else:
        plt.show()


def plot_3d(output_df):
    """ Plot a square 2D dataframe as a 3D heatmap. 
        The values at index i,j are the Z values.

    :param: output_df - The 2D dataframe to plot.
    """
    x = y = np.arange(0, 12, 1)
    X, Y = np.meshgrid(x, y)
    Z = output_df.T

    fig, ax = plt.subplots(subplot_kw={"projection": "3d"})
    ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
                    linewidth=0, antialiased=False)
    ax.view_init(20, -130)  # rotate the plot for best viewing angle

    if (write_out_plots):
        plt.savefig(os.path.join(data_dir, "3d_plot.pdf"))
    else:
        plt.show()


if __name__ == '__main__':
    output_df = pd.read_csv(file_path, header=None)
    plot_2d(output_df)
    plot_3d(output_df)
