import pandas as pd
import numpy as np
import pickle
import time

from omnetpp.internal import Gateway
from omnetpp.internal.TimeAndGuard import TimeAndGuard

import functools
from math import inf
print = functools.partial(print, flush=True)


def transform_results(df):
    # print(df)

    runattrs = df[df.type == "runattr"][["run", "attrname", "attrvalue"]]
    params = df[df.type == "param"][["run", "attrname", "attrvalue"]]
    itervars = df[df.type == "itervar"][["run", "attrname", "attrvalue"]]

    # print(runattrs)
    # print(params)
    # print(itervars)

    # we only keep the effective (first in the .sca file) parameter assignments, sorry...
    params.drop_duplicates(subset=["run", "attrname"], inplace=True)

    runattrs_pivot = runattrs.pivot(index="run", columns="attrname", values="attrvalue")
    params_pivot = params.pivot(index="run", columns="attrname", values="attrvalue")
    itervars_pivot = itervars.pivot(index="run", columns="attrname", values="attrvalue")

    runattrs_pivot.columns = pd.MultiIndex.from_product([['runattr'], runattrs_pivot.columns])
    params_pivot.columns = pd.MultiIndex.from_product([['param'], params_pivot.columns])
    itervars_pivot.columns = pd.MultiIndex.from_product([['itervar'], itervars_pivot.columns])

    # print(runattrs_pivot)
    # print(params_pivot)
    # print(itervars_pivot)

    # the stuff recorded as scalars with _runattrs_ as module is redundant, since we include the runattrs anyways
    scalars = df[df.type == "scalar"]
    if not scalars.empty:
        scalars = scalars[["run", "module", "name", "value"]]
        scalars.set_index(['run', 'module', 'name'], inplace=True)
        scalars.columns = pd.MultiIndex.from_product([['result'], scalars.columns])
    # print(scalars)

    vectors = df[df.type == "vector"]
    if not vectors.empty:
        vectors = vectors[["run", "module", "name", "vectime", "vecvalue"]]
        vectors.set_index(['run', 'module', 'name'], inplace=True)
        vectors.columns = pd.MultiIndex.from_product([['result'], vectors.columns])
    # print(vectors)

    histograms = df[df.type == "histogram"]
    if not histograms.empty:
        histograms = histograms[["run", "module", "name", "count", "sumweights", "mean", "stddev", "min", "max", "binedges", "binvalues"]]
        histograms.set_index(['run', 'module', 'name'], inplace=True)
        histograms.columns = pd.MultiIndex.from_product([['result'], histograms.columns])
    # print(histograms)

    statistics = df[df.type == "statistic"]
    if not statistics.empty:
        statistics = statistics[["run", "module", "name", "count", "sumweights", "mean", "stddev", "min", "max"]]
        statistics.set_index(['run', 'module', 'name'], inplace=True)
        statistics.columns = pd.MultiIndex.from_product([['result'], statistics.columns])
    # print(statistics)

    attrs = df[df.type == "attr"]
    if not attrs.empty:
        attrs = attrs[["run", "module", "name", "attrname", "attrvalue"]]
        # print(attrs)

        # we know that the attrname values are unique for every run-module-name index, but pandas doesn't, so it will
        # give us a series of 1 element length to aggregate. we will simply return the single element of that series
        first = lambda s: s[s.first_valid_index()]
        attrs_pivot = attrs.pivot_table(index=["run", "module", "name"], columns="attrname", values="attrvalue", aggfunc=first)
        attrs_pivot.columns = pd.MultiIndex.from_product([['attr'], attrs_pivot.columns])

        # print(attrs_pivot)
    else:
        attrs_pivot = pd.DataFrame()

    joined = pd.DataFrame()

    if not scalars.empty:
        joined = scalars if joined.empty else joined.join(scalars)
    if not vectors.empty:
        joined = vectors if joined.empty else joined.join(vectors)
    if not histograms.empty:
        joined = histograms if joined.empty else joined.join(histograms)
    if not statistics.empty:
        joined = statistics if joined.empty else joined.join(statistics)

    if not attrs_pivot.empty:
        joined = joined.join(attrs_pivot)

    if not itervars_pivot.empty:
        joined = joined.join(itervars_pivot, on='run')
    if not params_pivot.empty:
        joined = joined.join(params_pivot, on='run')
    if not runattrs_pivot.empty:
        joined = joined.join(runattrs_pivot, on='run')
    # print(joined)

    if not joined.empty:
        joined.reset_index(inplace=True)

        joined.set_index([('runattr', 'experiment'), ('runattr', 'measurement'), ('runattr', 'replication'), ('module', ""), ('name', "")], inplace=True)
        joined.sort_index(inplace=True)
        joined.index.names = ["experiment", "measurement", "replication", "module", "name"]

        joined.rename({'':'runid'}, axis="columns", level=1, inplace=True)
        joined.rename({'run':'runattr'}, axis="columns", level=0, inplace=True)

        joined.sort_index(axis="columns", inplace=True)

        joined = joined.reindex(['result', 'attr', 'itervar', 'param', 'runattr'], axis="columns", level=0)
    # print(joined.columns)
    # print(joined)

    return joined


def filter_metadata(df, attrs=[], itervars=[], params=[], runattrs=[], droplevel=True):
    keys = [k for k in list(df.columns) if k[0] == 'result']

    l = lambda s, b: [k for k in list(df.columns) if k[0] == s] if b == "all" else [(s, a) for a in b]

    keys += l("attr", attrs)
    keys += l("param", params)
    keys += l("itervar", itervars)
    keys += l("runattr", runattrs)

    df = df[keys]

    if droplevel:
        df.columns = df.columns.droplevel()

    return df

