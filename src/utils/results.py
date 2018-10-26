import os
import io
import subprocess
import numpy as np
import pandas as pd

inputfiles = list()

# TODO - DEDUPLICATE THIS - IT WAS COPIED FROM THE IDE MODULE
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

# TODO - DEDUPLICATE THIS - IT WAS COPIED FROM THE IDE MODULE
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


# TODO - DEDUPLICATE THIS - IT WAS COPIED FROM THE IDE MODULE
def pivotScalars1(df):
    attrs = df[df.type.isin(["runattr", "itervar"])][["run", "attrname", "attrvalue"]]

    scalars = df[df.type == "scalar"][["run", "module", "name", "value"]]

    runattrs_pivot = attrs.pivot(index="run", columns="attrname", values="attrvalue")

    scalars_merged = scalars.merge(runattrs_pivot, left_on="run", right_index=True)
    scalars_merged.set_index(["run", "module", "name"])

    df = scalars_merged[scalars_merged.module != "_runattrs_"].set_index(["run", "module", "name"])
    # print(df, flush=True)
    return df


# TODO - DEDUPLICATE THIS - IT WAS COPIED FROM THE IDE MODULE
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


def _parse_if_number(s):
    try: return float(s)
    except: return True if s=="true" else False if s=="false" else s if s else None

def _parse_ndarray(s):
    return np.fromstring(s, sep=' ') if s else None


def _get_results(filter_expression, file_extension, result_type):

    output = subprocess.check_output(["opp_scavetool", "x", *[i for i in inputfiles if i.endswith(file_extension)],
        '-T', result_type, '-f', filter_expression, "-F", "CSV-R", "-o", "-"])
    # print(output.decode('utf-8'))

    # TODO: stream the output through subprocess.PIPE ?
    df = pd.read_csv(io.BytesIO(output), converters = {
        'value': _parse_if_number,
        'attrvalue': _parse_if_number,
        'binedges': _parse_ndarray,
        'binvalues': _parse_ndarray,
        'vectime': _parse_ndarray,
        'vecvalue': _parse_ndarray
    })
    # print(df)

    return df


def getVectors(filter_expression):
    return _get_results(filter_expression, '.vec', 'v')

def getScalars(filter_expression):
    return _get_results(filter_expression, '.sca', 's')

def getStatistics(filter_expression):
    return _get_results(filter_expression, '.sca', 't')

def getHistograms(filter_expression):
    return _get_results(filter_expression, '.sca', 'h')
