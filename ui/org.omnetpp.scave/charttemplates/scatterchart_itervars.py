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
iso_itervar = props["iso_itervar"]

# query data into a data frame
df = results.get_scalars(filter_expression, include_runattrs=True, include_attrs=True, include_itervars=True)

if df.empty:
    plot.set_warning("The result filter returned no data.")
    exit(1)

if not xaxis_itervar and not iso_itervar:
    print("The X Axis and Iso Line options were not set in the dialog, inferring them from the data..")
    xaxis_itervar, iso_itervar = utils.pick_two_columns(df)
    if not xaxis_itervar:
        plot.set_warning("Please set the X Axis (and Iso Lines) options in the dialog!")
        exit(1)

utils.assert_columns_exist(df, [xaxis_itervar])
df[xaxis_itervar] = pd.to_numeric(df[xaxis_itervar], errors="ignore")

if iso_itervar:
    utils.assert_columns_exist(df, [xaxis_itervar])
    df[iso_itervar] = pd.to_numeric(df[iso_itervar], errors="ignore")

if not iso_itervar: # might be an empty string
    iso_itervar = None

if df.empty:
    plot.set_warning("Select scalars for the Y axis in the Properties dialog")
    exit(1)

unique_names = df["name"].unique()

if len(unique_names) != 1:
    plot.set_warning("Selected scalars must share the same name.")
    exit(1)

scalar_name = unique_names[0]

confidence_level_str = props["confidence_level"] if "confidence_level" in props else "none"

if confidence_level_str == "none":
    df = pd.pivot_table(df, values="value", columns=iso_itervar, index=xaxis_itervar)
    errors_df = None
else:
    confidence_level = float(confidence_level_str[:-1])/100
    conf_int = lambda values: utils.confidence_interval(confidence_level, values) if len(values) > 1 else math.nan
    pivoted = pd.pivot_table(df, values="value", columns=iso_itervar, index=xaxis_itervar, aggfunc=[np.mean, conf_int], dropna=False)
    df = pivoted["mean"]
    errors_df = pivoted["<lambda>"]

legend_cols, _ = utils.extract_label_columns(df)

p = plot if chart.is_native_chart() else plt


p.xlabel(xaxis_itervar)
p.ylabel(scalar_name)

try:
    xs = pd.to_numeric(df.index.values)
except:
    plot.set_warning("The X axis itervar is not purely numeric, this is not supported yet.")
    exit(1)

for c in df.columns:
    style = utils._make_line_args(props, c, df)
    ys = df[c].values
    p.plot(xs, ys, label=(iso_itervar + "=" + str(df[c].name) if iso_itervar else scalar_name), **style)

    if errors_df is not None and not chart.is_native_chart():
        style["linewidth"] = float(style["linewidth"])
        style["linestyle"] = "none"
        yerr = errors_df[c].values
        if props["error_style"] == "Error bars":
            p.errorbar(xs, ys, yerr=yerr, capsize=float(props["cap_size"]), **style)
        elif props["error_style"] == "Error band":
            plt.fill_between(xs, ys-yerr, ys+yerr, alpha=float(props["band_alpha"]))

utils.set_plot_title(scalar_name + " vs. " + xaxis_itervar)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
