from omnetpp.scave import results, chart, utils, plot, vectorops as ops
import math

props = chart.get_properties()

# This expression selects the results (you might be able to logically simplify it)

filter_expression = props["filter"]

start_time = float(props["vector_start_time"] or -math.inf)
end_time = float(props["vector_end_time"] or math.inf)

# The data is returned as a Pandas DataFrame
df = results.get_vectors(filter_expression, include_attrs=True, include_itervars=True, start_time=start_time, end_time=end_time)

df = ops.perform_vector_ops(df, props["vector_operations"])

# You can perform any transformations on the data here

print(df)

title_col, legend_cols = utils.extract_label_columns(df)

drawstyle = props["drawstyle"] or "auto"

for i, t in enumerate(df.itertuples(index=False)):
    if drawstyle == "auto":
        linetype = utils.interpolationmode_to_drawstyle(t.interpolationmode if "interpolationmode" in df else None, "enum" in df)
    else:
        linetype = props["drawstyle"] or "linear"
    props["Line.Type/" + str(i)] = linetype
    plot.plot_vector(utils.make_legend_label(legend_cols, t), t.vectime, t.vecvalue, i)
print(props)

plot.set_properties(props)
