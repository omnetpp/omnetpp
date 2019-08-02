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


def _get_array_from_shm(name):
    if not name:
        return None

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


# CSV format
def get_results(filter_expression="", row_types=['runattr', 'itervar', 'config', 'scalar', 'vector', 'statistic', 'histogram', 'param', 'attr'], omit_unused_columns=True, start_time=-inf, end_time=inf):

    pk = Gateway.results_provider.getResultsPickle(filter_expression, row_types, omit_unused_columns, start_time, end_time)

    df = pd.DataFrame(pickle.loads(pk), columns=[
        "runID", "type", "module", "name", "attrname", "attrvalue",
        "value", "count", "sumweights", "mean", "stddev", "min", "max",
        "underflows", "overflows", "binedges", "binvalues", "vectime", "vecvalue"])

    df["binedges"] = df["binedges"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')
    df["binvalues"] = df["binvalues"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')

    df["vectime"] = df["vectime"].map(_get_array_from_shm)
    df["vecvalue"] = df["vecvalue"].map(_get_array_from_shm)

    if omit_unused_columns:  # maybe do this in Java?
        df.dropna(axis='columns', how='all', inplace=True)

    return df


def _append_metadata_columns(df, metadata_pickle, suffix):
    metadata_df = pd.DataFrame(pickle.loads(metadata_pickle), columns=["runID", "name", "value"])
    metadata_df = pd.pivot_table(metadata_df, columns="name", aggfunc='first', index="runID", values="value")
    return df.join(metadata_df, on="runID", rsuffix=suffix)


def _append_additional_data(df, attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    if attrs is not None:
        attrs = pd.DataFrame(attrs, columns=["runID", "module", "name", "attrname", "attrvalue"])
        attrs = pd.pivot_table(attrs, columns="attrname", aggfunc='first', index=["runID", "module", "name"], values="attrvalue")
        df = df.merge(attrs, left_on=["runID", "module", "name"], right_index=True, how='left')

    runs = list(df["runID"].unique())

    if include_itervars:
        itervars = Gateway.results_provider.getItervarsForRunsPickle(runs)
        df = _append_metadata_columns(df, itervars, "_itervar")

    if include_runattrs:
        runattrs = Gateway.results_provider.getRunAttrsForRunsPickle(runs)
        df = _append_metadata_columns(df, runattrs, "_runattr")

    if include_param_assignments:
        params = Gateway.results_provider.getParamAssignmentsForRunsPickle(runs)
        df = _append_metadata_columns(df, params, "_param")

    if include_config_entries:
        entries = Gateway.results_provider.getConfigEntriesForRunsPickle(runs)
        df = _append_metadata_columns(df, entries, "_config")

    return df

def get_runs(filter_expression="", include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    pk = Gateway.results_provider.getRunsPickle(filter_expression)
    runs, runattrs, itervars = pickle.loads(pk)
    df = pd.DataFrame(runs, columns=["runID"])
    return _append_additional_data(df, None, include_runattrs, include_itervars, include_param_assignments, include_config_entries)


def get_run_attrs(filter_expression="", include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    pk = Gateway.results_provider.getRunAttrsPickle(filter_expression)
    df = pd.DataFrame(pickle.loads(pk), columns=["runID", "name", "value"])
    return _append_additional_data(df, None, include_runattrs, include_itervars, include_param_assignments, include_config_entries)


def get_itervars(filter_expression="", include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    pk = Gateway.results_provider.getItervarsPickle(filter_expression)
    df = pd.DataFrame(pickle.loads(pk), columns=["runID", "name", "value"])
    return _append_additional_data(df, None, include_runattrs, include_itervars, include_param_assignments, include_config_entries)


def get_scalars(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, include_params=False, merge_module_and_name=False):
    pk = Gateway.results_provider.getScalarsPickle(filter_expression, include_attrs, merge_module_and_name)

    scalars, attrs = pickle.loads(pk)
    df = pd.DataFrame(scalars, columns=["runID", "module", "name", "value"])

    return _append_additional_data(df, attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)


def get_param_values(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False, as_numeric=False):
    pk = Gateway.results_provider.getParamValuesPickle(filter_expression, include_attrs, merge_module_and_name)

    params, attrs = pickle.loads(pk)
    df = pd.DataFrame(params, columns=["runID", "module", "name", "value"])

    return _append_additional_data(df, attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)


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


def get_vectors(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False, start_time=-inf, end_time=inf):
    pk = Gateway.results_provider.getVectorsPickle(filter_expression, include_attrs, merge_module_and_name, float(start_time), float(end_time))

    scalars, attrs = pickle.loads(pk)
    df = pd.DataFrame(scalars, columns=["runID", "module", "name", "vectime", "vecvalue"])

    df["vectime"] = df["vectime"].map(_get_array_from_shm)
    df["vecvalue"] = df["vecvalue"].map(_get_array_from_shm)

    return _append_additional_data(df, attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)


def get_statistics(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False):
    pk = Gateway.results_provider.getStatisticsPickle(filter_expression, include_attrs, merge_module_and_name)

    scalars, attrs = pickle.loads(pk)
    df = pd.DataFrame(scalars, columns=["runID", "module", "name", "count", "sumweights", "mean", "stddev", "min", "max"])

    return _append_additional_data(df, attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)


def get_histograms(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False, include_statistics_fields=False):
    pk = Gateway.results_provider.getHistogramsPickle(filter_expression, include_attrs, merge_module_and_name)

    scalars, attrs = pickle.loads(pk)
    df = pd.DataFrame(scalars, columns=["runID", "module", "name", "count", "sumweights", "mean", "stddev", "min", "max", "underflows", "overflows", "binedges", "binvalues"])

    df["binedges"] = df["binedges"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')
    df["binvalues"] = df["binvalues"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')

    return _append_additional_data(df, attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)


def get_config_entries(filter_expression, exclude_config_options=False, exclude_param_assignments=True, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    pk = Gateway.results_provider.getConfigEntriesPickle(filter_expression)
    df = pd.DataFrame(pickle.loads(pk), columns=["runID", "name", "value"])

    return _append_additional_data(df, None, include_runattrs, include_itervars, include_param_assignments, include_config_entries)

