"""
This module implements the same result querying API that is provided by the IDE to
chart scripts, using the wrapped oppscave library to load the .sca and .vec files.
"""

import os
import glob
import numpy as np
import pandas as pd
from enum import Enum

from omnetpp.scave.utils import _append_metadata_columns, _import_scave_bindings
from typing import Dict, List, Union, Optional

sb = _import_scave_bindings()

_global_rfm = sb.ResultFileManager()
_serial_base = 0 # is only necessary because _global_rfm is recreated in set_inputs()


def _load_files_into(rfm : sb.ResultFileManager, input_patterns : Union[str, List[str]]) -> None:
    # handle single string as if it was a one-element list...
    if type(input_patterns) == str:
        input_patterns = [ input_patterns ]

    input_patterns = list(set(input_patterns))  # make unique

    load_flags = sb.LoadFlags.LOADFLAGS_DEFAULTS
    # load_flags = RFM::NEVER_RELOAD | (indexingAllowed ? RFM::ALLOW_INDEXING : RFM::ALLOW_LOADING_WITHOUT_INDEX) | RFM::SKIP_IF_LOCKED | (verbose ? RFM::VERBOSE : 0);

    for file_arg in input_patterns:
        files_to_load = []

        if os.path.isdir(file_arg):
            matching_files = glob.glob("*.sca", root_dir=file_arg, recursive=True)
            matching_files += glob.glob("*.vec", root_dir=file_arg, recursive=True)
            files_to_load = [os.path.join(file_arg, gr) for gr in matching_files]
        else: # even if it does not look like a glob pattern, nonexistent files shouldn't cause an error
            files_to_load = glob.glob(file_arg, recursive=True)

        for file_name in files_to_load:
            rfm.loadFile(file_name, file_name, load_flags)


def set_inputs(input_patterns : Union[str, List[str]]) -> None:
    global _global_rfm, _serial_base
    #_global_rfm.clear()  #TODO: this crashes... probably needs a lot of keep_alive annotations to work
    _serial_base += _global_rfm.getSerial()
    _global_rfm = sb.ResultFileManager()
    _serial_base += 1
    add_inputs(input_patterns)


def add_inputs(input_patterns : Union[str, List[str]]) -> None:
    _load_files_into(_global_rfm, input_patterns)


_SCALAR_COLUMN_NAMES = ["value"]
_STATISTIC_COLUMN_NAMES = ["count", "sumweights", "mean", "stddev", "min", "max"]
_HISTOGRAM_COLUMN_NAMES = ["underflows", "overflows", "binedges", "binvalues"]
_VECTOR_COLUMN_NAMES = ["vectime", "vecvalue"]
_PARAMETER_COLUMN_NAMES = ["value"]


def _ensure_columns_exist(df : pd.DataFrame, columns : List[str]) -> None:
    for col in columns:
        if col not in df:
            df[col] = []


def get_serial() -> int:
    return _serial_base + _global_rfm.getSerial()


