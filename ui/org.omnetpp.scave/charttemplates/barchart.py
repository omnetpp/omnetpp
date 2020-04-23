import pandas as pd
from omnetpp.scave import results, chart, plot, utils

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

# collect parameters for query
filter_expression = props["filter"]

# query scalar data into dataframe
df = results.get_scalars(filter_expression, include_attrs=True, include_itervars=True, include_runattrs=True)

if df.empty:
    plot.set_warning("The result filter returned no data.")
    exit(1)

groups = props["groups"].split(",")
series = props["series"].split(",")

if not groups[0] and not series[0]:
    print("The Groups and Series options were not set in the dialog, inferring them from the data.")
    g, s = ("module", "name") if len(df) == 1 else utils.pick_two_columns(df)
    if not g or not s:
        plot.set_warning("Please set the Groups and Series options in the dialog!")
        exit(1)
    groups, series = [g], [s]

for c in groups + series:
    df[c] = pd.to_numeric(df[c], errors="ignore")

df.sort_values(by=groups+series, axis='index', inplace=True)

names = utils.get_names_for_title(df, props)

df = pd.pivot_table(df, index=groups, columns=series, values='value')

utils.plot_bars(df, props, names)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
