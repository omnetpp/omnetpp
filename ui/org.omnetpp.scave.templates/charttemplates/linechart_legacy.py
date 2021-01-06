from omnetpp.scave import results, chart, utils, plot, vectorops as ops

params = chart.get_properties()
utils.preconfigure_plot(props)

filter_expression = params["filter"]

df = results.get_vectors(filter_expression, include_attrs=True, include_itervars=True)

if df.empty:
    plot.set_warning("The result filter returned no data.")
    exit(1)

df = ops.perform_vector_ops(df, params["vector_operations"])

plot.plot_vectors(df)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
