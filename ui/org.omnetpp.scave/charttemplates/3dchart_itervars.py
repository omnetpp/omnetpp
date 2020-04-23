import numpy as np
import pandas as pd
import matplotlib.cm as cm
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from omnetpp.scave import results, chart, utils, plot

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

# collect parameters for query
filter_expression = props["filter"]
xaxis_itervar = props["xaxis_itervar"]
yaxis_itervar = props["yaxis_itervar"]

# query data into a data frame
df = results.get_scalars(filter_expression, include_attrs=True, include_itervars=True)

if df.empty:
    plot.set_warning("The result filter returned no data.")
    exit(1)

if not xaxis_itervar and not yaxis_itervar:
    print("The X Axis and Y Axis options were not set in the dialog, inferring them from the data..")
    xaxis_itervar, yaxis_itervar = utils.pick_two_columns(df)
    if not xaxis_itervar or not yaxis_itervar:
        plot.set_warning("Please set the X Axis and Y Axis options in the dialog!")
        exit(1)

utils.assert_columns_exist(df, [xaxis_itervar, yaxis_itervar])

df[xaxis_itervar] = pd.to_numeric(df[xaxis_itervar], errors="ignore")
df[yaxis_itervar] = pd.to_numeric(df[yaxis_itervar], errors="ignore")

title_col, legend_cols = utils.extract_label_columns(df)

title = str(list(df[title_col])[0]) if title_col else None

unique_names = df["name"].unique()

if len(unique_names) != 1:
    plot.set_warning("Scalar names should be unique")
    exit(1)

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

utils.set_plot_title(scalar_name + " vs. " + xaxis_itervar + " and " + yaxis_itervar)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)

