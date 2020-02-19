import pandas as pd
from omnetpp.scave import results, chart, plot, utils

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

if not props["groups"] or not props["series"]:
    plot.set_warning("Please select group and series attributes in the Properties dialog!")
    exit(1)

# collect parameters for query
filter_expression = props["filter"]
groups = props["groups"].split(",")
series = props["series"].split(",")

# query scalar data into dataframe
df = results.get_scalars(filter_expression, include_attrs=True, include_itervars=True, include_runattrs=True)

for c in groups + series:
    df[c] = pd.to_numeric(df[c], errors="ignore")

df.sort_values(by=groups+series, axis='index', inplace=True)

names = df["title" if "title" in df else "name"].unique()

df = pd.pivot_table(df, index=groups, columns=series, values='value')

utils.plot_bars(df, props, names=names)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
