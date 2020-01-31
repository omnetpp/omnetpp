import pandas as pd
import matplotlib.pyplot as plt
from omnetpp.scave import results, chart, utils, plot, vectorops as ops

# get chart properties
props = chart.get_properties()

# collect parameters for query
filter_expression = props["filter"]
xaxis_itervar = props["xaxis_itervar"]
iso_itervar = props["iso_itervar"]

if not xaxis_itervar or not iso_itervar:
    plot.set_warning("Select itervars in the Properties dialog!")
    exit(1)

# query vector data into a data frame
df = results.get_scalars(filter_expression, include_attrs=True, include_itervars=True)

df[xaxis_itervar] = pd.to_numeric(df[xaxis_itervar], errors="ignore")
df[iso_itervar] = pd.to_numeric(df[iso_itervar], errors="ignore")

if df.empty:
    plot.set_warning("Select scalars for the Y axis in the Properties dialog")
    exit(1)

unique_names = df["name"].unique()

if len(unique_names) != 1:
    plot.set_warning("Scalar names should be unique")
    exit(1)

scalar_name = unique_names[0]

df = pd.pivot_table(df, values="value", columns=iso_itervar, index=xaxis_itervar)
print(df)
legend_cols, _ = utils.extract_label_columns(df)

p = plot if chart.is_native_chart() else plt


utils.preconfigure_plot(props)

p.xlabel(xaxis_itervar)
p.ylabel(scalar_name)

for i, c in enumerate(df.columns):
    style = utils._make_line_args(props, c, df)
    if chart.is_native_chart():
        style['key'] = str(i)  # khmm..
    p.plot(pd.to_numeric(df.index.values), df[c].values, label=iso_itervar + "=" + str(df[c].name), **style)

utils.set_plot_title(scalar_name + " vs. " + xaxis_itervar)

utils.postconfigure_plot(props)
