from omnetpp.scave import results, chart, utils

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

# collect parameters for query
filter_expression = props["filter"]
include_fields = props["include_fields"] == "true"

# query scalar data into dataframe
try:
    df = results.get_scalars(filter_expression, include_fields=include_fields, include_attrs=True, include_runattrs=True, include_itervars=True)
except results.ResultQueryError as e:
    raise chart.ChartScriptError("Error while querying results: " + str(e))

if df.empty:
    raise chart.ChartScriptError("The result filter returned no data.")

groups, series = utils.select_groups_series(df, props)
confidence_level = utils.get_confidence_level(props)
valuedf, errorsdf, metadf = utils.pivot_for_barchart(df, groups, series, confidence_level)

utils.plot_bars(valuedf, errorsdf, metadf, props)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
