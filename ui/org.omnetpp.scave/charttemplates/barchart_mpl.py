from omnetpp.scave import results, chart, utils, plot
import matplotlib.pyplot as plt
import pandas as pd

params = chart.get_properties()

# This expression selects the results (you might be able to logically simplify it)
filter_expression = params["filter"]

# The data is returned as a Pandas DataFrame
df = results.get_scalars(filter_expression, include_attrs=True, include_itervars=True)

# You can perform any transformations on the data here

print(df)

title, legend = utils.extract_label_columns(df)

for i, c in legend:
    df[c] = pd.to_numeric(df[c], errors="ignore")

df.sort_values(by=[l for i, l in legend], axis='index', inplace=True)

plt.title(utils.make_chart_title(df, title, legend))

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