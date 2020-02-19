from omnetpp.scave import results, chart, utils

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

# collect parameters for query
filter_expression = props["filter"]

# query vector data into a data frame
df = results.get_histograms(filter_expression, include_attrs=True, include_itervars=True)

# plot
utils.plot_histograms(df, props)
utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
