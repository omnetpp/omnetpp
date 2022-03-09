import os
import platform
import numpy as np
import pandas as pd
import pickle as pl

# posix_ipc is required for POSIX shm on Linux and Mac
if platform.system() in ['Linux', 'Darwin']:
    import posix_ipc
import mmap

import functools
print = functools.partial(print, flush=True)

from omnetpp.internal import Gateway

import py4j
from py4j.java_collections import MapConverter

_vector_data_counter = 0

def is_native_chart():
    return bool(Gateway.chart_plotter)

def assert_is_native_chart():
    if not Gateway.chart_plotter:
        raise RuntimeError("This method can only be used on native charts")

def set_property(key, value):
    """
    Sets one property of the native plot widget to the given value. When invoked
    outside the contex of a native plot widget, the function does nothing.

    # Parameters

    - **key** *(string)*: Name of the property.
    - **value** *(string)*: The value to set. If any other type than string is passed in, it will be converted to string.
    """
    assert_is_native_chart()
    msg = None
    try:
        Gateway.chart_plotter.setProperty(key, value)
    except py4j.protocol.Py4JJavaError as e:
        msg = e.java_exception.getMessage()

    if msg is not None:
        raise RuntimeError(msg)


def set_properties(props):
    """
    Sets several properties of the native plot widget. It is functionally equivalent to
    repeatedly calling `set_property` with the entries of the `props` dictionary.

    # Parameters

    - **props** *(dict)*: The properties to set.
    """
    assert_is_native_chart()
    msg = None
    try:
        Gateway.chart_plotter.setProperties(dict(props))
    except py4j.protocol.Py4JJavaError as e:
        msg = e.java_exception.getMessage()


    if msg is not None:
        raise RuntimeError(msg)


def get_supported_property_keys():
    """
    Returns the list of property names that the native plot widget supports, such as
    'Plot.Title', 'X.Axis.Max' and 'Legend.Display', among many others.
    """
    assert_is_native_chart()
    return set(Gateway.chart_plotter.getSupportedPropertyKeys())


def set_warning(warning):
    """
    Displays the given warning text in the plot.

    # Parameters

    - **warning** *(string)*: The warning string.
    """

    Gateway.warning_annotator.setWarning(warning)

def plot_lines(df, props):
    """
    Plots lines given in the DataFrame `df` (one line from each row), and sets
    the visual properties from `props` onto the plot.

    The DataFrame must have these columns (in any order):
    - `key` (string or `None`): An internal (unique) identifier for each line.
    - `label` (string): The name of the line, this will appear on the legend, string.
    - `xs` (array): Stores the X coordinates of the points used to draw the line.
    - `ys` (array): Stores the Y coordinates of the points used to draw the line.
        Must have the same number of elements as `xs`

    The properties in `props` will be made specific to the items added by this call,
    by appending `/<key>` row-by-row to each property key.

    This can only be called if the chart is of type `LINE`.
    """
    assert_is_native_chart()
    if sorted(list(df.columns)) != sorted(["key", "label", "xs", "ys"]):
        raise RuntimeError("Invalid DataFrame format in plot_lines")

    # only used on posix, to unlink them later
    shm_objs = list()
    # only used on windows, to prevent gc
    mmap_objs = list()

    # the key of each row is appended to the property names in Java
    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "key": row.key,
            "title": str(row.label),
            "xs": _put_array_in_shm(row.xs, shm_objs, mmap_objs),
            "ys": _put_array_in_shm(row.ys, shm_objs, mmap_objs)
        }
        for row in df.itertuples(index=False)
    ]), MapConverter().convert(props, Gateway.gateway._gateway_client))

    # this is a no-op on Windows
    for o in shm_objs:
        o.unlink()


def plot_bars(df, props):
    """
    Plots bars given in the DataFrame `df` (one series from each row), and sets
    the visual properties from `props` onto the plot.

    The DataFrame must have these columns (in any order):
    - `key` (string or `None`): An internal (unique) identifier for each series, any type.
    - `label`: The name of the series, this will appear on the legend, any type.
    - `values`: A list storing the heights of the bars.

    The properties in `props` will be made specific to the items added by this call,
    by appending `/<key>` row-by-row to each property key.

    This can only be called if the chart is of type "BAR".
    """
    # TODO: add check for one-layer indices? numbers-only data?
    assert_is_native_chart()
    if sorted(list(df.columns)) != sorted(["key", "label", "values"]):
        raise RuntimeError("Invalid DataFrame format in plot_bars")

    # the key of each row is appended to the property names in Java
    Gateway.chart_plotter.plotScalars(pl.dumps([
        {
            "key": row.key,
            "title": str(row.label),
            "values": _list_to_bytes(row.values),
        }
        for row in df.itertuples(index=False)
    ]), MapConverter().convert(props, Gateway.gateway._gateway_client))


