import numpy as np
import pandas as pd
import matplotlib.cm as cm
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from omnetpp.scave import results, chart, utils

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

# collect parameters for query
filter_expression = props["filter"]
include_fields = props["include_fields"] == "true"
xaxis_itervar = props["xaxis_itervar"]
yaxis_itervar = props["yaxis_itervar"]

# query data into a data frame
try:
    df = results.get_scalars(filter_expression, include_fields=include_fields, include_attrs=True, include_runattrs=True, include_itervars=True)
except results.ResultQueryError as e:
    raise chart.ChartScriptError("Error while querying results: " + str(e))

if df.empty:
    raise chart.ChartScriptError("The result filter returned no data.")

if not xaxis_itervar and not yaxis_itervar:
    print("The X Axis and Y Axis options were not set in the dialog, inferring them from the data..")
    xaxis_itervar, yaxis_itervar = utils.select_best_partitioning_column_pair(df, props)
if not xaxis_itervar or not yaxis_itervar:
    raise chart.ChartScriptError("Please set both the X Axis and Y Axis options in the dialog - or neither, for automatic selection!")

utils.assert_columns_exist(df, [xaxis_itervar], "The iteration variable for the X axis could not be found")
utils.assert_columns_exist(df, [yaxis_itervar], "The iteration variable for the Y axis could not be found")

if xaxis_itervar == yaxis_itervar:
    raise chart.ChartScriptError("The itervar for the X and Y axes are the same: " + xaxis_itervar)

df[xaxis_itervar] = pd.to_numeric(df[xaxis_itervar], errors="ignore")
df[yaxis_itervar] = pd.to_numeric(df[yaxis_itervar], errors="ignore")

title_cols, legend_cols = utils.extract_label_columns(df, props)

title = utils.make_chart_title(df, title_cols)

unique_names = df["name"].unique()

if len(unique_names) != 1:
    raise chart.ChartScriptError("Selected scalars must share the same name.")

scalar_name = unique_names[0]

df = pd.pivot_table(df, columns=[xaxis_itervar], index=yaxis_itervar, values="value", dropna=False)

ax = plt.gcf().add_subplot(111, projection='3d')

X, Y = np.meshgrid(df.columns, df.index)

colormap = props["colormap"]
if not colormap:
    colormap = None
colormap_reverse = props["colormap_reverse"]
if colormap_reverse == "true" and colormap:
    colormap += "_r"

cmap = cm.get_cmap(colormap)
max_height = np.max(df.values)  # get range of values so we can normalize
min_height = np.min(df.values)
top = df.values.ravel()
rgba = [cmap((k - min_height) / max_height) for k in top]

type = props["chart_type"]

if type == "bar":
    bottom = np.zeros_like(top)
    width = depth = 0.5
    ax.bar3d(X.ravel() - width / 2, Y.ravel() - depth / 2, bottom, width, depth, top, shade=True, color=rgba)
elif type == "points":
    ax.scatter(X, Y, df, c=top, cmap=cmap, depthshade=True)
elif type == "surface":
    ax.plot_surface(X, Y, df, cmap=cmap, shade=True)
elif type == "trisurf":
    ax.plot_trisurf(X.ravel(), Y.ravel(), df.values.ravel(), cmap=cmap, shade=True)

ax.set_xticks(df.columns)
ax.get_xaxis().set_label_text(df.columns.name)

ax.set_yticks(df.index)
ax.get_yaxis().set_label_text(df.index.name)

title = scalar_name + " vs. " + xaxis_itervar + " and " + yaxis_itervar
if "title" in props and props["title"]:
    title = props["title"]
utils.set_plot_title(title)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)

