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
    assert_is_native_chart()
    msg = None
    try:
        Gateway.chart_plotter.setProperty(key, value)
    except py4j.protocol.Py4JJavaError as e:
        msg = e.java_exception.getMessage()

    if msg is not None:
        raise RuntimeError(msg)


def set_properties(props):
    assert_is_native_chart()
    msg = None
    try:
        Gateway.chart_plotter.setProperties(dict(props))
    except py4j.protocol.Py4JJavaError as e:
        msg = e.java_exception.getMessage()


    if msg is not None:
        raise RuntimeError(msg)


def get_supported_property_keys():
    assert_is_native_chart()
    return set(Gateway.chart_plotter.getSupportedPropertyKeys())


def set_warning(warning):
    # TODO maybe we need a common interface?
    if Gateway.widget_provider:
        Gateway.widget_provider.setWarning(warning)

    if Gateway.chart_plotter:
        Gateway.chart_plotter.setWarning(warning)

def plot_lines(df, props):  # key, label, xs, ys
    assert_is_native_chart()
    if sorted(list(df.columns)) != sorted(["key", "label", "xs", "ys"]):
        raise RuntimeError("Invalid DataFrame format in plot_lines")

    # only used on posix, to unlink them later
    shm_objs = list()
    # only used on windows, to prevent gc
    mmap_objs = list()

    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "key": row.key,
            "title": row.label,
            "xs": _put_array_in_shm(row.xs, shm_objs, mmap_objs),
            "ys": _put_array_in_shm(row.ys, shm_objs, mmap_objs)
        }
        for row in df.itertuples(index=False)
    ]), MapConverter().convert(props, Gateway.gateway._gateway_client))

    # this is a no-op on Windows
    for o in shm_objs:
        o.unlink()


def plot_bars(df, props):
    # TODO: add check for one-layer indices? numbers-only data?
    assert_is_native_chart()
    if sorted(list(df.columns)) != sorted(["key", "label", "values"]):
        raise RuntimeError("Invalid DataFrame format in plot_bars")

    Gateway.chart_plotter.plotScalars(pl.dumps([
        {
            "key": str(row.key),
            "title": str(row.label),
            "values": _list_to_bytes(row.values),
        }
        for row in df.itertuples(index=False)
    ]), props)


def plot_histograms(df, props):  # key, label, binedges, binvalues, underflows, overflows, min, max
    assert_is_native_chart()

    if sorted(list(df.columns)) != sorted(["key", "label", "binedges", "binvalues", "underflows", "overflows", "min", "max"]):
        raise RuntimeError("Invalid DataFrame format in plot_histogram")

    Gateway.chart_plotter.plotHistograms(pl.dumps([
        {
            "key": row.key,
            "title": row.label,

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
