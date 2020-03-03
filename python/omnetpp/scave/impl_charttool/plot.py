import matplotlib.pyplot as plt
import math
from omnetpp.scave import utils # for small misc. methods


def set_properties(*vargs, **kwargs):
    pass

def set_property(key, value):
    pass


def get_supported_property_keys():
    return [] # TODO


def set_warning(warning):
    # TODO this might not always work, depending on whether it's called before or after plotting?
    plt.annotate(warning, xy=(20, 40), xycoords="figure pixels", color="red")


def plot(xs, ys, key=None, label=None, drawstyle=None, linestyle=None, linewidth=None, color=None, marker=None, markersize=None):
    params = {k:v for k, v in locals().items() if k is not None and k not in ["key", "xs", "ys"]}
    return plt.plot(xs, ys, **params)


def hist(x, bins, density=False, weights=None, cumulative=False, bottom=None, histtype='stepfilled', color=None, label=None, linewidth=None,
         underflows=0.0, overflows=0.0, minvalue=math.nan, maxvalue=math.nan):

    params = locals()
    params = {k:v for k, v in locals().items() if k is not None and k not in ["underflows", "overflows", "minvalue", "maxvalue", "params"]}
    return plt.hist(**params)

bar = plt.bar
title = plt.title
xlabel = plt.xlabel
ylabel = plt.ylabel
xlim = plt.xlim
ylim = plt.ylim
xscale = plt.xscale
yscale = plt.yscale
xticks = plt.xticks
legend = plt.xticks
grid = plt.grid

def legend(*args, **kwargs):
    if "show" in kwargs:
        if kwargs["show"] is not None and not kwargs["show"]:
            if plt.gca().get_legend() is not None:
                plt.gca().get_legend().remove()
            return
        del kwargs["show"]
    if "loc" in kwargs and kwargs["loc"] and kwargs["loc"].startswith("outside"):
        kwargs2 = utils._legend_mpl_loc_outside_args(kwargs["loc"])
        del kwargs["loc"]
        kwargs.update(kwargs2)
    plt.legend(*args, **kwargs)
