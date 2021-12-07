from omnetpp.scave import results, chart, utils, ideplot
import matplotlib.pyplot as plt
import pandas as pd

props = chart.get_properties()
utils.preconfigure_plot(props)

try:
    stats = results.get_statistics(props["filter"], include_attrs=True, include_runattrs=True, include_itervars=True)
    hists = results.get_histograms(props["filter"], include_attrs=True, include_runattrs=True, include_itervars=True)
except ValueError as e:
    raise chart.ChartScriptError("Error while querying results: " + str(e))

df = pd.concat([stats, hists], sort=False)

if df.empty:
    raise chart.ChartScriptError("The result filter returned no data.")

title, legend = utils.extract_label_columns(df, props)

df.sort_values(by=legend, axis='index', inplace=True)

# This is how much of the standard deviation will give the 25th and 75th
# percentiles, assuming normal distribution.
# >>> math.sqrt(2) * scipy.special.erfinv(0.5)
coeff = 0.6744897501960817

boxes = [(r.min, r.mean - r.stddev * coeff, r.mean, r.mean + r.stddev * coeff, r.max)
         for r in df.itertuples(index=False) if r.count > 0]
labels = [", ".join([getattr(r, l) for l in legend])
         for r in df.itertuples(index=False) if r.count > 0]
utils.customized_box_plot(boxes, labels)

title = utils.make_chart_title(df, title)
if "title" in props and props["title"]:
    title = props["title"]
utils.set_plot_title(title)

plt.tight_layout()

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
