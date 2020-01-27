import math
from omnetpp.scave import results, chart, utils, plot, vectorops as ops

# get chart properties
props = chart.get_properties()

# collect parameters for query 
filter_expression = props["filter"]
start_time = float(props["vector_start_time"] or -math.inf)
end_time = float(props["vector_end_time"] or math.inf)

# query vector data into a data frame
df = results.get_vectors(filter_expression, include_attrs=True, include_itervars=True, start_time=start_time, end_time=end_time)

# apply vector operations
df = ops.perform_vector_ops(df, props["vector_operations"])

# plot
utils.plot_vectors(df, props)
