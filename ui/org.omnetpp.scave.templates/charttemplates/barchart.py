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
title_names = utils.get_names_for_title(df, props)
names = title_names[0] if len(title_names) == 1 else None
confidence_level_str = props["confidence_level"] if "confidence_level" in props else "none"

# pivoting and plotting
df.sort_values(by=groups+series, axis='index', inplace=True)


def aggfunc(values):
    if values.empty:
        return None
    if values.dtype == np.float64 or values.dtype == np.int64:
        return values.mean()
    if values.dtype == object:
        uniq = values.unique()
        if len(uniq) == 1:
            return uniq[0]
        else:
            return uniq[0] + ", etc."
    return "<?>"

metadf = pd.pivot_table(df, index=series, aggfunc=aggfunc, dropna=False)
del metadf["value"]

if confidence_level_str == "none":
    df = pd.pivot_table(df, index=series, columns=groups, values='value', dropna=False)
    utils.plot_bars(df, props, names, None, metadf)
else:
    confidence_level = float(confidence_level_str[:-1])/100
    def conf_intv(values):
        return utils.confidence_interval(confidence_level, values)

    pivoted = pd.pivot_table(df, index=series, columns=groups, values="value", aggfunc=[np.mean, conf_intv], dropna=False)
    valuedf = pivoted["mean"]
    errorsdf = pivoted["conf_intv"]
    if errorsdf.isna().values.all():
        errorsdf = None

    utils.plot_bars(valuedf, props, names, errorsdf, metadf)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
