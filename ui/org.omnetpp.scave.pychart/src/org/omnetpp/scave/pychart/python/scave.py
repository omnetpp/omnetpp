import pandas as pd
import numpy as np

import Gateway
import pickle as pickle

import functools
print = functools.partial(print, flush=True)

import time

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
    #print("times: getting pickle: " + str((time1 - time0) * 1000.0) + " ms, unpickling: " + str((time2 - time1) * 1000.0) + " df constr:" + str((time3 - time2) * 1000.0) + " ms")

    #pickle.dump(df, open("/home/attila/scalars_df.pkl", "wb"))

    return df


def pivotScalars(df, columns=["module"], index=["name"], values = None):
    attrs = df[df.type.isin(["runattr", "itervar"])][["run", "attrname", "attrvalue"]]
    
    scalars = df[df.type == "scalar"][["run", "module", "name", "value"]]

    runattrs_pivot = attrs.pivot(index="run", columns="attrname", values="attrvalue")

    scalars_merged = scalars.merge(runattrs_pivot, left_on="run", right_index=True)
    scalars_merged.set_index(["run", "module", "name"])
    
    df = scalars_merged[scalars_merged.module != "_runattrs_"].set_index(["run", "module", "name"])
    print(df, flush=True)
    df = df.pivot_table(columns = columns, index = index, values = values)
    
    print(df, flush=True)
    
    df.columns = df.columns.droplevel() # the "value" label
    
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
    #print("times: getting pickle: " + str((time1 - time0) * 1000.0) + " ms, unpickling: " + str((time2 - time1) * 1000.0) + " df constr:" + str((time3 - time2) * 1000.0) + " ms")

    #pickle.dump(df, open("/home/attila/vectors_df.pkl", "wb"))

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
    #print("times: getting pickle: " + str((time1 - time0) * 1000.0) + " ms, unpickling: " + str((time2 - time1) * 1000.0) + " df constr:" + str((time3 - time2) * 1000.0) + " ms")

    #pickle.dump(df, open("/home/attila/statistics_df.pkl", "wb"))

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
    #print("times: getting pickle: " + str((time1 - time0) * 1000.0) + " ms, unpickling: " + str((time2 - time1) * 1000.0) + " df constr:" + str((time3 - time2) * 1000.0) + " ms")

    #pickle.dump(df, open("/home/attila/histograms_df.pkl", "wb"))

    return df

