import math
import numpy as np
import pandas as pd
from omnetpp.scave import results, chart, ideplot, utils

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

# collect parameters for query
filter_expression = props["filter"]
include_fields = props["include_fields"] == "true"

# query scalar data into dataframe
try:
    df = results.get_scalars(filter_expression, include_fields=include_fields, include_attrs=True, include_runattrs=True, include_itervars=True)
except ValueError as e:
    raise chart.ChartScriptError("Error while querying results: " + str(e))

if df.empty:
    raise chart.ChartScriptError("The result filter returned no data.")

# determine "groups" and "series" for pivoting, and check them
groups = utils.split(props["groups"])
series = utils.split(props["series"])

if not groups and not series:
    print("The Groups and Series options were not set in the dialog, inferring them from the data.")
    g, s = ("module", "name") if len(df) == 1 else utils.pick_two_columns(df,props)
    groups, series = [g], [s]

if not groups or not groups[0] or not series or not series[0]:
    raise chart.ChartScriptError("Please set both the Groups and Series properties in the dialog - or neither, for automatic setup.")

common = list(set(groups) & set(series))
if common:
    raise chart.ChartScriptError("Overlap between Group and Series columns: " + ", ".join(common))

utils.assert_columns_exist(df, groups + series, "No such iteration variable or run attribute")

for c in groups + series:
    df[c] = pd.to_numeric(df[c], errors="ignore")

# names for title, confidence level
names = ", ".join(utils.get_names_for_title(df, props))
confidence_level_str = props["confidence_level"] if "confidence_level" in props else "none"

# pivoting and plotting
df.sort_values(by=groups+series, axis='index', inplace=True)
if confidence_level_str == "none":
    df = pd.pivot_table(df, index=groups, columns=series, values='value')
    utils.plot_bars(df, props, names)
else:
    confidence_level = float(confidence_level_str[:-1])/100
    def conf_intv(values):
        return utils.confidence_interval(confidence_level, values)
    df = pd.pivot_table(df, index=groups, columns=series, values='value', aggfunc=[np.mean, conf_intv], dropna=False)
    utils.plot_bars(df["mean"], props, names, df["conf_intv"])

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
