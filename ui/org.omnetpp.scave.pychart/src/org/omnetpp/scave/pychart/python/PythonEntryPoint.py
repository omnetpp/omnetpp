import time
import sys
import os

import pickle as pl
import math

import functools
print = functools.partial(print, flush=True)

from TimeAndGuard import TimeAndGuard
import traceback

import results

import Gateway

try:
    import numpy as np
    import matplotlib as mpl

    mpl.use('module://backend_SWTAgg')

    import pandas as pd
    import matplotlib.pyplot as plt

    pd.set_option("display.width", 500)
    pd.set_option("display.max_columns", 20)
    pd.set_option("display.max_colwidth", 50)
    pd.set_option("display.max_rows", 100)

except ImportError as e:
    print("can't import " + e.name)
    sys.exit(1)

try:
    import prettyplotlib as ppl
except ImportError:
    ppl = None

try:
    import seaborn as sbs
except ImportError:
    sbs = None

# optionally import scipy as well?

# adding some modules as implicit imports for chart scripts
# the print function is replaced so it will flush after each line
execContext = {
    "np": np,
    "pd": pd,
    "mpl": mpl,
    "plt" : plt,
    "ppl": ppl,
    "sbs": sbs,
    "print": print
}


def listToBytes(l):
    return np.array(np.array(l), dtype=np.dtype('>f8')).tobytes()


def to_label(x):
    if isinstance(x, str):
        return x
    elif isinstance(x, tuple):
        return ", ".join(list(x))
    elif isinstance(x, list):
        return ", ".join(x)
    else:
        return str(x)


