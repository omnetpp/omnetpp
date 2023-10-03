import pandas as pd
from omnetpp.scave import results, chart, utils

props = chart.get_properties()
utils.preconfigure_plot(props)

try:
    stats = results.get_statistics(props["filter"], include_attrs=True, include_runattrs=True, include_itervars=True)
    hists = results.get_histograms(props["filter"], include_attrs=True, include_runattrs=True, include_itervars=True)
except results.ResultQueryError as e:
    raise chart.ChartScriptError("Error while querying results: " + str(e))

df = pd.concat([stats, hists], sort=False)

if df.empty:
    raise chart.ChartScriptError("The result filter returned no data.")

utils.add_legend_labels(df, props)
utils.sort_rows_by_legend(df, props)
utils.plot_boxwhiskers(df, props, sort=False)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
