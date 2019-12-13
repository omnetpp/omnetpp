# This file will be imported by some chart scripts in the NativePlottingExamples.anf file.

from omnetpp.scave import chart, results
import pandas as pd
import matplotlib.pyplot as plt

def chart_function():
    plt.plot([10, 20, 30], [4, 3, 8])

def chart_function_native():
    df = results.get_scalars("name =~ rxBytes:sum OR name =~ txBytes:sum", include_runattrs=True)
    
    print(chart.extract_label_columns(df))
    df = pd.pivot_table(df, columns=["name", "replication"], index=["module"])
    print(df)
    chart.plot_scalars(df)
    


def importing_4():
    plt.plot([10, 20], [2, 3])

def importing_5():
    df = results.get_scalars("name =~ rxBytes:sum OR name =~ txBytes:sum", include_runattrs=True)
    
    print(chart.extract_label_columns(df))
    df = pd.pivot_table(df, columns=["name", "replication"], index=["module"])
    print(df)
    chart.plot_scalars(df)

scripts = {
    "importing 4": importing_4,
    "importing 5": importing_5
}
