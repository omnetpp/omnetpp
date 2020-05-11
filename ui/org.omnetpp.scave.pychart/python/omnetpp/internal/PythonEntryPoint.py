import sys
import os
import pickle as pl

import functools
print = functools.partial(print, flush=True)

from omnetpp.internal.TimeAndGuard import TimeAndGuard
from omnetpp.internal import Gateway

from py4j.java_gateway import DEFAULT_PORT
from py4j.clientserver import ClientServer, JavaParameters, PythonParameters

try:
    import matplotlib as mpl
    import matplotlib.pyplot as plt
    import numpy as np
    import pandas as pd
except ImportError as e:
    print("can't import " + e.name)
    sys.exit(1)

# the print function is replaced so it will flush after each line
execContext = {
    "print": print
}


class PythonEntryPoint(object):

    class Java:
        implements = ["org.omnetpp.scave.pychart.IPythonEntryPoint"]

    def check(self):
        return True

    def setResultsProvider(self, results_provider):
        Gateway.results_provider = results_provider

    def setChartProvider(self, chart_provider):
        Gateway.chart_provider = chart_provider

    def setPlotWidgetProvider(self, widget_provider):
        Gateway.widget_provider = widget_provider

    def setNativeChartPlotter(self, chart_plotter):
        Gateway.chart_plotter = chart_plotter

    # @TimeAndGuard(measureTime=False)
    def execute(self, chartInput):
        global execContext
        exec(chartInput, execContext)

    def setGlobalObjectPickle(self, name, pickle):
        global execContext
        execContext[name] = pl.loads(pickle)


def connect_to_IDE():
    java_port = int(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_PORT
    # print("initiating Python ClientServer, connecting to port " + str(java_port))

    entry_point = PythonEntryPoint()

    gateway = ClientServer(
        java_parameters=JavaParameters(port=java_port, auto_field=True, auto_convert=True, auto_close=True),
        python_parameters=PythonParameters(port=0, daemonize=True, daemonize_connections=True),
        python_server_entry_point=entry_point)

    Gateway.gateway = gateway

    python_port = gateway.get_callback_server().get_listening_port()

    # telling Java which port we listen on
    address = gateway.java_gateway_server.getCallbackClient().getAddress()
    gateway.java_gateway_server.resetCallbackClient(address, python_port)

    # print("Python ClientServer done, listening on port " + str(python_port))

if __name__ == "__main__":
    # I believe the purpose of the following piece of code is entirely achieved by the "-u" command line argument.
    # But just to be sure, let's leave this in here, I don't think it will cause harm.
    # We're just trying to make the stdout and stderr streams unbuffered, so all output is sure(r) to reach the Console.
    if os.name == "posix":
        import fcntl

        fl = fcntl.fcntl(sys.stdout.fileno(), fcntl.F_GETFL)
        fl |= os.O_SYNC
        fcntl.fcntl(sys.stdout.fileno(), fcntl.F_SETFL, fl)

        fl = fcntl.fcntl(sys.stderr.fileno(), fcntl.F_GETFL)
        fl |= os.O_SYNC
        fcntl.fcntl(sys.stderr.fileno(), fcntl.F_SETFL, fl)
    else:
        import msvcrt
        msvcrt.setmode(sys.stdout.fileno(), os.O_BINARY)
        msvcrt.setmode(sys.stderr.fileno(), os.O_BINARY)

    pd.set_option("display.width", 500)
    pd.set_option("display.max_columns", 50)
    pd.set_option("display.max_colwidth", 50)
    pd.set_option("display.max_rows", 500)

    connect_to_IDE()

    for line in sys.stdin:
        # We don't actually expect any input, this is just a simple way to wait
        # for the parent process (Java) to die.
        pass

    # print("Python process exiting...")

    Gateway.gateway.close(False, True)
    Gateway.gateway.shutdown_callback_server()
    Gateway.gateway.shutdown()

    sys.exit()
    # it should never come to this, but just to make sure:
    os._exit(1)