def _collect_results(rfm : sb.ResultFileManager, filter_expression : str, include_fields_as_scalars : bool, vector_start_time : float, vector_end_time : float) -> pd.DataFrame:
    results = rfm.getAllItems(include_fields_as_scalars)
    results = rfm.filterIDList(results, filter_expression)

    metadata_rows = []
    runs = _collect_runs(rfm, results)
    for runID, run in runs.items():
        ras = run.getAttributes()
        for an, av in ras.items():
            metadata_rows.append({"runID": runID, "type": "runattr", "attrname": an, "attrvalue": av})

        ivs = run.getIterationVariables()
        for an, av in ivs.items():
            metadata_rows.append({"runID": runID, "type": "itervar", "attrname": an, "attrvalue": av})

        ces = run.getConfigEntries()
        for an, av in ces:
            metadata_rows.append({"runID": runID, "type": "config", "attrname": an, "attrvalue": av})

    result_rows = []
    scalar_buffer = sb.ScalarResult()
    for r in results:
        result = rfm.getItem(r, scalar_buffer)
        run = result.getRun()

        runID = run.getRunName()
        module = result.getModuleName()
        name = result.getName()

        row = {"runID": runID, "module": module, "name": name}

        result_type = result.getItemType()

        if result_type == sb.ItemType.PARAMETER:
            row["type"] = "param" # NOTE: not "parameter"
            row["value"] = result.getValue()
        elif result_type == sb.ItemType.SCALAR:
            row["type"] = "scalar"
            row["value"] = result.getValue()
        elif result_type == sb.ItemType.VECTOR:
            row["type"] = "vector"
            # TODO: memory limit? interrupt flag? precise X? event numbers?
            arrays = sb.readVectorsIntoArrays(rfm, sb.IDList(r), False, False, simTimeStart = vector_start_time, simTimeEnd = vector_end_time)
            array = arrays[0]
            l = array.length()

            times = np.empty(l, dtype=np.float64)
            values = np.empty(l, dtype=np.float64)

            sb.xyArrayToNumpyArrays(array, times, values)

            row["vectime"] = times
            row["vecvalue"] = values
        elif result_type == sb.ItemType.STATISTICS or result_type == sb.ItemType.HISTOGRAM:
            if result_type == sb.ItemType.HISTOGRAM:
                row["type"] = "histogram"
                hist : sb.Histogram = result.getHistogram()
                row["binedges"] = np.array(hist.getBinEdges())
                row["binvalues"] = np.array(hist.getBinValues())
                row["underflows"] = hist.getUnderflows()
                row["overflows"] = hist.getOverflows()
            else:
                row["type"] = "statistic" # NOTE: not "statistics"

            stats : sb.Statistics = result.getStatistics()
            row["count"] = int(stats.getCount())
            row["sumweights"] = stats.getSumWeights()
            row["mean"] = stats.getMean()
            row["stddev"] = stats.getStddev()
            row["min"] = stats.getMin()
            row["max"] = stats.getMax()

        result_rows.append(row)

        attrs = result.getAttributes()
        for an, av in attrs.items():
            result_rows.append({"runID": runID, "type": "attr", "module": module, "name": name, "attrname": an, "attrvalue": av})

    run_metadata_df = pd.DataFrame(metadata_rows)
    results_df = pd.DataFrame(result_rows)
    df = pd.concat([run_metadata_df, results_df], ignore_index=True)

    df = df.reindex(columns=["runID", "type", "module", "name", "attrname", "attrvalue",
                             "value", "count", "sumweights", "mean", "stddev",
                             "min", "max", "underflows", "overflows",
                             "binedges", "binvalues", "vectime", "vecvalue"])

    df["count"] = df["count"].astype(np.int64, errors="ignore")

    return df


def read_result_files(filenames : Union[str, List[str]], filter_expression : str, include_fields_as_scalars : bool, vector_start_time : float, vector_end_time : float) -> pd.DataFrame:
    if type(filenames) == str:
        filenames = [ filenames ]
    if filter_expression is not None and not filter_expression:
        raise ValueError("Empty filter expression")

    rfm = sb.ResultFileManager()
    _load_files_into(rfm, filenames)

    if filter_expression is None:
        filter_expression = "*"

    return _collect_results(rfm, filter_expression, include_fields_as_scalars, vector_start_time, vector_end_time)


def get_results(filter_expression : str, row_types : Optional[List[str]], omit_unused_columns : bool, include_fields_as_scalars : bool, start_time : float, end_time : float):
    df = _collect_results(_global_rfm, filter_expression, include_fields_as_scalars, start_time, end_time)

    # TODO: optimize - don't even add ones that will be dropped...
    if row_types is not None:
        df = df[df["type"].isin(row_types)]

    if omit_unused_columns:
        df.dropna(axis='columns', how='all', inplace=True)

    df.reset_index(inplace=True, drop=True)
    return df


class _AttributeMode(Enum):
    ENSURE_ONLY_UNIT = 1
    ENSURE_UNIT_INCLUDED = 2
    NO_CHANGE = 3


