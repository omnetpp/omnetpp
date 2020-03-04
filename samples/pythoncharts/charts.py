# This file will be imported by some chart scripts in the Importing.anf file.

from omnetpp.scave import results, utils, plot
import pandas as pd
import matplotlib.pyplot as plt


def chart_function():
    plt.plot([10, 20, 30], [4, 3, 8])


def chart_function_native():
    plot.plot([10, 20, 30], [4, 3, 8])


def chart_function_bars():
    df = results.get_scalars("name =~ rxBytes:sum OR name =~ txBytes:sum", include_runattrs=True)

    names = utils.get_names_for_title(df, dict())
    
    df = pd.pivot_table(df, index="name", columns="module", values='value')
    
    utils.preconfigure_plot(dict())
    utils.plot_bars(df, dict(), names)
    utils.postconfigure_plot(dict())
