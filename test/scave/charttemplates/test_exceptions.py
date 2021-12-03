import sys
from omnetpp.scave import ideplot, utils, chart

use_colors = sys.stdout.isatty()

def red(txt):
    global use_colors
    return '\033[0;31m' + txt + "\033[0;0m" if use_colors else txt

def yellow(txt):
    global use_colors
    return '\033[0;33m' + txt + "\033[0;0m" if use_colors else txt

def green(txt):
    global use_colors
    return '\033[0;32m' + txt + "\033[0;0m" if use_colors else txt

def blue(txt):
    global use_colors
    return '\033[0;34m' + txt + "\033[0;0m" if use_colors else txt

messages = []

def warn(warning):
    global messages
    messages.append(warning)

ideplot.set_warning = warn

def expect_message(expected):
    actual = "; ".join(messages)
    if expected not in actual:
        raise RuntimeError("Wrong error message received: EXPECTED '{}', GOT '{}'".format(expected,actual))

    # make empty file so that opp_charttool doesn't complain
    fname = utils.get_image_export_filepath(chart.get_properties())
    with open(fname, 'wt') as f:
        pass
