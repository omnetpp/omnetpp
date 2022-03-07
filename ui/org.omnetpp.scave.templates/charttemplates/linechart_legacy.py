from omnetpp.scave import results, chart, utils, ideplot

props = chart.get_properties()
utils.preconfigure_plot(props)

filter_expression = props["filter"]

try:
    df = results.get_vectors(filter_expression, include_attrs=True, include_itervars=True)
except results.ResultQueryError as e:
    raise chart.ChartScriptError("Error while querying results: " + str(e))

if df.empty:
    raise chart.ChartScriptError("The result filter returned no data.")

df = utils.perform_vector_ops(df, props["vector_operations"])

ideplot.plot_vectors(df)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
