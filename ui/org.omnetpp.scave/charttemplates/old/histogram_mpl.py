from omnetpp.scave import results, chart, utils, plot
import matplotlib.pyplot as plt

props = chart.get_properties()
utils.update_matplotlib_rcparams(props)
utils.update_matplotlib_rcparams(utils.parse_matplotlib_rcparams(props["matplotlibrc"] or ""))

# This expression selects the results (you might be able to logically simplify it)
filter_expression = props["filter"]

# The data is returned as a Pandas DataFrame
df = results.get_histograms(filter_expression, include_attrs=True, include_itervars=True)

# You can perform any transformations on the data here

print(df)

title, legend = utils.extract_label_columns(df)

df.sort_values(by=[l for i, l in legend], axis='index', inplace=True)

for t in df.itertuples(index=False):
    plt.hist(bins=t.binedges, x=t.binedges[:-1], weights=t.binvalues, label=utils.make_legend_label(legend, t), histtype='stepfilled')

plt.legend()
plt.grid()

title = chart.get_property("title")
if not title:
    title_cols, legend_cols = utils.extract_label_columns(df)
    title = utils.make_chart_title(df, title_cols, legend_cols)
utils.set_plot_title(title)
