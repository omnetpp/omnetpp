import math
import pandas as pd
import numpy as np
import platform
import pickle

# posix_ipc is required for POSIX shm on Linux and Mac
if platform.system() in ['Linux', 'Darwin']:
    import posix_ipc
import mmap

from omnetpp.internal import Gateway

import functools
print = functools.partial(print, flush=True)

def _load_pickle_from_shm(name_and_size : str):
    """
    Internal. Opens a shared memory object (region, file, content) in a platform-specific
    way, unpickles its whole content, and returns the loaded object.
    `name_and_size` should be a space-separated pair of an object name and an integer,
    which is the size of the named SHM object in bytes.
    """
    if not name_and_size:
        return None

    name, size = name_and_size.split(" ")
    size = int(size)

    if name == "<EMPTY>" and size == 0:
        return None

    system = platform.system()
    if system in ['Linux', 'Darwin']:
        mem = posix_ipc.SharedMemory(name)

        with mmap.mmap(mem.fd, mem.size) as mf:
            mf.write_byte(1)
            mf.seek(8)
            p = pickle.load(mf)

    elif system == 'Windows':
        # on Windows, the mmap module in itself provides shared memory functionality
        with mmap.mmap(-1, size, tagname=name) as mf:
            mf.write_byte(1)
            mf.seek(8)
            p = pickle.load(mf)
    else:
        raise RuntimeError("unsupported platform")

    return p

# TODO: use in more places instead of _load_pickle_from_shm
def _load_df_from_shm(name_and_size : str):
    """
    Internal. Wraps `_load_pickle_from_shm`, constructing a DataFrame from the returned
    list of tuples (records), using the first one as column header.
    """
    results = _load_pickle_from_shm(name_and_size)

    header = results[0]
    data = results[1:]

    if data:
        # padding the first row to have as many elements as the header,
        # so Pandas is willing to fill the rest of the rows with None...
        padding = [None] * (len(header) - len(data[0]))
        data[0] = (*data[0], *padding)

    return pd.DataFrame(data, columns=header)


def _get_array_from_shm(name_and_size : str):
    """
    Internal. Opens a shared memory object (region, file, content) in a platform-specific
    way, and returns the whole contents of it as a np.array of doubles.
    `name_and_size` should be a space-separated pair of an object name and an integer,
    which is the size of the named SHM object in bytes.
    This function writes a 1 into the first byte of the SHM object, to notify the
    sender about the consumption of the buffer, and does not read the first 8 bytes,
    which is an added header reserved for this purpose.
    These 8 bytes are included in the "size" part of name_and_size, except for
    the special case of "<EMPTY> 0".
    """
    if not name_and_size:
        return None

    name, size = name_and_size.split(" ")
    size = int(size)

    if name == "<EMPTY>" and size == 0:
        return np.array([])

    system = platform.system()
    if system in ['Linux', 'Darwin']:
        mem = posix_ipc.SharedMemory(name)

        if system == 'Darwin':
            # for some reason we can't directly np.memmap the shm file, because it is "unseekable"
            # but the mmap module works with it, so we just copy the data into np, and release the shared memory
            with mmap.mmap(mem.fd, length=mem.size) as mf:
                mf.write_byte(1)
                mf.seek(0)
                arr = np.frombuffer(mf.read(), dtype=np.double, offset=8, count=int((size-8)/8))
        else:
            # on Linux, we can just continue to use the existing shm memory without copying
            with open(mem.fd, 'wb') as mf:
                mf.write(b"\1")
                mf.seek(0)
                arr = np.memmap(mf, dtype=np.double, offset=8, shape=(int((size-8)/8),))

        # on Mac we are done with shm (data is copied), on Linux we can delete the name even though the mapping is still in use
    elif system == 'Windows':
        # on Windows, the mmap module in itself provides shared memory functionality. and we copy the data here as well.
        with mmap.mmap(-1, size, tagname=name) as mf: # should NOT subtract the 8 bytes of the header from the size here!
            mf.write_byte(1)
            mf.seek(0)
            arr = np.frombuffer(mf.read(), dtype=np.double, offset=8, count=int((size-8)/8))
    else:
        raise RuntimeError("unsupported platform")

    return arr


