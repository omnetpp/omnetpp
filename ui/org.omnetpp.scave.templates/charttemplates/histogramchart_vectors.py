import math
import numpy as np
from omnetpp.scave import results, chart, utils

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

# collect parameters for query
filter_expression = props["filter"]
start_time = float(props["vector_start_time"] or -math.inf)
end_time = float(props["vector_end_time"] or math.inf)

# query vector data into a data frame
try:
    df = results.get_vectors(filter_expression, include_attrs=True, include_runattrs=True, include_itervars=True, start_time=start_time, end_time=end_time)
except results.ResultQueryError as e:
    raise chart.ChartScriptError("Error while querying results: " + str(e))

if df.empty:
    raise chart.ChartScriptError("The result filter returned no data.")

def compute_histogram(row):
    values = row["vecvalue"]
    edges = utils.histogram_bin_edges(values)
    row["binvalues"], row["binedges"] = np.histogram(values, bins=edges)
    return row

df = df.apply(compute_histogram, axis=1)

# plot
utils.plot_histograms(df, props)
utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
