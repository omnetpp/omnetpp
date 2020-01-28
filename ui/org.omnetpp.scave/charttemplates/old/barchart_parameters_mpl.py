from omnetpp.scave import results, chart, utils, plot
import matplotlib.pyplot as plt
import pandas as pd

props = chart.get_properties()
utils.update_matplotlib_rcparams(props)
utils.update_matplotlib_rcparams(utils.parse_matplotlib_rcparams(props["matplotlibrc"] or ""))

# This expression selects the results (you might be able to logically simplify it)
filter_expression = props["filter"]

# TODO: maybe try to do something clever with numbers with units, instead of discarding them because of as_numeric?
# The data is returned as a Pandas DataFrame
df = results.get_parameters(filter_expression, include_attrs=True, include_itervars=True, as_numeric=True)

# You can perform any transformations on the data here

print(df)

title, legend = utils.extract_label_columns(df)

for i, c in legend:
    df[c] = pd.to_numeric(df[c], errors="ignore")

df.sort_values(by=[l for i, l in legend], axis='index', inplace=True)

utils.set_plot_title(utils.make_chart_title(df, title, legend))

if len(legend) == 2:
    df = pd.pivot_table(df, index=legend[0][1], columns=legend[1][1], values='value')
else:
    df = pd.pivot_table(df, index=[l for i, l in legend], values='value')

print(df)

if False:
    df = df.transpose()

df.plot(kind='bar', ax=plt.gca())

plt.legend()
plt.grid()