class OppAccessor(object):

    def __init__(self, entry_point):
        self.entry_point = entry_point

    def _plotScalars_lists(self, row_label, labels, values):
        Gateway.chart_plotter.plotScalars(pl.dumps(
            {
                "columnKeys": labels if labels else [""] * len(values),
                "rowKeys": [row_label if row_label else ""],
                "values": listToBytes(values)
            }
        ))

    def _plotScalars_DF_simple(self, df):
        Gateway.chart_plotter.plotScalars(pl.dumps(
            {
                "columnKeys": [to_label(c) for c in list(df.columns)],
                "rowKeys": [to_label(i) for i in list(df.index)],
                "values": listToBytes(df.values.flatten('F'))
            }
        ))

    def _plotScalars_DF_scave(self, df):
        self._plotScalars_DF_simple(results.pivotScalars(df))

    def _plotScalars_DF_2(self, df):
        names = df.index.get_level_values('name').tolist()
        modules = df.index.get_level_values('module').tolist()

        paths = list(map(lambda t: '.'.join(t), zip(names, modules)))

        values = df[('result', 'value')]

        self._plotScalars_lists(None, paths, values)

    def plotScalars(self, df_or_values, labels=None, row_label=None):
        """
        This method only does dynamic dispatching based on its first argument.
        TODO: styling in-place?
        """
        if isinstance(df_or_values, pd.DataFrame):
            df = df_or_values
            if "value" in df.columns and "type" in df.columns and "module" in df.columns and "name" in df.columns:
                self._plotScalars_DF_scave(df)
            elif "experiment" in df.index.names and "measurement" in df.index.names and "replication" in df.index.names and "module" in df.index.names and "name" in df.index.names:
                self._plotScalars_DF_2(df)
            else:
                self._plotScalars_DF_simple(df)
        else:
            self._plotScalars_lists(row_label, labels, df_or_values)

    def plotVector(self, label, xs, ys):
        Gateway.chart_plotter.plotVectors(pl.dumps([
            {
                "title": str(label),
                "key": str(label),
                "xs": listToBytes(xs),
                "ys": listToBytes(ys)
            }
        ]))

    def _plotVectors_tuplelist(self, vectors):
        """ vectors is a list of (label, xs, ys) tuples """
        Gateway.chart_plotter.plotVectors(pl.dumps([
            {
                "title": str(v[0]),
                "key": str(v[0]),
                "xs": listToBytes(v[1]),
                "ys": listToBytes(v[2])
            }
            for v in vectors
        ]))

    def _plotVectors_DF_simple(self, df):
        xs = None
        if "time" in df:
            xs = listToBytes(df["time"])
        else:
            xs = listToBytes(range(len(df[df.columns[0]])))

        Gateway.chart_plotter.plotVectors(pl.dumps([
            {
                "title": column,
                "key": column,
                "xs": xs,
                "ys": listToBytes(df[column])
            }
            for column in df if column != "time"
        ]))

    def _plotVectors_DF_scave(self, df):
        Gateway.chart_plotter.plotVectors(pl.dumps([
            {
                "title": row.module + ":" + row.name,
                "key": row.module + ":" + row.name,
                "xs": listToBytes(row.vectime),
                "ys": listToBytes(row.vecvalue)
            }
            for row in df.itertuples(index=False) if row.type == "vector"
        ]))

    def _plotVectors_DF_2(self, df):
        Gateway.chart_plotter.plotVectors(pl.dumps([
            {
                "title": row[('attr', 'title')],
                "key": "-".join(row.name) + row[('attr', 'title')],
                "xs": listToBytes(row[('result', 'vectime')]),
                "ys": listToBytes(row[('result', 'vecvalue')])
            }
            for index, row in df.iterrows()
        ]))

        for index, row in df.iterrows():
            if ('attr', 'interpolationmode') in row:
                interp = row[('attr', 'interpolationmode')]
                key = "-".join(row.name) + row[('attr', 'title')]

                if interp == "none":
                    self.setProperty("Line.Type/" + key, "Dots")
                elif interp == "linear":
                    self.setProperty("Line.Type/" + key, "Linear")
                elif interp == "sample-hold":
                    self.setProperty("Line.Type/" + key, "SampleHold")
                elif interp == "backward-sample-hold":
                    self.setProperty("Line.Type/" + key, "BackwardSampleHold")

    def plotVectors(self, df_or_list):
        """
        TODO: styling in-place?
        """
        if isinstance(df_or_list, pd.DataFrame):
            df = df_or_list
            if "vectime" in df.columns and "vecvalue" in df.columns and "type" in df.columns and "module" in df.columns and "name" in df.columns:
                self._plotVectors_DF_scave(df)
            elif "experiment" in df.index.names and "measurement" in df.index.names and "replication" in df.index.names and "module" in df.index.names and "name" in df.index.names:
                self._plotVectors_DF_2(df)
            else:
                self._plotVectors_DF_simple(df)
        else:
            self._plotVectors_tuplelist(df_or_list)

    def _plotPoints_DF(self, df):
        Gateway.chart_plotter.plotVectors(pl.dumps([
            {
                "title": str(row[0]),
                "key": str(row[0]),
                "xs": listToBytes([row.x]),
                "ys": listToBytes([row.y])
            }
            for row in df.itertuples()
        ]))

    def _plotPoints_tuplelist(self, points):
        """ points is a list of (label, x, y) tuples """
        Gateway.chart_plotter.plotVectors(pl.dumps([
            {
                "title": str(point[0]),
                "key": str(point[0]),
                "xs": listToBytes([point[1]]),
                "ys": listToBytes([point[2]])
            }
            for point in points
        ]))

    """
    # TODO: scatter charts: same as vector, just with implicit "Dots" Line Type?
    def plotPoints(self, df_or_points):
        if isinstance(df_or_points, pd.DataFrame):
            self._plotPoints_DF(df_or_points)
        else:
            self._plotPoints_tuplelist(df_or_points)
    """

    def plotScatter(self, df, xdata, iso_column):

        names = set(df["name"].values)
        names.discard(None)
        names.discard(xdata)
        names.discard(iso_column)

        df = results.pivotScalars(df, index=xdata, columns=iso_column, values="value")

        renaming = dict()

        if iso_column:
            for c in list(df.columns):
                renaming[c] = iso_column + "=" + str(c)

        df = df.reset_index()

        renaming[xdata] = "time"

        df = df.rename(renaming, axis="columns")

        df.dropna(inplace=True)

        df["time"] = pd.to_numeric(df["time"])

        df.sort_values(by="time", inplace=True)

        print(df)

        xs = listToBytes(df["time"])

        Gateway.chart_plotter.plotVectors(pl.dumps([
            {
                "title": column,
                "key": column,
                "xs": xs,
                "ys": listToBytes(df[column])
            }
            for column in df if column != "time"
        ]))

        return list(names)

    def plotHistogram(self, label, edges, values, count=-1, lowest=math.nan, highest=math.nan):
        Gateway.chart_plotter.plotHistograms(pl.dumps([
            {
                "title": label,
                "key": label,
                "count": len(values) if count == -1 else count,
                "min": float(min(edges)) if lowest == math.nan else lowest,
                "max": float(max(edges)) if highest == math.nan else highest,
                "edges": listToBytes(edges),
                "values": listToBytes(values)
            }
        ]))

    def _plotHistograms_DF(self, df):
        for row in df.itertuples(index=False):
            if row[1] == "histogram":

                min = float(row[10])
                max = float(row[11])
                count = int(row[6])

                edges = listToBytes(list(row[12]) + [float('inf')])
                values = listToBytes(row[13])

                Gateway.chart_plotter.plotHistograms(pl.dumps([
                    {
                        "title": row[2] + ":" + row[3],
                        "key": row[2] + ":" + row[3],
                        "count": count,
                        "min": min,
                        "max": max,
                        "edges": edges,
                        "values": values
                    }
                ]))

    def _plotHistograms_DF_2(self, df):
        Gateway.chart_plotter.plotHistograms(pl.dumps([
            {
                "title": row[('attr', 'title')],  # row[2] + ":" + row[3],
                "key": "-".join(row.name) + row[('attr', 'title')],  # row[2] + ":" + row[3],
                "count": int(row[('result', 'count')]),
                "min": float(row[('result', 'min')]),
                "max": float(row[('result', 'max')]),
                "edges": listToBytes(list(row[('result', 'binedges')]) + [float('inf')]),
                "values": listToBytes(row[('result', 'binvalues')])
            }
            for index, row in df.iterrows()
        ]))

    def plotHistograms(self, df):
        if "experiment" in df.index.names and "measurement" in df.index.names and "replication" in df.index.names and "module" in df.index.names and "name" in df.index.names:
            self._plotHistograms_DF_2(df)
        else:
            self._plotHistograms_DF(df)

    def setProperty(self, key, value):
        Gateway.chart_plotter.setChartProperty(key, value)

    def setProperties(self, *vargs, **kwargs):
        for a in vargs:
            for k, v in a.items():
                Gateway.chart_plotter.setChartProperty(k, v)  # TODO: could be optimized
        for k, v in kwargs.items():
            Gateway.chart_plotter.setChartProperty(k, v)

    def getProperties(self):
        return Gateway.properties_provider.getChartProperties()

    def getProperty(self, key):
        return Gateway.properties_provider.getChartProperties()["key"]  # TODO: could be optimized

    def getDefaultProperties(self):
        return Gateway.properties_provider.getDefaultChartProperties()

    def getName(self):
        return Gateway.properties_provider.getChartName()

    """
    def getChartContents(self):
        return self.entry_point.getChartContents()
    """


class PythonEntryPoint(object):

    class Java:
        implements = ["org.omnetpp.scave.pychart.IPythonEntryPoint"]

    def check(self):
        return True

    def setResultsProvider(self, results_provider):
        global execContext
        Gateway.results_provider = results_provider

        execContext["chart"] = OppAccessor(Gateway.results_provider)

    def setChartPropertiesProvider(self, properties_provider):
        Gateway.properties_provider = properties_provider

    def setPlotWidgetProvider(self, widget_provider):
        Gateway.widget_provider = widget_provider

    def setNativeChartPlotter(self, chart_plotter):
        Gateway.chart_plotter = chart_plotter

    # @TimeAndGuard(measureTime=False)
    def execute(self, chartInput):
        global execContext
        exec(chartInput, execContext)

    def setGlobalObjectPickle(self, name, pickle):
        execContext[name] = pl.loads(pickle)


if __name__ == "__main__":
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
