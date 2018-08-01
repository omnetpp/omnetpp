# This file will be imported by some chart scripts in the NativePlottingExamples.anf file.

import matplotlib.pyplot as plt

def chart_function():
    plt.plot([10, 20, 30], [4, 3, 8])

def chart_function_native(results, chart):
    df = results.getScalars("name(rxBytes:sum) OR name(txBytes:sum)")
    df = results.pivotScalars(df, columns=["name", "replication"], index=["module"])
    print(df)
    chart.plotScalars(df)


def importing_4():
    plt.plot([10, 20], [2, 3])

def importing_5(results, chart):
    df = results.getScalars("name(rxBytes:sum) OR name(txBytes:sum)")
    df = results.pivotScalars(df, columns=["name", "replication"], index=["module"])
    print(df)
    chart.plotScalars(df)

scripts = {
    "importing 4": importing_4,
    "importing 5": importing_5
}
