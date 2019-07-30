import pandas as pd
import numpy as np
import platform
import pickle
import time

# posix_ipc is required for POSIX shm on Linux and Mac
if platform.system() in ['Linux', 'Darwin']:
    import posix_ipc
import mmap

from omnetpp.internal import Gateway
from omnetpp.internal.TimeAndGuard import TimeAndGuard

import functools
from math import inf
print = functools.partial(print, flush=True)

# CSV format
def get_results(filter_expression="", row_types=['runattr', 'itervar', 'param', 'scalar', 'vector', 'statistics', 'histogram', 'attr'], omit_unused_columns=True, start_time=-inf, end_time=inf):

    pk = Gateway.results_provider.getResultsPickle(filter_expression, row_types, omit_unused_columns, start_time, end_time)

    df = pd.DataFrame(pickle.loads(pk), columns=[
        "runID", "type", "module", "name", "attrname", "attrvalue",
        "value", "count", "sumweights", "mean", "stddev", "min", "max",
        "underflows", "overflows", "binedges", "binvalues", "vectime", "vecvalue"])

    df["binedges"] = df["binedges"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')
    df["binvalues"] = df["binvalues"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')

    df["vectime"] = df["vectime"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')
    df["vecvalue"] = df["vecvalue"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')

    # TODO: do this filtering in Java instead, might be faster
    df = df[df['type'].isin(row_types)]

    if omit_unused_columns:
        df.dropna(axis='columns', how='all', inplace=True)

    return df

# simple format
def get_result_attrs(filter_expression="", include_runattrs=False, include_itervars=False):
    pk = Gateway.results_provider.getResultAttrsPickle(filter_expression)
    return pd.DataFrame(pickle.loads(pk), columns=["runID", "module", "name", "attrname", "attrvalue"])


def get_runs(filter_expression="", include_runattrs=False, include_itervars=False):
    pk = Gateway.results_provider.getRunsPickle(filter_expression, include_runattrs, include_itervars)
    runs, runattrs, itervars = pickle.loads(pk)
    df = pd.DataFrame(runs, columns=["runID"])
    return _append_additional_data(df, None, runattrs, itervars)

# simple format
def get_run_attrs(filter_expression="", include_runattrs=False, include_itervars=False):
    pk = Gateway.results_provider.getRunAttrsPickle(filter_expression)
    df = pd.DataFrame(pickle.loads(pk), columns=["runID", "name", "value"])
    runattrs = None
    itervars = None
    if include_runattrs:
        runattrs = df
    if include_itervars:
        itervars = get_itervars(filter_expression)
    return _append_additional_data(df, None, runattrs, itervars)

def get_itervars(filter_expression="", include_runattrs=False, include_itervars=False):
    pk = Gateway.results_provider.getItervarsPickle(filter_expression)
    df = pd.DataFrame(pickle.loads(pk), columns=["runID", "name", "value"])
    runattrs = None
    itervars = None
    if include_runattrs:
        runattrs = pickle.loads(Gateway.results_provider.getRunAttrsForItervarsPickle(filter_expression))
    if include_itervars:
        itervars = df
    return _append_additional_data(df, None, runattrs, itervars)

def _append_additional_data(df, attrs, runattrs, itervars):
    if attrs is not None:
        attrs = pd.DataFrame(attrs, columns=["runID", "module", "name", "attrname", "attrvalue"])
        attrs = pd.pivot_table(attrs, columns="attrname", aggfunc='first', index=["runID", "module", "name"], values="attrvalue")
        df = df.merge(attrs, left_on=["runID", "module", "name"], right_index=True, how='left')

    if runattrs is not None:
        runattrs = pd.DataFrame(runattrs, columns=["runID", "name", "value"])
        runattrs = pd.pivot_table(runattrs, columns="name", aggfunc='first', index="runID", values="value")
        df = df.join(runattrs, on="runID", rsuffix="_runattr")

    if itervars is not None:
        itervars = pd.DataFrame(itervars, columns=["runID", "name", "value"])
        itervars = pd.pivot_table(itervars, columns="name", aggfunc='first', index="runID", values="value")
        df = df.join(itervars, on="runID", rsuffix="_itervar")

    return df

def get_scalars(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, merge_module_and_name=False):
    pk = Gateway.results_provider.getScalarsPickle(filter_expression, include_attrs, include_runattrs, include_itervars, merge_module_and_name)

    scalars, attrs, runattrs, itervars = pickle.loads(pk)
    df = pd.DataFrame(scalars, columns=["runID", "module", "name", "value"])

    return _append_additional_data(df, attrs, runattrs, itervars)


def _get_array_from_shm(name):
    system = platform.system()

    name, size = name.split(" ")
    size = int(size)

    if system in ['Linux', 'Darwin']:
        mem = posix_ipc.SharedMemory(name)

        if system == 'Darwin':
            # for some reason we can't directly np.memmap the shm file, because it is "unseekable"
            # but the mmap module works with it, so we just copy the data into np, and release the shared memory
            with mmap.mmap(mem.fd, length=mem.size) as mf:
                arr = np.frombuffer(mf.read(), dtype=np.dtype('>f8'))
        else:
            # on Linux, we can just continue to use the existing shm memory without copying
            with open(mem.fd, 'wb') as mf:
                arr = np.memmap(mf, dtype=np.dtype('>f8'))

        # on Mac we are done with shm (data is copied), on Linux we can delete the name even though the mapping is still in use
        mem.unlink()
    elif system == 'Windows':
        # on Windows, the mmap module in itself provides shared memory functionality. and we copy the data here as well.
        with mmap.mmap(-1, size, tagname=name) as mf:
            arr = np.frombuffer(mf.read(), dtype=np.dtype('>f8'))
    else:
        raise RuntimeError("unsupported platform")

    return arr

def get_vectors(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, merge_module_and_name=False, start_time=-inf, end_time=inf):
    pk = Gateway.results_provider.getVectorsPickle(filter_expression, include_attrs, include_runattrs, include_itervars, merge_module_and_name, float(start_time), float(end_time))

    scalars, attrs, runattrs, itervars = pickle.loads(pk)
    df = pd.DataFrame(scalars, columns=["runID", "module", "name", "vectime", "vecvalue"])

    df["vectime"] = df["vectime"].map(_get_array_from_shm)
    df["vecvalue"] = df["vecvalue"].map(_get_array_from_shm)

    return _append_additional_data(df, attrs, runattrs, itervars)

def get_statistics(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, merge_module_and_name=False):
    pk = Gateway.results_provider.getStatisticsPickle(filter_expression, include_attrs, include_runattrs, include_itervars, merge_module_and_name)

    scalars, attrs, runattrs, itervars = pickle.loads(pk)
    df = pd.DataFrame(scalars, columns=["runID", "module", "name", "count", "sumweights", "mean", "stddev", "min", "max"])

    return _append_additional_data(df, attrs, runattrs, itervars)

def get_histograms(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, merge_module_and_name=False, include_statistics_fields=False):
    pk = Gateway.results_provider.getHistogramsPickle(filter_expression, include_attrs, include_runattrs, include_itervars, merge_module_and_name)

    scalars, attrs, runattrs, itervars = pickle.loads(pk)
    df = pd.DataFrame(scalars, columns=["runID", "module", "name", "count", "sumweights", "mean", "stddev", "min", "max", "underflows", "overflows", "binedges", "binvalues"])

    df["binedges"] = df["binedges"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')
    df["binvalues"] = df["binvalues"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')

    return _append_additional_data(df, attrs, runattrs, itervars)


"""


def get_param_patterns(filter_expression, include_runattrs=False, include_itervars=False):
    pass

def get_param_values(filter_expression, include_runattrs=False, include_itervars=False):
    pass


"""
