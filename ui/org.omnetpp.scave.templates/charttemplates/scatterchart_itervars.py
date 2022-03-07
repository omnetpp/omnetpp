from omnetpp.scave import results, chart, utils, ideplot

# get chart properties
props = chart.get_properties()
utils.preconfigure_plot(props)

# collect parameters for query
filter_expression = props["filter"]
include_fields = props["include_fields"] == "true"

# query data into a data frame
try:
    df = results.get_scalars(filter_expression, include_fields=include_fields, include_attrs=True, include_runattrs=True, include_itervars=True)
except results.ResultQueryError as e:
    raise chart.ChartScriptError("Error while querying results: " + str(e))

if df.empty:
    raise chart.ChartScriptError("The result filter returned no data.")

xaxis_itervar, group_by = utils.select_xaxis_and_groupby(df, props)
confidence_level = utils.get_confidence_level(props)
newdf = utils.pivot_for_scatterchart(df, xaxis_itervar, group_by, confidence_level)
utils.plot_lines(newdf, props)

names = df["name"].unique()
scalar_names = names[0] + ", etc." if len(names) > 1 else names[0]
ideplot.ylabel(scalar_names) # TODO: shouldn't call into ideplot directly

title = (scalar_names + " vs. " + xaxis_itervar) if xaxis_itervar else scalar_names
if "title" in props and props["title"]:
    title = props["title"]
utils.set_plot_title(title)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
