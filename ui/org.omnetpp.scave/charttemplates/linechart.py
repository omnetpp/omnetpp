from omnetpp.scave import results, chart, vectorops as ops
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

# Finally, the results are plotted
chart.plot_vectors(df)

chart.copy_properties()
