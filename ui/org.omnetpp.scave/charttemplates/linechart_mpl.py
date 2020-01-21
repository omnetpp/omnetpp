from omnetpp.scave import results, chart, utils, plot, vectorops as ops
import matplotlib.pyplot as plt
import math

params = chart.get_properties()

# This expression selects the results (you might be able to logically simplify it)

filter_expression = params["filter"]

start_time = float(params["vector_start_time"] or -math.inf)
end_time = float(params["vector_end_time"] or math.inf)

# The data is returned as a Pandas DataFrame
df = results.get_vectors(filter_expression, include_attrs=True, include_itervars=True, start_time=start_time, end_time=end_time)

df = ops.perform_vector_ops(df, params["vector_operations"])

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
            pass # nothing to do
        elif interp == "sample-hold":
            style['drawstyle'] = 'steps-post'
        elif interp == "backward-sample-hold":
            style['drawstyle'] = 'steps-pre'

    plt.plot(t.vectime, t.vecvalue, label=utils.make_legend_label(legend, t), **style)

if 'title' in params and params['title']:
    plt.title(params['title'])
else:
    plt.title(utils.make_chart_title(df, title, legend))

plt.legend()
plt.grid()
