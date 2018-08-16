import pandas as pd
import numpy as np
import pickle
import time

import Gateway
from TimeAndGuard import TimeAndGuard

import functools
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
    scalars = df[(df.type == "scalar") & (df.module != '_runattrs_')][["run", "module", "name", "value"]]
    scalars.set_index(['run', 'module', 'name'], inplace=True)
    scalars.columns = pd.MultiIndex.from_product([['result'], scalars.columns])

    # print(scalars)

    attrs = df[df.type == "attr"][["run", "module", "name", "attrname", "attrvalue"]]

    # print(attrs)

    # we know that the attrname values are unique for every run-module-name index, but pandas doesn't, so it will
    # give us a series of 1 element length to aggregate. we will simply return the single element of that series
    attrs_pivot = attrs.pivot_table(index=["run", "module", "name"], columns="attrname", values="attrvalue", aggfunc=lambda s: s[s.first_valid_index()])
    attrs_pivot.columns = pd.MultiIndex.from_product([['attr'], attrs_pivot.columns])

    # print(attrs)

    joined = scalars
    joined = joined.join(attrs_pivot)
    joined = joined.join(itervars_pivot, on='run')
    joined = joined.join(params_pivot, on='run')
    joined = joined.join(runattrs_pivot, on='run')

    # print(joined)
    joined.reset_index(inplace=True)
    joined.set_index([('runattr', 'experiment'), ('runattr', 'measurement'), ('runattr', 'replication'), ('module', ""), ('name', "")], inplace=True)
    #joined.sort_index(inplace=True)
    #joined.index.names = ["experiment", "measurement", "replication", "module", "name"]

    # joined.rename({'':'runid'}, axis="columns", level=1, inplace=True)
    # joined.rename({'run':'runattr'}, axis="columns", level=0, inplace=True)

    # print(joined.columns)
    # print(joined)

    return joined


def getScalars(filter):
    time0 = time.perf_counter()
    pckl = Gateway.results_provider.getScalarsPickle(filter)
    # f = open("/home/attila/scalars_flat.pkl", "wb")
    # f.write(pckl)
    time1 = time.perf_counter()
    unpickled = pickle.loads(pckl)
    time2 = time.perf_counter()

    df = pd.DataFrame(unpickled, columns=["run", "type", "module", "name", "attrname", "attrvalue", "value"])

    time3 = time.perf_counter()
    # print("times: getting pickle: " + str((time1 - time0) * 1000.0) + " ms, unpickling: " + str((time2 - time1) * 1000.0) + " df constr:" + str((time3 - time2) * 1000.0) + " ms")

    # pickle.dump(df, open("/home/attila/scalars_df.pkl", "wb"))

    return df


def pivotScalars1(df):
    attrs = df[df.type.isin(["runattr", "itervar"])][["run", "attrname", "attrvalue"]]

    scalars = df[df.type == "scalar"][["run", "module", "name", "value"]]

    runattrs_pivot = attrs.pivot(index="run", columns="attrname", values="attrvalue")

    scalars_merged = scalars.merge(runattrs_pivot, left_on="run", right_index=True)
    scalars_merged.set_index(["run", "module", "name"])

    df = scalars_merged[scalars_merged.module != "_runattrs_"].set_index(["run", "module", "name"])
    # print(df, flush=True)
    return df


# @TimeAndGuard(measureTime=False)
def pivotScalars(df, columns=["module"], index=["name"], values=None):

    df = pivotScalars1(df)

    df = df.pivot_table(columns=columns, index=index, values=values)

    # print(df, flush=True)

    if isinstance(df.columns, pd.MultiIndex):
        if df.columns.nlevels > 1:
            if not df.columns.names[0]:
                df.columns = df.columns.droplevel()  # the "value" label

    return df


def getVectors(filter):
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


def getStatistics(filter):
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


def getHistograms(filter):
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

