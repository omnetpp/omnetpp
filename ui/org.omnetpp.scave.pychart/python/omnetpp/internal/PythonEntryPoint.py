"""
The initial Python module that sets up the Py4J connection between the
IDE (Java), and a python3 process spawned by it (Python).
"""

__copyright__ = "Copyright 2016-2020, OpenSim Ltd"
__license__ = """
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
"""

import sys
import os
import re
import pickle as pl
import traceback as tb

# the print function is replaced so it will flush after each line
import functools
print = functools.partial(print, flush=True)

# the manual conversions are only necessary because of a Py4J bug:
# https://github.com/bartdag/py4j/issues/359
from py4j.java_collections import MapConverter, ListConverter

from omnetpp.internal.TimeAndGuard import TimeAndGuard
from omnetpp.internal import Gateway

from py4j.java_gateway import DEFAULT_PORT
from py4j.clientserver import ClientServer, JavaParameters, PythonParameters

try:
    import matplotlib as mpl
    import matplotlib.pyplot as plt
    import numpy as np
    import scipy as sp
    import pandas as pd
except ImportError as e:
    print("can't import " + e.name)
    sys.exit(1)

def _extract_stacktrace(exception):
    msg = "".join(tb.format_exception(type(exception), exception, exception.__traceback__))

    # Tweak the exception message to only show the frame of the chart script, in a friendlier way.
    # Also, only tweak if the message conforms to the expected pattern, otherwise leave it alone.
    expectedFirstLine = "Traceback (most recent call last):"
    replacementFirstLine = "An error occurred. Traceback (most recent call last):"
    startOfFirstRelevantFrame = "  File \"<string>\", line "

    pattern = "(?s)" + re.escape(expectedFirstLine) + "\\n.*?\\n" + re.escape(startOfFirstRelevantFrame)
    return re.sub(pattern, replacementFirstLine + "\n" + startOfFirstRelevantFrame, msg, 1)

def _extract_message(exception):
    # extract line number (from the last 'File "<string", line' in the stack trace)
    msg = "".join(tb.format_exception(type(exception), exception, exception.__traceback__))
    m = re.search(r'(?s).*File "<string>", line (\d+)', msg)
    line = int(m.group(1)) if m else None

    # extract warning text (last line)
    msg = tb.format_exception_only(type(exception), exception)[-1].strip()

    if msg.startswith("py4j.protocol.Py4JJavaError:") and "\n" in msg:
        # discard Py4JJavaError line (1st line) and Java stack trace (3rd and further lines);
        # keep 2nd line only (type and msg of the Java exception)
        msg = msg.split("\n")[1].strip(" :")
    elif "\n" in msg:
        msg = msg.split("\n")[0].strip()  # only display first line

    return line, msg


class PythonEntryPoint(object):
    """
    An implementation of the Java interface org.omnetpp.scave.pychart.IPythonEntryPoint
    through Py4J. Provides an initial point to set up additional object references between
    Java and Python, and to execute arbitrary Python code from the IDE.
    """

    class Java:
        implements = ["org.omnetpp.scave.pychart.IPythonEntryPoint"]

    def __init__(self):
        """ The execution context of the scripts submitted to execute() """
        self.execContext = {
            "print": print,
            "exit": sys.exit
        }

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

    def setWarningAnnotator(self, warning_annotator):
        Gateway.warning_annotator = warning_annotator

    # @TimeAndGuard(measureTime=False)
    def execute(self, chartInput):
        # these imports must be lazy, to allow correct module initialization
        from omnetpp.scave import chart, ideplot

        try:
            exec(chartInput, self.execContext)
        except chart.ChartScriptError as e:
            # ChartScriptErrors are by convention constructed with a message that
            # is helpful and complete on its own, so just printing that is OK.
            Gateway.warning_annotator.setWarning(str(e))
            sys.exit(1)
        except Exception as e:
            # These errors are also printed to stderr (the Console) in more detail
            print(_extract_stacktrace(e), file=sys.stderr)

            # Add problem marker in editor
            line, message = _extract_message(e)
            Gateway.warning_annotator.setWarning(message + "\n(See Console for details)")
            if line:
                Gateway.warning_annotator.setErrorMarkerAnnotation(line, message)
            sys.exit(1)

    # @TimeAndGuard(measureTime=False)
    def evaluate(self, expression):
        return eval(expression, self.execContext)

    def getRcParams(self):
        return MapConverter().convert({str(k) : str(v) for k,v in mpl.rcParams.items()}, Gateway.gateway._gateway_client)

    def getVectorOps(self):
        from omnetpp.scave import vectorops

        ops = []
        # ctor is: VectorOp(String module, String name, String signature, String docstring, String label, String example)
        for o in vectorops._report_ops():
            try:
                ops.append(Gateway.gateway.jvm.org.omnetpp.scave.editors.VectorOperations.VectorOp(*o))
            except Exception as E:
                print("Exception while processing vector operation:", o[4])

        return ListConverter().convert(ops, Gateway.gateway._gateway_client)

    def setGlobalObjectPickle(self, name, pickle):
        self.execContext[name] = pl.loads(pickle)


def connect_to_IDE():
    java_port = int(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_PORT
    # print("initiating Python ClientServer, connecting to port " + str(java_port))

    entry_point = PythonEntryPoint()

    gateway = ClientServer(
        java_parameters=JavaParameters(
            address="127.0.0.1", port=java_port,
            auto_field=True, auto_convert=True, auto_close=True,
            auth_token=os.environ["PY4J_AUTH_TOKEN"]),
        python_parameters=PythonParameters(
             address="127.0.0.1", port=0,
             daemonize=True, daemonize_connections=True),
        python_server_entry_point=entry_point)
    # Even though this won't erase it from the environment of the process itself at
    # the system level, at least it will be more difficult to access from Python.
    del os.environ["PY4J_AUTH_TOKEN"]

    Gateway.gateway = gateway

    python_port = gateway.get_callback_server().get_listening_port()

    # telling Java which port we listen on
    address = gateway.java_gateway_server.getCallbackClient().getAddress()
    gateway.java_gateway_server.resetCallbackClient(address, python_port)

    # print("Python ClientServer done, listening on port " + str(python_port))


def setup_unbuffered_output():
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


def setup_pandas_display_parameters():
    pd.set_option("display.width", 500)
    pd.set_option("display.max_columns", 50)
    pd.set_option("display.max_colwidth", 50)
    pd.set_option("display.max_rows", 500)


if __name__ == "__main__":

    setup_unbuffered_output()
    setup_pandas_display_parameters()
    connect_to_IDE()

    # block the main Python thread while the host process is running
    # (our stdin is connected to it via a pipe, so stdin will be closed when it exists)
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