def read_result_files(**_):
    raise RuntimeError("This is not available inside the IDE. Use the get_* functions with filter expressions.")

def set_inputs(**_):
    raise RuntimeError("This is not available inside the IDE. The inputs are configured on the Inputs tab.")

def add_inputs(**_):
    raise RuntimeError("This is not available inside the IDE. The inputs are configured on the Inputs tab.")


def get_results(filter_expression, row_types, omit_unused_columns, include_fields_as_scalars, start_time, end_time):
    if row_types is None:
        row_types = ["config", "runattr", "itervar", "scalar", "vector", "statistic", "histogram", "param", "attr"]

    shmnames = Gateway.results_provider.getResultsPickle(filter_expression, list(row_types), False, bool(include_fields_as_scalars), float(start_time), float(end_time))

    df = _load_df_from_shm(shmnames[0])

    df["binedges"] = df["binedges"].map(lambda v: np.frombuffer(v, dtype=np.double), na_action='ignore')
    df["binvalues"] = df["binvalues"].map(lambda v: np.frombuffer(v, dtype=np.double), na_action='ignore')

    def getter(v):
        # skip lines that aren't vectors
        if v is None or math.isnan(v):
            return v
        return _get_array_from_shm(shmnames[int(v)])

    df["vectime"] = df["vectime"].map(getter)
    df["vecvalue"] = df["vecvalue"].map(getter)

    if omit_unused_columns:  # maybe do this in Java?
        df.dropna(axis='columns', how='all', inplace=True)

    return df


def _append_metadata_columns(df, metadata, suffix):
    """
    Internal. Helper for _append_additional_data().
    """
    metadata_df = pd.DataFrame(metadata, columns=["runID", "name", "value"])
    metadata_df = pd.pivot_table(metadata_df, columns="name", aggfunc='first', index="runID", values="value")
    if metadata_df.empty:
        return df
    else:
        return df.join(metadata_df, on="runID", rsuffix=suffix)


