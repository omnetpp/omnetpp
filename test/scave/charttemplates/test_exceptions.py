from omnetpp.scave import plot, utils, chart

messages = []

def warn(warning):
    global messages
    messages.append(warning)

plot.set_warning = warn


def handle_exception(expected, e):
    msgs_str = str(e)
    if messages:
        msgs_str += "; Status message: " + "; ".join(messages)

    print("EXPECTED:", expected)
    print("GOT:", msgs_str)
    assert expected in msgs_str

    # make empty file so that opp_charttool doesn't complain
    fname = utils.get_image_export_filepath(chart.get_properties())
    with open(fname, 'wt') as f:
        pass
