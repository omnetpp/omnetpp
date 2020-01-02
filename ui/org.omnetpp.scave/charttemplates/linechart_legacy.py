from omnetpp.scave import results, chart, vectorops as ops

params = chart.get_configured_properties()

# This expression selects the results (you might be able to logically simplify it)

filter_expression = params["filter"]

# The data is returned as a Pandas DataFrame
df = results.get_vectors(filter_expression, include_attrs=True, include_itervars=True)

df = ops.perform_vector_ops(df, params["vector_operations"])

# You can perform any transformations on the data here

print(df)

# Finally, the results are plotted
chart.plot_vectors(df)

chart.copy_properties()
