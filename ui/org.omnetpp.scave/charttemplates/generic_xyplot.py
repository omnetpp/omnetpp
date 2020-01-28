import math
from omnetpp.scave import results, chart, utils

# get chart properties
props = chart.get_properties()

# example plot -- replace as needed
input = props["input"]

if input:
	df = ...
	
# plot
utils.plot_vectors(df, props)