def _append_additional_data(df, attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    """
    Internal. Performs the pivoting and appending of additional run/result
    metadata as columns onto the result DataFrames.
    """
    if attrs is not None:
        attrs = pd.DataFrame(attrs, columns=["runID", "module", "name", "attrname", "attrvalue"])
        if not attrs.empty:
            attrs = pd.pivot_table(attrs, columns="attrname", aggfunc='first', index=["runID", "module", "name"], values="attrvalue")
            df = df.merge(attrs, left_on=["runID", "module", "name"], right_index=True, how='left')

    runs = list(df["runID"].unique())

    if include_itervars:
        shmname = Gateway.results_provider.getItervarsForRunsPickle(runs) # TODO
        itervars = _load_pickle_from_shm(shmname)
        df = _append_metadata_columns(df, itervars, "_itervar")
    if include_runattrs:
        shmname = Gateway.results_provider.getRunAttrsForRunsPickle(runs)
        runattrs = _load_pickle_from_shm(shmname)
        df = _append_metadata_columns(df, runattrs, "_runattr")
    if include_config_entries:
        shmname = Gateway.results_provider.getConfigEntriesForRunsPickle(runs)  # TODO
        entries = _load_pickle_from_shm(shmname)
        df = _append_metadata_columns(df, entries, "_config")
    elif include_param_assignments and not include_config_entries:  # param_assignments are a subset of config_entries
        shmname = Gateway.results_provider.getParamAssignmentsForRunsPickle(runs)  # TODO
        params = _load_pickle_from_shm(shmname)
        df = _append_metadata_columns(df, params, "_param")

    return df


def get_serial():
    return Gateway.results_provider.getSerial()


def get_runs(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    shmname = Gateway.results_provider.getRunsPickle(filter_expression)
    runs = _load_pickle_from_shm(shmname)
    df = pd.DataFrame({"runID": runs})
    return _append_additional_data(df, None, include_runattrs, include_itervars, include_param_assignments, include_config_entries)


def get_runattrs(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    shmname = Gateway.results_provider.getRunAttrsPickle(filter_expression)
    runattrs = _load_pickle_from_shm(shmname)
    df = pd.DataFrame(runattrs, columns=["runID", "name", "value"])
    return _append_additional_data(df, None, include_runattrs, include_itervars, include_param_assignments, include_config_entries)


def get_itervars(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    shmname = Gateway.results_provider.getItervarsPickle(filter_expression)
    itervars = _load_pickle_from_shm(shmname)
    df = pd.DataFrame(itervars, columns=["runID", "name", "value"])
    return _append_additional_data(df, None, include_runattrs, include_itervars, include_param_assignments, include_config_entries)


def get_config_entries(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    shmname = Gateway.results_provider.getConfigEntriesPickle(filter_expression)
    configentries = _load_pickle_from_shm(shmname)
    df = pd.DataFrame(configentries, columns=["runID", "name", "value"])
    return _append_additional_data(df, None, include_runattrs, include_itervars, include_param_assignments, include_config_entries)


def get_param_assignments(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    shmname = Gateway.results_provider.getParamAssignmentsPickle(filter_expression)
    paramassignments = _load_pickle_from_shm(shmname)
    df = pd.DataFrame(paramassignments, columns=["runID", "name", "value"])
    return _append_additional_data(df, None, include_runattrs, include_itervars, include_param_assignments, include_config_entries)


def get_scalars(filter_expression, include_attrs, include_fields, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    shmname = Gateway.results_provider.getScalarsPickle(filter_expression, include_attrs, include_fields)
    scalars, attrs = _load_pickle_from_shm(shmname)
    df = pd.DataFrame(scalars, columns=["runID", "module", "name", "value"])

    df =_append_additional_data(df, attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
    df["value"] = pd.to_numeric(df["value"], errors="raise")

    return df


def get_parameters(filter_expression, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    shmname = Gateway.results_provider.getParamValuesPickle(filter_expression, include_attrs)
    parameters, attrs = _load_pickle_from_shm(shmname)
    df = pd.DataFrame(parameters, columns=["runID", "module", "name", "value"])

    df = _append_additional_data(df, attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
    return df


def get_vectors(filter_expression, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, start_time, end_time):
    shmnames = Gateway.results_provider.getVectorsPickle(filter_expression, include_attrs, float(start_time), float(end_time))
    vectors, attrs = _load_pickle_from_shm(shmnames[0])
    df = pd.DataFrame(vectors, columns=["runID", "module", "name", "vectime", "vecvalue"])

    def getter(v):
        return _get_array_from_shm(shmnames[int(v)])

    df["vectime"] = df["vectime"].map(getter)
    df["vecvalue"] = df["vecvalue"].map(getter)

    df = _append_additional_data(df, attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
    return df


def get_statistics(filter_expression, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    shmname = Gateway.results_provider.getStatisticsPickle(filter_expression, include_attrs)
    statistics, attrs = _load_pickle_from_shm(shmname)
    df = pd.DataFrame(statistics, columns=["runID", "module", "name", "count", "sumweights", "mean", "stddev", "min", "max"])

    df = _append_additional_data(df, attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
    return df


def get_histograms(filter_expression, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    shmname = Gateway.results_provider.getHistogramsPickle(filter_expression, include_attrs)
    histograms, attrs = _load_pickle_from_shm(shmname)
    df = pd.DataFrame(histograms, columns=["runID", "module", "name", "count", "sumweights", "mean", "stddev", "min", "max", "underflows", "overflows", "binedges", "binvalues"])

    df["binedges"] = df["binedges"].map(lambda v: np.frombuffer(v, dtype=np.double), na_action='ignore')
    df["binvalues"] = df["binvalues"].map(lambda v: np.frombuffer(v, dtype=np.double), na_action='ignore')

    df = _append_additional_data(df, attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
    return df