def plot_histograms(df, props):
    """
    Plots histograms given in the DataFrame `df` (one from each row), and sets
    the visual properties from `props` onto the plot.

    The DataFrame must have these columns (in any order):
    - `key` (string or `None`): An internal (unique) identifier for each histogram
    - `label`: The name of the histogram, this will appear on the legend, string
    - `binedges`: A NumPy array storing the histograms' bins' edges.
        Must contain one more element than the number of bins.
    - `binvalues`: A NumPy array storing the height of each bin in the histogram
    - `underflows`: The (weighted) sum of the underflowed samples (pseudo-bin before the first real one), float
    - `overflows`: The (weighted) sum of the overflowed samples (pseudo-bin after the last one), float
    - `min`: The minimum of the collected samples, this is the left edge of the underflow "bin", float
    - `max`: The maximum of the collected samples, this is the right edge of the overflow "bin", float

    The properties in `props` will be made specific to the items added by this call,
    by appending `/<key>` row-by-row to each property key.

    This can only be called if the chart is of type `HISTOGRAM`.
    """
    assert_is_native_chart()

    if sorted(list(df.columns)) != sorted(["key", "label", "binedges", "binvalues", "underflows", "overflows", "min", "max"]):
        raise RuntimeError("Invalid DataFrame format in plot_histogram")

    # the key of each row is appended to the property names in Java
    Gateway.chart_plotter.plotHistograms(pl.dumps([
        {
            "key": row.key,
            "title": str(row.label),

            # this could be computed in Java as well, but just to make things simpler, we do it here
            "sumweights": float(np.sum(row.binvalues) + row.underflows + row.overflows),

            "edges": _list_to_bytes(row.binedges),
            "values": _list_to_bytes(row.binvalues),

            "underflows": float(row.underflows),
            "overflows": float(row.overflows),

            "min": float(row.min),
            "max": float(row.max),
        }
        for row in df.itertuples(index=False)
    ]), MapConverter().convert(props, Gateway.gateway._gateway_client))

def set_group_titles(labels):
    Gateway.chart_plotter.setGroupTitles(labels)

def _list_to_bytes(l):
    """
    Internal. Encodes/serializes a list or `np.array` of numbers as
    8-byte floats (doubles) into an `np.array` of bytes.
    """
    return np.array(np.array(l), dtype=np.dtype('>f8')).tobytes()


def _put_array_in_shm(arr, shm_objs, mmap_objs):
    """
    Internal. Turns a `np.array` into a byte sequence, and writes it into
    a newly created (platform-specific) SHM object with the given name.
    Returns a space-separated string, holding the name and the size
    of the SHM object.
    """
    if arr.size == 0:
        return "<EMPTY> 0"

    global _vector_data_counter
    # they only need to start with a / on POSIX, but Windows doesn't seem to mind, so just putting it there always...
    name = "/vecdata-" + str(os.getpid()) + "-" + str(_vector_data_counter)
    _vector_data_counter += 1

    system = platform.system()

    if system in ['Linux', 'Darwin']:
        mem = posix_ipc.SharedMemory(name, posix_ipc.O_CREAT | posix_ipc.O_EXCL, size=arr.nbytes)

        shm_objs.append(mem)

        if system == 'Darwin':
            # for some reason we can't write to the shm fd directly on mac, only after mmap-ing it
            with mmap.mmap(mem.fd, length=mem.size) as mf:
                mf.write(arr.astype(np.dtype('>f8')).tobytes())
        else:
            with open(mem.fd, 'wb') as mf:
                arr.astype(np.dtype('>f8')).tofile(mf)

    elif system == 'Windows':
        # on Windows, the mmap module in itself provides shared memory functionality
        mm = mmap.mmap(-1, arr.nbytes, tagname=name)
        mmap_objs.append(mm)
        mm.write(arr.astype(np.dtype('>f8')).tobytes())
    else:
        raise RuntimeError("unsupported platform")

    return name + " " + str(arr.nbytes)
