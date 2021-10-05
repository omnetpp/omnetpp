from omnetpp.scave import ideplot, utils, chart

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
