from omnetpp.scave import results, chart, utils, plot, vectorops as ops

params = chart.get_properties()
utils.preconfigure_plot(props)

# This expression selects the results (you might be able to logically simplify it)

filter_expression = params["filter"]

# The data is returned as a Pandas DataFrame
df = results.get_vectors(filter_expression, include_attrs=True, include_itervars=True)

df = ops.perform_vector_ops(df, params["vector_operations"])

# You can perform any transformations on the data here

print(df)

# Finally, the results are plotted
plot.plot_vectors(df)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
