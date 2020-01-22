from omnetpp.scave import results, chart, utils, plot
import matplotlib.pyplot as plt
import pandas as pd

props = chart.get_properties()
utils.update_matplotlib_rcparams(props)
utils.update_matplotlib_rcparams(utils.parse_matplotlib_rcparams(props["matplotlibrc"] or ""))

stats = results.get_statistics(props["filter"], include_attrs=True, include_runattrs=True, include_itervars=True)
hists = results.get_histograms(props["filter"], include_attrs=True, include_runattrs=True, include_itervars=True)

df = pd.concat([stats, hists], sort=False)
print(df)

# TODO: move this to chart ? (or an utils module)
# source: https://stackoverflow.com/a/39789718/635587
def customized_box_plot(percentiles, axes, redraw = True, *args, **kwargs):
    """
    Generates a customized boxplot based on the given percentile values
    """
    n_box = len(percentiles)
    box_plot = axes.boxplot([[-9, -4, 2, 4, 9],]*n_box, showmeans=True, meanprops=dict(marker='+'), *args, **kwargs)
    # Creates len(percentiles) no of box plots

    min_y, max_y = float('inf'), -float('inf')

    for box_no, pdata in enumerate(percentiles):
        if len(pdata) == 6:
            (q1_start, q2_start, q3_start, q4_start, q4_end, fliers_xy) = pdata
        elif len(pdata) == 5:
            (q1_start, q2_start, q3_start, q4_start, q4_end) = pdata
            fliers_xy = None
        else:
            raise ValueError("Percentile arrays for customized_box_plot must have either 5 or 6 values")

        # Lower cap
        box_plot['caps'][2*box_no].set_ydata([q1_start, q1_start])
        # xdata is determined by the width of the box plot

        # Lower whiskers
        box_plot['whiskers'][2*box_no].set_ydata([q1_start, q2_start])

        # Higher cap
        box_plot['caps'][2*box_no + 1].set_ydata([q4_end, q4_end])

        # Higher whiskers
        box_plot['whiskers'][2*box_no + 1].set_ydata([q4_start, q4_end])

        # Box
        path = box_plot['boxes'][box_no].get_path()
        path.vertices[0][1] = q2_start
        path.vertices[1][1] = q2_start
        path.vertices[2][1] = q4_start
        path.vertices[3][1] = q4_start
        path.vertices[4][1] = q2_start

        box_plot['means'][box_no].set_ydata([q3_start, q3_start])
        box_plot['medians'][box_no].set_visible(False)

        # Outliers
        if fliers_xy is not None and len(fliers_xy[0]) != 0:
            # If outliers exist
            box_plot['fliers'][box_no].set(xdata = fliers_xy[0],
                                           ydata = fliers_xy[1])

            min_y = min(q1_start, min_y, fliers_xy[1].min())
            max_y = max(q4_end, max_y, fliers_xy[1].max())

        else:
            min_y = min(q1_start, min_y)
            max_y = max(q4_end, max_y)

        mid_y = (min_y + max_y) / 2
        # The y axis is rescaled to fit the new box plot completely with 10%
        # of the maximum value at both ends
        axes.set_ylim([mid_y - (mid_y - min_y) * 1.25, mid_y + (max_y - mid_y) * 1.25])

    # If redraw is set to true, the canvas is updated.
    if redraw:
        axes.figure.canvas.draw()

    return box_plot


title, legend = utils.extract_label_columns(df)

df.sort_values(by=[l for i, l in legend], axis='index', inplace=True)

# This is how much of the standard deviation will give the 25th and 75th
# percentiles, assuming normal distribution.
# >>> math.sqrt(2) * scipy.special.erfinv(0.5)
coeff = 0.6744897501960817

customized_box_plot([(r.min, r.mean - r.stddev * coeff, r.mean, r.mean + r.stddev * coeff, r.max)
                        for r in df.itertuples(index=False)
                            if r.count > 0
                    ], plt.gca())

plt.title(utils.make_chart_title(df, title, legend))

plt.gca().set_xticklabels([
    utils.make_legend_label(legend, r) for r in df.itertuples(index=False)
     if r.count > 0], rotation=60)

plt.tight_layout()

