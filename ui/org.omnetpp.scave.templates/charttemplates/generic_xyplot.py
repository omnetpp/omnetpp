import math
from omnetpp.scave import results, chart, utils

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

# example plot -- replace as needed
input = props["input"]

if input:
	df = ...

# plot
utils.plot_vectors(df, props)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
#utils.export_data_if_needed(df, props)