def get_vectors(filter):
    time0 = time.perf_counter()
    pk = Gateway.results_provider.getVectorsPickle(filter)
    # f = open("/home/attila/vectors.pkl", "wb")
    # f.write(pk)
    time1 = time.perf_counter()
    unpickled = pickle.loads(pk)
    time2 = time.perf_counter()

    df = pd.DataFrame(unpickled, columns=["run", "type", "module", "name", "attrname", "attrvalue", "vectime", "vecvalue"])

    df["vectime"] = df["vectime"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')
    df["vecvalue"] = df["vecvalue"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')
    df["attrvalue"] = pd.to_numeric(df["attrvalue"], errors='ignore')

    time3 = time.perf_counter()
    # print("times: getting pickle: " + str((time1 - time0) * 1000.0) + " ms, unpickling: " + str((time2 - time1) * 1000.0) + " df constr:" + str((time3 - time2) * 1000.0) + " ms")

    # pickle.dump(df, open("/home/attila/vectors_df.pkl", "wb"))

    return df


def get_statistics(filter):
    time0 = time.perf_counter()
    pk = Gateway.results_provider.getStatisticsPickle(filter)
    # f = open("/home/attila/statistics.pkl", "wb")
    # f.write(pk)
    time1 = time.perf_counter()
    unpickled = pickle.loads(pk)
    time2 = time.perf_counter()

    df = pd.DataFrame(unpickled, columns=["run", "type", "module", "name", "attrname", "attrvalue", "count", "sumweights", "mean", "stddev", "min", "max"])

    time3 = time.perf_counter()
    # print("times: getting pickle: " + str((time1 - time0) * 1000.0) + " ms, unpickling: " + str((time2 - time1) * 1000.0) + " df constr:" + str((time3 - time2) * 1000.0) + " ms")

    # pickle.dump(df, open("/home/attila/statistics_df.pkl", "wb"))

    return df


def get_histograms(filter):
    time0 = time.perf_counter()
    pk = Gateway.results_provider.getHistogramsPickle(filter)
    # f = open("/home/attila/histograms.pkl", "wb")
    # f.write(pk)
    time1 = time.perf_counter()
    unpickled = pickle.loads(pk)
    time2 = time.perf_counter()

    df = pd.DataFrame(unpickled, columns=["run", "type", "module", "name", "attrname", "attrvalue", "count", "sumweights", "mean", "stddev", "min", "max", "binedges", "binvalues"])

    df["binedges"] = df["binedges"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')
    df["binvalues"] = df["binvalues"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')

    time3 = time.perf_counter()
    # print("times: getting pickle: " + str((time1 - time0) * 1000.0) + " ms, unpickling: " + str((time2 - time1) * 1000.0) + " df constr:" + str((time3 - time2) * 1000.0) + " ms")

    # pickle.dump(df, open("/home/attila/histograms_df.pkl", "wb"))

    return df


def get_transformed_results(filter):

    df = pd.DataFrame()

    df = df.append(transform_results(get_scalars(filter)))
    df = df.append(transform_results(get_vectors(filter)))
    df = df.append(transform_results(get_statistics(filter)))
    df = df.append(transform_results(get_histograms(filter)))

    return df




# CSV format
def get_results(filter_expression="", row_types=['runattr', 'itervar', 'param', 'scalar', 'vector', 'statistics', 'histogram', 'attr'], omit_unused_columns=True, start_time=-inf, end_time=inf):

    pk = Gateway.results_provider.getResultsPickle(filter_expression, row_types, omit_unused_columns, start_time, end_time)

    df = pd.DataFrame(pickle.loads(pk), columns=[
        "runID", "type", "module", "name", "attrname", "attrvalue",
        "value", "count", "sumweights", "mean", "stddev", "min", "max",
        "binedges", "binvalues", "vectime", "vecvalue"])

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

def get_vectors(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, merge_module_and_name=False, start_time=-inf, end_time=inf):
    pk = Gateway.results_provider.getVectorsPickle(filter_expression, include_attrs, include_runattrs, include_itervars, merge_module_and_name, float(start_time), float(end_time))

    scalars, attrs, runattrs, itervars = pickle.loads(pk)
    df = pd.DataFrame(scalars, columns=["runID", "module", "name", "vectime", "vecvalue"])

    df["vectime"] = df["vectime"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')
    df["vecvalue"] = df["vecvalue"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')

    return _append_additional_data(df, attrs, runattrs, itervars)

def get_statistics(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, merge_module_and_name=False):
    pk = Gateway.results_provider.getStatisticsPickle(filter_expression, include_attrs, include_runattrs, include_itervars, merge_module_and_name)

    scalars, attrs, runattrs, itervars = pickle.loads(pk)
    df = pd.DataFrame(scalars, columns=["runID", "module", "name", "count", "sumweights", "mean", "stddev", "min", "max"])

    return _append_additional_data(df, attrs, runattrs, itervars)

def get_histograms(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, merge_module_and_name=False, include_statistics_fields=False):
    pk = Gateway.results_provider.getHistogramsPickle(filter_expression, include_attrs, include_runattrs, include_itervars, merge_module_and_name)

    scalars, attrs, runattrs, itervars = pickle.loads(pk)
    df = pd.DataFrame(scalars, columns=["runID", "module", "name", "count", "sumweights", "mean", "stddev", "min", "max", "binedges", "binvalues"])

    df["binedges"] = df["binedges"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')
    df["binvalues"] = df["binvalues"].map(lambda v: np.frombuffer(v, dtype=np.dtype('>f8')), na_action='ignore')

    return _append_additional_data(df, attrs, runattrs, itervars)


"""


def get_param_patterns(filter_expression, include_runattrs=False, include_itervars=False):
    pass

def get_param_values(filter_expression, include_runattrs=False, include_itervars=False):
    pass


"""
