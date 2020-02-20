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
df = results.get_scalars(filter_expression, include_attrs=True, include_itervars=True)

if not xaxis_itervar and not iso_itervar:
    print("The X Axis and Iso Line options were not set in the dialog, inferring them from the data.")
    title_col, label_cols = utils.extract_label_columns(df)
    label_cols = [l[1] for l in label_cols]
    if len(label_cols) == 0:
        plot.set_warning("Please set the X Axis and Iso Lines options in the dialog!")
        exit(1)
    if len(label_cols) == 1:
        xaxis_itervar = title_col
        iso_itervar = label_cols[0]
    if len(label_cols) >= 2:
        xaxis_itervar = label_cols[0]
        iso_itervar = label_cols[1]


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


p.xlabel(xaxis_itervar)
p.ylabel(scalar_name)

for i, c in enumerate(df.columns):
    style = utils._make_line_args(props, c, df)
    p.plot(pd.to_numeric(df.index.values), df[c].values, label=iso_itervar + "=" + str(df[c].name), **style)

utils.set_plot_title(scalar_name + " vs. " + xaxis_itervar)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
