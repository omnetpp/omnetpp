from omnetpp.scave import results, chart, vectorops as ops
import matplotlib.pyplot as plt

params = chart.get_properties()

# This expression selects the results (you might be able to logically simplify it)

filter_expression = params["filter"]

# The data is returned as a Pandas DataFrame
df = results.get_vectors(filter_expression, include_attrs=True, include_itervars=True)

# This is where the vector operations will be added:
# <|> vectorops marker <|>

# You can perform any transformations on the data here

print(df)

plt.xlabel('Simulation time (s)')

title, legend = chart.extract_label_columns(df)

for t in df.itertuples(index=False):
    style = dict()
    interp = t.interpolationmode if 'interpolationmode' in df else 'sample-hold' if 'enum' in df else None
    if interp:
        if interp == "none":
            style['linestyle'] = ' '
            style['marker'] = '.'
        elif interp == "linear":
            pass # nothing to do
        elif interp == "sample-hold":
            style['drawstyle'] = 'steps-post'
        elif interp == "backward-sample-hold":
            style['drawstyle'] = 'steps-pre'

    plt.plot(t.vectime, t.vecvalue, label=chart.make_legend_label(legend, t), **style)

plt.title(chart.make_chart_title(df, title, legend))

plt.legend()
plt.grid()