import math
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from omnetpp.scave import results, chart, utils, ideplot

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

# collect parameters for query
filter_expression = props["filter"]
include_fields = props["include_fields"] == "true"
xaxis_itervar = props["xaxis_itervar"]
group_by = utils.split(props["group_by"])

# query data into a data frame
try:
    df = results.get_scalars(filter_expression, include_fields=include_fields, include_attrs=True, include_runattrs=True, include_itervars=True)
except ValueError as e:
    raise chart.ChartScriptError("Error while querying results: " + str(e))

if df.empty:
    raise chart.ChartScriptError("The result filter returned no data.")

if not xaxis_itervar and not group_by:
    print("The 'X Axis' and 'Group By' options were not set in the dialog, inferring them from the data..")
    xaxis_itervar, group_by = utils.pick_two_columns(df, props)
    group_by = [group_by] if group_by else []
    print("X Axis: " + xaxis_itervar + ", Group By: " + ",".join(group_by))

if xaxis_itervar:
    utils.assert_columns_exist(df, [xaxis_itervar], "The iteration variable for the X axis could not be found")
    df[xaxis_itervar] = pd.to_numeric(df[xaxis_itervar], errors="ignore")
else:
    raise chart.ChartScriptError("Please select the iteration variable for the X axis!")

if xaxis_itervar in group_by:
    raise chart.ChartScriptError("X axis column also in grouper columns: " + xaxis_itervar)

if group_by:
    utils.assert_columns_exist(df, group_by, "An iteration variable for grouping could not be found")
    for iv in group_by:
        if iv:
            df[iv] = pd.to_numeric(df[iv], errors="ignore")

uninteresting = ["runID", "value", "datetime", "datetimef", "processid",
                "iterationvars", "iterationvarsf", "iterationvarsd",
                "measurement", "replication", "runnumber", "seedset",
                "title", "source", "interpolationmode"]

for c in df:
    ul = len(df[c].unique())
    if ul > 1 and c != xaxis_itervar and c not in group_by and c not in uninteresting:
        print("Points are averaged from an overall", ul, "unique", c, "values.")

scalar_names = ', '.join(df["name"].unique())

confidence_level_str = props["confidence_level"] if "confidence_level" in props else "none"

if confidence_level_str == "none":
    df = pd.pivot_table(df, values="value", columns=group_by, index=xaxis_itervar)
    errors_df = None
else:
    confidence_level = float(confidence_level_str[:-1])/100
    def conf_intv(values):
        return utils.confidence_interval(confidence_level, values)
    pivoted = pd.pivot_table(df, values="value", columns=group_by, index=xaxis_itervar if xaxis_itervar else "name", aggfunc=[np.mean, conf_intv], dropna=False)
    df = pivoted["mean"]
    errors_df = pivoted["conf_intv"]
    if errors_df.isna().values.all():
        errors_df = None

try:
    xs = pd.to_numeric(df.index.values)
    ideplot.xlabel(xaxis_itervar)
except:
    xs = np.zeros_like(df.index.values)

ideplot.ylabel(scalar_names)

for c in df.columns:
    style = utils._make_line_args(props, c, df)
    if len(xs) < 2 and style["marker"] == ' ':
        style["marker"] = '.'

    ys = df[c].values
    if group_by:
        names = df[c].name
        if type(names) != tuple:
            names = [names]
        label = ', '.join([str(a) + "=" + str(b) for a, b in zip(group_by, names)])
    else:
        label = scalar_names
    ideplot.plot(xs, ys, label=label, **style)

    if errors_df is not None and not ideplot.is_native_plot():
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
