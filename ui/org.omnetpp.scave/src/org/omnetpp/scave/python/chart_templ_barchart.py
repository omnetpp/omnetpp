from omnetpp.scave import results, chart

params = chart.get_properties()

# This expression selects the results (you might be able to logically simplify it)

filter_expression = params["filter"]

# The data is returned as a Pandas DataFrame
df = results.get_scalars(filter_expression)

# You can perform any transformations on the data here

# Finally, the results are plotted
chart.plot_scalars(df)