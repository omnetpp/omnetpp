import sys
import matplotlib.pyplot as plt
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

def set_warning(warning):
    global messages
    messages.append(warning)

ideplot.set_warning = set_warning

def expect_message(expected):
    actual = "; ".join(messages)
    if expected not in actual:
        raise RuntimeError("Wrong error message received: EXPECTED '{}', GOT '{}'".format(expected,actual))

def create_blank_image_file():
    # make empty file so that opp_charttool doesn't complain
    fname = utils.get_image_export_filepath(chart.get_properties())
    with open(fname, 'wt') as f:
        pass

def run_test(chartscript_function, name, description, expected_error):
    print(yellow("Running test: " + name + ":"), description, ("->" + repr(expected_error) if expected_error else ""))
    messages.clear()

    try:
        plt.figtext(0, 1, name + ": " + description + "\n" + (expected_error or ""), color='m', horizontalalignment='left', verticalalignment='top')
        chartscript_function()
        if expected_error:
            raise RuntimeError("Expected error did not occur")
    except chart.ChartScriptError as e:
        set_warning(str(e))
        expect_message(expected_error)
        create_blank_image_file()

    print(green("PASS"))
