import math
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from omnetpp.scave import results, chart, utils, plot, vectorops as ops

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

# collect parameters for query
filter_expression = props["filter"]
xaxis_itervar = props["xaxis_itervar"]
iso_itervars = utils.split(props["iso_itervar"])

# query data into a data frame
try:
    df = results.get_scalars(filter_expression, include_runattrs=True, include_attrs=True, include_itervars=True)
except ValueError as e:
    plot.set_warning("Error while querying results: " + str(e))
    exit(1)

if df.empty:
    plot.set_warning("The result filter returned no data.")
    exit(1)

if not xaxis_itervar and not iso_itervars:
    print("The 'Itervar for X Axis' and 'Itervar for iso line' options were not set in the dialog, inferring them from the data..")
    xaxis_itervar, iso_itervar = utils.pick_two_columns(df)
    iso_itervars = [iso_itervar] if iso_itervar else []
    print("X axis: " + xaxis_itervar + " iso lines: " + ",".join(iso_itervars))

if xaxis_itervar:
    utils.assert_columns_exist(df, [xaxis_itervar], "The iteration variable for the X axis could not be found")
    df[xaxis_itervar] = pd.to_numeric(df[xaxis_itervar], errors="ignore")
else:
    plot.set_warning("Please select the iteration variable for the X axis!")
    exit(1)

if xaxis_itervar in iso_itervars:
    plot.set_warning("X axis column also in iso line columns: " + xaxis_itervar)
    exit(1)

if iso_itervars:
    utils.assert_columns_exist(df, iso_itervars, "An iteration variable for the iso lines could not be found")
    for iv in iso_itervars:
        if iv:
            df[iv] = pd.to_numeric(df[iv], errors="ignore")

uninteresting = ["runID", "value", "datetime", "iterationvars",
                "iterationvarsf", "measurement", "processid",
                "replication", "runnumber", "seedset", "title",
                "source", "interpolationmode"]

for c in df:
    ul = len(df[c].unique())
    if ul > 1 and c != xaxis_itervar and c not in iso_itervars and c not in uninteresting:
        print("Points are averaged from an overall", ul, "unique", c, "values.")

scalar_names = ', '.join(df["name"].unique())

confidence_level_str = props["confidence_level"] if "confidence_level" in props else "none"

if confidence_level_str == "none":
    df = pd.pivot_table(df, values="value", columns=iso_itervars, index=xaxis_itervar)
    errors_df = None
else:
    confidence_level = float(confidence_level_str[:-1])/100
    conf_int = lambda values: utils.confidence_interval(confidence_level, values) if len(values) > 1 else math.nan

    pivoted = pd.pivot_table(df, values="value", columns=iso_itervars, index=xaxis_itervar if xaxis_itervar else "name", aggfunc=[np.mean, conf_int], dropna=False)
    df = pivoted["mean"]
    errors_df = pivoted["<lambda>"]

legend_cols, _ = utils.extract_label_columns(df)

try:
    xs = pd.to_numeric(df.index.values)
    plot.xlabel(xaxis_itervar)
except:
    xs = np.zeros_like(df.index.values)

plot.ylabel(scalar_names)

for c in df.columns:
    style = utils._make_line_args(props, c, df)
    if len(xs) < 2 and style["marker"] == ' ':
        style["marker"] = '.'

    ys = df[c].values
    if iso_itervars:
        names = df[c].name
        if type(names) != tuple:
            names = [names]
        label = ', '.join([str(a) + "=" + str(b) for a, b in zip(iso_itervars, names)])
    else:
        label = scalar_names
    plot.plot(xs, ys, label=label, **style)

    if errors_df is not None and not plot.is_native_plot():
        style["linewidth"] = float(style["linewidth"])
        style["linestyle"] = "none"
        yerr = errors_df[c].values

        if props["error_style"] == "Error bars":
            plt.errorbar(xs, ys, yerr=yerr, capsize=float(props["cap_size"]), **style)
        elif props["error_style"] == "Error band":
            plt.fill_between(xs, ys-yerr, ys+yerr, alpha=float(props["band_alpha"]))

title = (scalar_names + " vs. " + xaxis_itervar) if xaxis_itervar else scalar_names
if "title" in props and props["title"]:
    title = props["title"]
utils.set_plot_title(title)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
