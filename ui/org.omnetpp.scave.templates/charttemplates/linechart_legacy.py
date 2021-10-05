from omnetpp.scave import results, chart, utils, ideplot

props = chart.get_properties()
utils.preconfigure_plot(props)

filter_expression = props["filter"]

try:
    df = results.get_vectors(filter_expression, include_attrs=True, include_itervars=True)
except ValueError as e:
    ideplot.set_warning("Error while querying results: " + str(e))
    exit(1)

if df.empty:
    ideplot.set_warning("The result filter returned no data.")
    exit(1)

df = utils.perform_vector_ops(df, props["vector_operations"])

ideplot.plot_vectors(df)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
