from omnetpp.scave import results, chart, utils

# get chart properties
props = chart.get_properties()

# collect parameters for query
filter_expression = props["filter"]

# query vector data into a data frame
df = results.get_histograms(filter_expression, include_attrs=True, include_itervars=True)

# plot
utils.plot_histograms(df, props)