def _collect_attrs(idlist : sb.IDList, attribute_mode : _AttributeMode) -> pd.DataFrame:
    runIDs = []
    modules = []
    names = []
    attrnames = []
    attrvalues = []

    scalar_buffer = sb.ScalarResult()
    for id in idlist:
        item = _global_rfm.getItem(id, scalar_buffer)
        attributes = item.getAttributes()

        if attribute_mode == _AttributeMode.ENSURE_ONLY_UNIT:
            attrkeys = ["unit"]
        elif attribute_mode == _AttributeMode.ENSURE_UNIT_INCLUDED:
            attrkeys = list(attributes.keys())
            if "unit" not in attrkeys:
                attrkeys.append("unit")
        else: # _AttributeMode.NO_CHANGE
            attrkeys = list(attributes.keys())

        runID = item.getRun().getRunName()
        module = item.getModuleName()
        name = item.getName()

        for attrname in attrkeys:
            runIDs.append(runID)
            modules.append(module)
            names.append(name)
            attrnames.append(attrname)
            attrvalues.append(attributes.get(attrname, ""))

    return pd.DataFrame({"runID" : runIDs, "module": modules, "name": names, "attrname": attrnames, "attrvalue": attrvalues})


def _collect_runs(rfm : sb.ResultFileManager, idlist : sb.IDList) -> Dict[str, sb.Run]:
    runs = {}
    scalar_buffer = sb.ScalarResult()
    for id in idlist:
        item = rfm.getItem(id, scalar_buffer)
        run = item.getRun()
        runID = run.getRunName()
        if runID not in runs:
            runs[runID] = run
    return runs


def _collect_param_assignments(runs : Dict[str, sb.Run]) -> pd.DataFrame:
    runIDs = []
    attrnames = []
    attrvalues = []

    for runID, run in runs.items():
        entries = run.getParamAssignmentConfigEntries()

        for e in entries:
            runIDs.append(runID)
            attrnames.append(e[0])
            attrvalues.append(e[1])

    return pd.DataFrame({"runID" : runIDs, "attrname": attrnames, "attrvalue": attrvalues})


def _collect_config_entries(runs : Dict[str, sb.Run]) -> pd.DataFrame:
    runIDs = []
    attrnames = []
    attrvalues = []

    for runID, run in runs.items():
        entries = run.getConfigEntries()

        for e in entries:
            runIDs.append(runID)
            attrnames.append(e[0])
            attrvalues.append(e[1])

    return pd.DataFrame({"runID" : runIDs, "attrname": attrnames, "attrvalue": attrvalues})


def _collect_run_attributes(runs : Dict[str, sb.Run]) -> pd.DataFrame:
    runIDs = []
    attrnames = []
    attrvalues = []

    for runID, run in runs.items():
        attributes = run.getAttributes()

        for n, v in attributes.items():
            runIDs.append(runID)
            attrnames.append(n)
            attrvalues.append(v)

    return pd.DataFrame({"runID" : runIDs, "attrname": attrnames, "attrvalue": attrvalues})


def _collect_iteration_variables(runs : Dict[str, sb.Run]) -> pd.DataFrame:
    runIDs = []
    attrnames = []
    attrvalues = []

    for runID, run in runs.items():
        itervars = run.getIterationVariables()

        for n, v in itervars.items():
            runIDs.append(runID)
            attrnames.append(n)
            attrvalues.append(v)

    return pd.DataFrame({"runID" : runIDs, "attrname": attrnames, "attrvalue": attrvalues})


def _add_metadata(df : pd.DataFrame, runs : Dict[str, sb.Run], include_runattrs : bool, include_itervars : bool, include_param_assignments : bool, include_config_entries : bool):
    if include_itervars:
        itervars = _collect_iteration_variables(runs)
        df = _append_metadata_columns(df, itervars, "_itervar")
    if include_runattrs:
        runattrs = _collect_run_attributes(runs)
        df = _append_metadata_columns(df, runattrs, "_runattr")
    if include_config_entries:
        configs = _collect_config_entries(runs)
        df = _append_metadata_columns(df, configs, "_config")
    if include_param_assignments and not include_config_entries:
        params = _collect_param_assignments(runs)
        df = _append_metadata_columns(df, params, "_param")
    return df


