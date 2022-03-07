import numpy as np
import pandas as pd
from omnetpp.scave import results, chart, utils, ideplot

props = chart.get_properties()


def aggfunc(x):
    if type(x.values[0]) == float:
        return np.nanmean(x, dtype=np.float64)

    if type(x.values[0]) == str:
        return ", ".join(x.unique())

    return np.nanmean(x, dtype=np.float64)


def get_data(filter):
    try:
        sc = results.get_scalars(filter, include_attrs=True)
        iv = results.get_itervars(filter)
        ra = results.get_runattrs(filter)
    except results.ResultQueryError as e:
        raise chart.ChartScriptError("Error while querying results: " + str(e))

    df = pd.concat([sc, iv, ra])

    df["value"] = pd.to_numeric(df["value"], errors="ignore")
    df = pd.pivot_table(df, columns="name", index="runID", dropna=False, aggfunc=aggfunc)

    return df


def filter_for(what):
    return "(" + props["common_filter"] + ") AND (" + props[what] + ")"


xa = get_data(filter_for("xaxis_filter"))
ya = get_data(filter_for("yaxis_filter"))
iso = get_data(filter_for("iso_filter"))

cols = xa["value"].columns.values
if len(cols) != 1:
    raise chart.ChartScriptError("X axis filter must result in exactly one value column, instead of " + str(cols))
xaxis_column = cols[0]

cols = ya["value"].columns.values
if len(cols) != 1:
    raise chart.ChartScriptError("Y axis filter must result in exactly one variable, instead of: " + ", ".join(cols) + "\nTry narrowing down the filter expression to one of these by name!")
yaxis_column = cols[0]

cols = iso["value"].columns.values
if len(cols) != 1:
    raise chart.ChartScriptError("Iso line filter must result in exactly one value column, instead of " + str(cols))
iso_column = cols[0]

try:
    df = xa.join(ya, how='outer').join(iso, how='outer')
except:
    import traceback
    traceback.print_exc()

values = df["value"]
titles = df["title"] if "title" in df else None
units = df["unit"] if "unit" in df else None

values[yaxis_column] = pd.to_numeric(values[yaxis_column], errors="ignore")

df = pd.pivot_table(values, columns=iso_column, index=xaxis_column, values=yaxis_column, dropna=False)

df.reset_index(inplace=True)
df.rename({xaxis_column: "time"}, axis="columns", inplace=True)

ideplot.plot_vectors(df)

title = props["title"] or ', '.join(titles[yaxis_column].unique())
utils.set_plot_title(title)

if units is not None:
    ideplot.set_property("Y.Axis.Title", "[" + ', '.join(units[yaxis_column].unique()) + "]")
