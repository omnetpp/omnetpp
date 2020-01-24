import numpy as np
import pandas as pd
import matplotlib as mpl
import matplotlib.cm as cm
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from omnetpp.scave import results, chart, utils, plot

props = chart.get_properties()
utils.update_matplotlib_rcparams(props)
utils.update_matplotlib_rcparams(utils.parse_matplotlib_rcparams(props["matplotlibrc"] or ""))

if not (props["x_attr"] and props["y_attr"]):
    raise Exception("Please select axis attributes in the properties dialog!")

df = results.get_scalars(props["scalar_filter"], include_attrs=True, include_itervars=True, include_runattrs=True)

df[props["x_attr"]] = pd.to_numeric(df[props["x_attr"]])
df[props["y_attr"]] = pd.to_numeric(df[props["y_attr"]])

# print(df)

title_col, legend_cols = utils.extract_label_columns(df)

print(legend_cols)
title = str(list(df[title_col])[0]) if title_col else None

df = pd.pivot_table(df, columns=[props["x_attr"]], index=props["y_attr"], values="value", dropna=False)

ax = plt.gcf().add_subplot(111, projection='3d')

print(df)

X, Y = np.meshgrid(df.columns, df.index)

colormap = props["colormap"]
if not colormap:
    colormap = None
cmap = cm.get_cmap(colormap)  # Get desired colormap - you can change this!
max_height = np.max(df.values)  # get range of colorbars so we can normalize
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

if title:
    utils.set_plot_title(title)
