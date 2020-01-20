import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from omnetpp.scave import results, chart, plot

pd.set_option('display.max_rows', 500)
pd.set_option('display.max_columns', 500)
pd.set_option('display.width', 1000)

params = chart.get_properties()


def aggfunc(x):
    if type(x.values[0]) == float:
        return np.nanmean(x, dtype=np.float64)

    if type(x.values[0]) == str:
        return ", ".join(x.unique())

    return np.nanmean(x, dtype=np.float64)


def get_data(filter):
    sc = results.get_scalars(filter, include_attrs=True)
    iv = results.get_itervars(filter)
    ra = results.get_runattrs(filter)

    df = pd.concat([sc, iv, ra])
    # print(df)

    df["value"] = pd.to_numeric(df["value"], errors="ignore")
    df = pd.pivot_table(df, columns="name", index="runID", dropna=False, aggfunc=aggfunc)
    # print(df)

    return df


def filter_for(what):
    return "(" + params["common_filter"] + ") AND (" + params[what] + ")"


xa = get_data(filter_for("xaxis_filter"))
ya = get_data(filter_for("yaxis_filter"))
iso = get_data(filter_for("iso_filter"))

cols = xa["value"].columns.values
if len(cols) != 1:
    raise Exception("X axis filter must result in exactly one value column, instead of " + str(cols))
xaxis_column = cols[0]

cols = ya["value"].columns.values
if len(cols) != 1:
    raise Exception("Y axis filter must result in exactly one variable, instead of: " + ", ".join(cols) + "\nTry narrowing down the filter expression to one of these by name!")
yaxis_column = cols[0]

cols = iso["value"].columns.values
if len(cols) != 1:
    raise Exception("Iso line filter must result in exactly one value column, instead of " + str(cols))
iso_column = cols[0]

try:
    df = xa.join(ya, how='outer').join(iso, how='outer')
except:
    import traceback
    traceback.print_exc()
print(df)

values = df["value"]
titles = df["title"] if "title" in df else None
units = df["unit"] if "unit" in df else None

values[yaxis_column] = pd.to_numeric(values[yaxis_column], errors="ignore")

# values[iso_column] = values[iso_column].apply(lambda x: 1 if x == 'true' else 0)
# values.plot.scatter( x=xaxis_column,y=yaxis_column, c=iso_column, colormap='rainbow')

df = pd.pivot_table(values, columns=iso_column, index=xaxis_column, values=yaxis_column, dropna=False)

if False:
    ax = plt.subplot(1, 2, 1)
    ax.table(cellText=df.values, colLabels=df.columns, loc='center')

    df.plot.line(ax=plt.subplot(1, 2, 2))
else:
    df.plot.line()

if titles is not None:
    plt.title(', '.join(titles[yaxis_column].unique()))

if units is not None:
    plt.gca().get_yaxis().set_label_text(plt.gca().get_yaxis().get_label_text() + " [" + ', '.join(units[yaxis_column].unique()) + "]")

plt.tight_layout()

plt.grid()