def get_scalars(filter_expression, include_attrs, include_fields, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    scalars = _global_rfm.getAllScalars(include_fields)
    scalars = _global_rfm.filterIDList(scalars, filter_expression)
    n = scalars.size()

    runIDs = np.empty(n, dtype=np.object_)
    modules = np.empty(n, dtype=np.object_)
    names = np.empty(n, dtype=np.object_)
    values = np.empty(n, dtype=np.float64)

    scalar_buffer = sb.ScalarResult()
    for i, s in enumerate(scalars):
        scalar = _global_rfm.getScalar(s, scalar_buffer)
        runIDs[i] = scalar.getRun().getRunName()
        modules[i] = scalar.getModuleName()
        names[i] = scalar.getName()
        values[i] = scalar.getValue()
    df = pd.DataFrame({"runID" : runIDs, "module": modules, "name": names, "value": values})

    attrmode = _AttributeMode.ENSURE_UNIT_INCLUDED if include_attrs else _AttributeMode.ENSURE_ONLY_UNIT
    attrs = _collect_attrs(scalars, attrmode)
    if not attrs.empty:
        attrs = pd.pivot(attrs, columns="attrname", index=["runID", "module", "name"], values="attrvalue")
        df = df.merge(attrs, left_on=["runID", "module", "name"], right_index=True, how='left', suffixes=(None, "_attr"))

    if include_itervars or include_runattrs or include_config_entries or include_param_assignments:
        runs = _collect_runs(_global_rfm, scalars)
        df = _add_metadata(df, runs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)

    _ensure_columns_exist(df, _SCALAR_COLUMN_NAMES)
    return df


def get_vectors(filter_expression, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, start_time, end_time):
    vectors = _global_rfm.getAllVectors()
    vectors = _global_rfm.filterIDList(vectors, filter_expression)
    n = vectors.size()

    runIDs = np.empty(n, dtype=np.object_)
    modules = np.empty(n, dtype=np.object_)
    names = np.empty(n, dtype=np.object_)
    vectimes = np.empty(n, dtype=np.object_)
    vecvalues = np.empty(n, dtype=np.object_)

    # TODO: memory limit? interrupt flag? precise X? event numbers?
    arrays = sb.readVectorsIntoArrays(_global_rfm, vectors, False, False, simTimeStart = start_time, simTimeEnd = end_time)
    for i, v in enumerate(vectors):
        vector = _global_rfm.getVector(v)
        runIDs[i] = vector.getRun().getRunName()
        modules[i] = vector.getModuleName()
        names[i] = vector.getName()

        array = arrays[i]

        times = np.empty(array.length(), dtype=np.float64)
        values = np.empty(array.length(), dtype=np.float64)

        sb.xyArrayToNumpyArrays(array, times, values)

        vectimes[i] = times
        vecvalues[i] = values

    df = pd.DataFrame({"runID" : runIDs, "module": modules, "name": names, "vectime": vectimes, "vecvalue": vecvalues})

    attrmode = _AttributeMode.ENSURE_UNIT_INCLUDED if include_attrs else _AttributeMode.ENSURE_ONLY_UNIT
    attrs = _collect_attrs(vectors, attrmode)
    if not attrs.empty:
        attrs = pd.pivot(attrs, columns="attrname", index=["runID", "module", "name"], values="attrvalue")
        df = df.merge(attrs, left_on=["runID", "module", "name"], right_index=True, how='left', suffixes=(None, "_attr"))

    if include_itervars or include_runattrs or include_config_entries or include_param_assignments:
        runs = _collect_runs(_global_rfm, vectors)
        df = _add_metadata(df, runs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)

    _ensure_columns_exist(df, _VECTOR_COLUMN_NAMES)
    return df


def get_statistics(filter_expression, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    statistics = _global_rfm.getAllStatistics()
    statistics = _global_rfm.filterIDList(statistics, filter_expression)
    n = statistics.size()

    runIDs = np.empty(n, dtype=np.object_)
    modules = np.empty(n, dtype=np.object_)
    names = np.empty(n, dtype=np.object_)
    counts = np.empty(n, dtype=np.float64)
    sumweights = np.empty(n, dtype=np.float64)
    means = np.empty(n, dtype=np.float64)
    stddevs = np.empty(n, dtype=np.float64)
    mins = np.empty(n, dtype=np.float64)
    maxs = np.empty(n, dtype=np.float64)

    for i, s in enumerate(statistics):
        statistic = _global_rfm.getStatistics(s)
        runIDs[i] = statistic.getRun().getRunName()
        modules[i] = statistic.getModuleName()
        names[i] = statistic.getName()

        stat = statistic.getStatistics()
        counts[i] = stat.getCount()
        sumweights[i] = stat.getSumWeights()
        means[i] = stat.getMean()
        stddevs[i] = stat.getStddev()
        mins[i] = stat.getMin()
        maxs[i] = stat.getMax()

    df = pd.DataFrame({"runID" : runIDs, "module": modules, "name": names,
                       "count": counts, "sumweights": sumweights,
                       "mean": means, "stddev": stddevs, "min": mins, "max": maxs})

    attrmode = _AttributeMode.ENSURE_UNIT_INCLUDED if include_attrs else _AttributeMode.ENSURE_ONLY_UNIT
    attrs = _collect_attrs(statistics, attrmode)
    if not attrs.empty:
        attrs = pd.pivot(attrs, columns="attrname",  index=["runID", "module", "name"], values="attrvalue")
        df = df.merge(attrs, left_on=["runID", "module", "name"], right_index=True, how='left', suffixes=(None, "_attr"))

    if include_itervars or include_runattrs or include_config_entries or include_param_assignments:
        runs = _collect_runs(_global_rfm, statistics)
        df = _add_metadata(df, runs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)

    _ensure_columns_exist(df, _STATISTIC_COLUMN_NAMES)
    return df


def get_histograms(filter_expression, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    histograms = _global_rfm.getAllHistograms()
    histograms = _global_rfm.filterIDList(histograms, filter_expression)
    n = histograms.size()

    runIDs = np.empty(n, dtype=np.object_)
    modules = np.empty(n, dtype=np.object_)
    names = np.empty(n, dtype=np.object_)
    counts = np.empty(n, dtype=np.float64)
    sumweights = np.empty(n, dtype=np.float64)
    means = np.empty(n, dtype=np.float64)
    stddevs = np.empty(n, dtype=np.float64)
    mins = np.empty(n, dtype=np.float64)
    maxs = np.empty(n, dtype=np.float64)
    underflows = np.empty(n, dtype=np.float64)
    overflows = np.empty(n, dtype=np.float64)
    binedges = np.empty(n, dtype=np.object_)
    binvalues = np.empty(n, dtype=np.object_)

    for i, h in enumerate(histograms):
        histogram = _global_rfm.getHistogram(h)
        runIDs[i] = histogram.getRun().getRunName()
        modules[i] = histogram.getModuleName()
        names[i] = histogram.getName()

        stat = histogram.getStatistics()
        counts[i] = stat.getCount()
        sumweights[i] = stat.getSumWeights()
        means[i] = stat.getMean()
        stddevs[i] = stat.getStddev()
        mins[i] = stat.getMin()
        maxs[i] = stat.getMax()

        hist = histogram.getHistogram()
        underflows[i] = hist.getUnderflows()
        overflows[i] = hist.getOverflows()
        binedges[i] = np.array(hist.getBinEdges())
        binvalues[i] = np.array(hist.getBinValues())

    df = pd.DataFrame({"runID" : runIDs, "module": modules, "name": names,
                       "count": counts, "sumweights": sumweights,
                       "mean": means, "stddev": stddevs, "min": mins, "max": maxs,
                       "underflows": underflows, "overflows": overflows,
                       "binedges": binedges, "binvalues": binvalues})

    attrmode = _AttributeMode.ENSURE_UNIT_INCLUDED if include_attrs else _AttributeMode.ENSURE_ONLY_UNIT
    attrs = _collect_attrs(histograms, attrmode)
    if not attrs.empty:
        attrs = pd.pivot(attrs, columns="attrname", index=["runID", "module", "name"], values="attrvalue")
        df = df.merge(attrs, left_on=["runID", "module", "name"], right_index=True, how='left', suffixes=(None, "_attr"))

    if include_itervars or include_runattrs or include_config_entries or include_param_assignments:
        runs = _collect_runs(_global_rfm, histograms)
        df = _add_metadata(df, runs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)

    _ensure_columns_exist(df, _HISTOGRAM_COLUMN_NAMES)
    return df


def get_parameters(filter_expression, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    parameters = _global_rfm.getAllParameters()
    parameters = _global_rfm.filterIDList(parameters, filter_expression)
    n = parameters.size()

    runIDs = np.empty(n, dtype=np.object_)
    modules = np.empty(n, dtype=np.object_)
    names = np.empty(n, dtype=np.object_)
    values = np.empty(n, dtype=np.object_)

    for i, p in enumerate(parameters):
        parameter = _global_rfm.getParameter(p)
        runIDs[i] = parameter.getRun().getRunName()
        modules[i] = parameter.getModuleName()
        names[i] = parameter.getName()
        values[i] = parameter.getValue()
    df = pd.DataFrame({"runID" : runIDs, "module": modules, "name": names, "value": values})

    if include_attrs:
        attrs = _collect_attrs(parameters, _AttributeMode.NO_CHANGE)
        if not attrs.empty:
            attrs = pd.pivot(attrs, columns="attrname", index=["runID", "module", "name"], values="attrvalue")
            df = df.merge(attrs, left_on=["runID", "module", "name"], right_index=True, how='left', suffixes=(None, "_attr"))
    if include_itervars or include_runattrs or include_config_entries or include_param_assignments:
        runs = _collect_runs(_global_rfm, parameters)
        df = _add_metadata(df, runs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)

    _ensure_columns_exist(df, _PARAMETER_COLUMN_NAMES)
    return df


def get_runs(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    runlist = _global_rfm.filterRunList(_global_rfm.getRuns(), filter_expression)
    df = pd.DataFrame({"runID" : [r.getRunName() for r in runlist]})

    if include_itervars or include_runattrs or include_config_entries or include_param_assignments:
        runs = {r.getRunName() : r for r in runlist}
        df = _add_metadata(df, runs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)

    df.reset_index(inplace=True, drop=True)
    return df


def get_runattrs(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    runlist = _global_rfm.filterRunList(_global_rfm.getRuns(), filter_expression)
    runs = {r.getRunName() : r for r in runlist}
    df = _collect_run_attributes(runs)
    df.rename(columns={"attrname" : "name", "attrvalue" : "value"}, inplace=True)

    df = _add_metadata(df, runs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
    df.reset_index(inplace=True, drop=True)

    return df


def get_itervars(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    runlist = _global_rfm.filterRunList(_global_rfm.getRuns(), filter_expression)
    runs = {r.getRunName() : r for r in runlist}
    df = _collect_iteration_variables(runs)
    df.rename(columns={"attrname" : "name", "attrvalue" : "value"}, inplace=True)

    df = _add_metadata(df, runs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
    df.reset_index(inplace=True, drop=True)

    return df


def get_config_entries(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    runlist = _global_rfm.filterRunList(_global_rfm.getRuns(), filter_expression)
    runs = {r.getRunName() : r for r in runlist}
    df = _collect_config_entries(runs)
    df.rename(columns={"attrname" : "name", "attrvalue" : "value"}, inplace=True)

    df = _add_metadata(df, runs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
    df.reset_index(inplace=True, drop=True)

    return df


def get_param_assignments(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    runlist = _global_rfm.filterRunList(_global_rfm.getRuns(), filter_expression)
    runs = {r.getRunName() : r for r in runlist}
    df = _collect_param_assignments(runs)
    df.rename(columns={"attrname" : "name", "attrvalue" : "value"}, inplace=True)

    df = _add_metadata(df, runs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
    df.reset_index(inplace=True, drop=True)

    return df
