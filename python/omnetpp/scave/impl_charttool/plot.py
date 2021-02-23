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
        args.update(_legend_loc_outside_args(loc))
    plt.legend(**args)

def _legend_loc_outside_args(loc):
    mapping = {
        "outside top left": ("lower left", (0,1.05)),
        "outside top center": ("lower center", (0.5,1.05)),
        "outside top right": ("lower right", (1,1.05)),
        "outside bottom left": ("upper left", (0,-0.05)),
        "outside bottom center": ("upper center", (0.5,-0.05)),
        "outside bottom right": ("upper right", (1,-0.05)),
        "outside left top": ("upper right", (-0.03, 1)),
        "outside left center": ("center right", (-0.03,0.5)),
        "outside left bottom": ("lower right", (-0.03,0)),
        "outside right top": ("upper left", (1.03,1)),
        "outside right center": ("center left", (1.03,0.5)),
        "outside right bottom": ("lower left", (1.03,0)),
    }
    if loc not in mapping:
        raise ValueError("loc='{}' is not recognized/supported".format(loc))
    (anchorloc, relpos) = mapping[loc]
    return {"loc" : anchorloc, "bbox_to_anchor" : relpos}

title = plt.title
xlabel = plt.xlabel
ylabel = plt.ylabel
xlim = plt.xlim
ylim = plt.ylim
xscale = plt.xscale
yscale = plt.yscale
xticks = plt.xticks
yticks = plt.yticks
grid = plt.grid
