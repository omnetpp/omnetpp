from omnetpp.scave import results, chart, utils, plot, vectorops as ops
import matplotlib.pyplot as plt
import math

props = chart.get_properties()
utils.update_matplotlib_rcparams(props)
utils.update_matplotlib_rcparams(utils.parse_matplotlib_rcparams(props["matplotlibrc"] or ""))

# This expression selects the results (you might be able to logically simplify it)

filter_expression = props["filter"]

start_time = float(props["vector_start_time"] or -math.inf)
end_time = float(props["vector_end_time"] or math.inf)

# The data is returned as a Pandas DataFrame
df = results.get_vectors(filter_expression, include_attrs=True, include_itervars=True, start_time=start_time, end_time=end_time)

df = ops.perform_vector_ops(df, props["vector_operations"])

# You can perform any transformations on the data here

print(df)

plt.xlabel('Simulation time (s)')

title, legend = utils.extract_label_columns(df)

for t in df.itertuples(index=False):
    style = dict()
    interp = t.interpolationmode if 'interpolationmode' in df else 'sample-hold' if 'enum' in df else None
    if interp:
        if interp == "none":
            style['linestyle'] = ' '
            style['marker'] = '.'
        elif interp == "linear":
            pass  # nothing to do
        elif interp == "sample-hold":
            style['drawstyle'] = 'steps-post'
        elif interp == "backward-sample-hold":
            style['drawstyle'] = 'steps-pre'

    plt.plot(t.vectime, t.vecvalue, label=utils.make_legend_label(legend, t), **style)

if 'title' in props and props['title']:
    plt.title(props['title'])
else:
    plt.title(utils.make_chart_title(df, title, legend))

plt.legend()
plt.grid()
