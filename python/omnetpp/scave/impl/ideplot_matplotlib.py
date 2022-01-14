import matplotlib.pyplot as plt
from omnetpp.scave import utils

def set_properties(*vargs, **kwargs):
    pass


def set_property(key, value):
    pass


def get_supported_property_keys():
    return [] # TODO


def set_warning(warning):
    print("WARNING:", warning)
    # TODO this might not always work, depending on whether it's called before or after plotting?
    plt.annotate(warning, xy=(20, 40), xycoords="figure pixels", color="red")


def plot(xs, ys, key, label, drawstyle, linestyle, linewidth, color, marker, markersize):
    params = {k:v for k, v in locals().items() if k is not None and k not in ["key", "xs", "ys"]}
    return plt.plot(xs, ys, **params)


def bar(x, height, width, key, label, color, edgecolor):
    params = {k:v for k, v in locals().items() if k is not None and k not in ["key"]}
    return plt.bar(**params)


def hist(x, bins, key, density, weights, cumulative, bottom, histtype, color, label, linewidth,
         underflows, overflows, minvalue, maxvalue):
    params = {k:v for k, v in locals().items() if k is not None and k not in ["underflows", "overflows", "minvalue", "maxvalue", "key"]}
    return plt.hist(**params)


def legend(show, frameon, loc):
    if show is not None and not show:
        legend = plt.gca().get_legend()
        if legend is not None:
            legend.remove()
        return
    args = {"frameon": frameon, "loc": loc}
    if loc and loc.startswith("outside"):
        args.update(utils._legend_loc_outside_args(loc))
    plt.legend(**args)


def grid(b=True, which="major"):
    # This is consistent with `ideplot.grid`, not with `matplotlib.pyplot.grid`.
    if which in ["minor", "both"]:
        if b:
            plt.minorticks_on()
        else:
            plt.minorticks_off()
    plt.grid(b, which)


title = plt.title
xlabel = plt.xlabel
ylabel = plt.ylabel
xlim = plt.xlim
ylim = plt.ylim
xscale = plt.xscale
yscale = plt.yscale
xticks = plt.xticks
yticks = plt.yticks

