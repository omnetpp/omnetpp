import pandas as pd
from omnetpp.scave import results, chart, plot, utils

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

# collect parameters for query
filter_expression = props["filter"]

# query scalar data into dataframe
df = results.get_scalars(filter_expression, include_attrs=True, include_itervars=True, include_runattrs=True)

groups = props["groups"].split(",")
series = props["series"].split(",")

if not groups[0] and not series[0]:
    print("The Groups and Series options were not set in the dialog, inferring them from the data.")
    title_col, label_cols = utils.extract_label_columns(df)
    label_cols = [l[1] for l in label_cols]
    if len(label_cols) == 0:
        plot.set_warning("Please set the Groups and Series options in the dialog!")
        exit(1)
    if len(label_cols) == 1:
        groups = [title_col]
        series = [label_cols[0]]
    if len(label_cols) >= 2:
        groups = [label_cols[0]]
        series = [label_cols[1]]

for c in groups + series:
    df[c] = pd.to_numeric(df[c], errors="ignore")

df.sort_values(by=groups+series, axis='index', inplace=True)

names = df["title" if "title" in df else "name"].unique()

df = pd.pivot_table(df, index=groups, columns=series, values='value')

utils.plot_bars(df, props, names=names)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
