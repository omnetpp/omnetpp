"""
Provides access to simulation results loaded from OMNeT++ result files (.sca,
.vec). The results are returned as Pandas `DataFrame`'s of various formats.

The module can be used in several ways, depending on the environment it is run
in, and on whether the set of result files to query are specified in a stateful
or a stateless way:

1. Inside a chart script in the Analysis Tool in the Simulation IDE. In that
   mode, the set of result files to take as input are defined on the "Inputs"
   page of the editor. The `get_results()`, `get_scalars()` and similar methods
   are invoked with a filter string as first argument to select the appropriate
   subset of results from the result files. Note that this mode of operation is
   stateful: The state is set up appropriately by the IDE before the chart
   script is run.

   A similar thing happens when charts in an analysis (.anf) file are run from
   within `opp_charttool`: the tool sets up the module state before running the
   chart script, so that the getter methods invoked with a filter string will
   return result from the set of result files saved as "inputs" in the anf file.

2. Standalone stateful mode. In order to use `get_results()`, `get_scalars()`
   and similar methods with a filter string, the module needs to be configured
   via the `set_inputs()`/`add_inputs()` functions to tell it the set of result
   files to use as input for the queries. (Doing so is analogous to filling in
   the "Inputs" page in the IDE).

3. Stateless mode. It is possible to load the result files (in whole or a subset
   of results in them) into memory as a "raw" `DataFrame` using
   `read_result_files()`, and then use `get_scalars()`, `get_vectors()` and
   other getter functions with the dataframe as their first argument to produce
   `DataFrame`'s of other formats. Note that when going this route, a filter
   string can be specified to `read_result_files()` but not to the getter
   methods. However, Pandas already provides several ways for filtering the rows
   of a dataframe, for example by indexing with logical operators on columns, or
   using the `df.query()`, `df.pipe()` or `df.apply()` methods.

**Filter expressions**

The `filter_or_dataframe` parameters in all functions must contain either a
filter string, or a "raw" dataframe produced by `read_result_files()`. When it
contains a filter string, the function operates on the set of result files
configured earlier (see stateful mode above).

Filter strings of all functions have the same syntax. It is always evaluated
independently on every loaded result item or metadata entry, and its value
determines whether the given item or piece of metadata is included in the
returned `DataFrame`.

A filter expression is composed of terms that can be combined with the `AND`,
`OR`, `NOT` operators, and parentheses. A term filters for the value of some
property of the item, and has the form `<property> =~ <pattern>`, or simply
`<pattern>`. The latter is equivalent to `name =~ <pattern>`.

The following properties are available:
- `name`: Name of the result or item.
- `module`: Full path of the result's module.
- `type`: Type of the item. Value is one of: `scalar`, `vector`, `parameter`,
  `histogram`, `statistics`.
- `isfield`: `true` is the item is a synthetic scalar that represents a field of
  statistic or a vector, `false` if not.
- `file`: File name of the result or item.
- `run`: Unique run ID of the run that contains the result or item.
- `runattr:<name>`: Run attribute of the run that contains the result or item.
  Example: `runattr:measurement`.
- `attr:<name>`: Attribute of the result. Example: `attr:unit`.
- `itervar:<name>`: Iteration variable of the run that contains the result or
  item. Example: `itervar:numHosts`.
- `config:<key>`: Configuration key of the run that contains the result or item.
  Example: `config:sim-time-limit`, `config:**.sendIaTime`.

Patterns may contain the following wildcards:
- `?` matches any character except '.'
- `*` matches zero or more characters except '.'
- `**` matches zero or more characters (any character)
- `{a-z}` matches a character in range a-z
- `{^a-z}` matches a character not in range a-z
- `{32..255}` any number (i.e. sequence of digits) in range 32..255 (e.g. `99`)
- `[32..255]` any number in square brackets in range 32..255 (e.g. `[99]`)
- `\` takes away the special meaning of the subsequent character

Patterns only need to be surrounded with quotes if they contain whitespace or
other characters that would cause ambiguity in parsing the expression.

Example: `module =~ "**.host*" AND (name =~ "pkSent*" OR name =~ "pkRecvd*")`


**The "raw" dataframe format**

This dataframe format is a central one, because the content of "raw" dataframes
correspond exactly to the content result files, i.e. it is possible to convert
between result files and the "raw" dataframe format without data loss. The "raw"
dataframe format also corresponds in a one-to-one manner to the "CSV-R" export
format of the Simulation IDE and `opp_scavetool`.

The outputs of the `get_results()` and `read_result_files()` functions are in
this format, and the dataframes that can be passed as input into certain query
functions (`get_scalars()`, `get_vectors()`, `get_runs()`, etc.) are also
expected in the same format.

Columns of the `DataFrame`:

- `runID` (string):  Identifies the simulation run
- `type` (string): Row type, one of the following: scalar, vector, statistics,
  histogram, runattr, itervar, param, attr
- `module` (string): Hierarchical name (a.k.a. full path) of the module that
  recorded the result item
- `name` (string): Name of the result item (scalar, statistic, histogram or
  vector)
- `attrname` (string): Name of the run attribute or result item attribute (in
  the latter case, the module and name columns identify the result item the
  attribute belongs to)
- `attrvalue` (string): Value of run and result item attributes, iteration
  variables, saved ini param settings (runattr, attr, itervar, param)
- `value` (double or string): Output scalar or parameter value
- `count`, `sumweights`, `mean`, `min`, `max`, `stddev` (double): Fields of the
  statistics or histogram
- `binedges`, `binvalues` (np.array): Histogram bin edges and bin values.
  `len(binedges)==len(binvalues)+1`
- `underflows`, `overflows` (double): Sum of weights (or counts) of underflown
  and overflown samples of histograms
- `vectime`, `vecvalue` (np.array): Output vector time and value arrays


**Requesting metadata columns**

Several query functions have the `include_attrs`, `include_runattrs`,
`include_itervars`, `include_param_assignments`, and `include_config_entries`
boolean options. When such an option is turned on, it will add extra columns
into the returned `DataFrame`, one for each result attribute, run attribute,
iteration variable, etc. When there is a name clash among items of different
types, the column name for the second one will be modified by adding its type
after an underscore (`_runattr`, `_itervar`, `_config`, `_param`).

- `include_attrs`: Adds the attributes of the result in question
- `include_runattrs`: Adds the run attributes of the (result's) run
- `include_itervars`: Adds the iteration variables of the (result's) run
- `include_config_entries`: Adds all configuration entries of the (result's)
   run, including parameter parameter assignments and per-object configuration
   options. If this option is turned on, `include_param_assignments` has no
   effect.
- `include_param_assignments`: Adds the configuration entries that set module or
   channel parameters. This is a subset of the entries added by
   `include_config_entries`.

Note that values in metadata columns are generally strings (with missing values
represented as `None` or `nan`). The Pandas `to_numeric()` function or
`utils.to_numeric()` can be used to convert values to `float` or `int` where
needed.
"""

# Technically, this module only delegates all of its functions to one of two different
# implementations of the API described here, based on whether it is running from within the IDE
# or not (for example, from opp_charttool), detected using the WITHIN_OMNETPP_IDE environment variable.

import os

if os.getenv("WITHIN_OMNETPP_IDE") == "yes":
    from omnetpp.scave.impl_ide import results as impl
else:
    from .impl import results_standalone as impl

from math import inf
from functools import wraps

import numpy as np
import pandas as pd
import re

from ._version import __version__

from omnetpp.scave.utils import _pivot_results, _pivot_metadata, _select_param_assignments

# Nontechnical error whose text may directly be displayed to the end user.
# Subclasses ValueError for backward compatibility.
class ResultQueryError(ValueError):
    pass

def _guarded_result_query_func(func):
    @wraps(func)
    def inner(filter_or_dataframe, **rest):
        if type(filter_or_dataframe) is str and not filter_or_dataframe:
            raise ResultQueryError("Empty filter expression")
        # TODO: add else: assert on dataframe columns
        try:
            return func(filter_or_dataframe, **rest)
        except Exception as e:
            if "Parse error in match expression: syntax error" in str(e):
                raise ResultQueryError("Syntax error in result filter expression")
            elif "org.omnetpp.scave.editors.ResultFileException" in str(e):
                m = re.search(r"org.omnetpp.scave.editors.ResultFileException(.*?)\n", str(e))
                msg = m.group(1).strip(" :")
                raise ResultQueryError(msg)
            else:
                raise e
    return inner


def get_serial():
    """
    Returns an integer that is incremented every time the set of loaded results
    change, typically as a result of the IDE loading, reloading or unloading
    a scalar or vector result file. The serial can be used for invalidating
    cached intermediate results when their input changes.
    """
    return impl.get_serial()

def set_inputs(filenames):
    """
    Specifies the set of simulation result files (.vec, .sca) to use as input
    for the query functions. The argument may be a single string, or a list of
    strings. Each string is interpreted as a file or directory path, and may
    also contain wildcards. In addition to `?` and `*`, `**` (which is able to
    match several directory levels) is also accepted as wildcard. If a path
    corresponds to a directory, it is interpreted as `[ "<dir>/**/*.sca",
    "<dir>/**/*.vec" ]`, that is, all result files will be loaded from that
    directory and recursively all its subdirectories.

    Examples: `set_inputs("results/")`, `set_inputs("results/**.sca")`,
    `set_inputs(["config1/*.sca", *config2/*.sca"])`.
    """
    impl.set_inputs(filenames)

def add_inputs(filenames):
    """
    Appends to the set of simulation result files (.vec, .sca) to use as input
    for the query functions.  The argument may be a single string, or a list of
    strings. Each string is interpreted as a file or directory path, and may
    also contain wildcards (`?`, `*`, `**`). See `set_inputs()` for more details.
    """
    impl.add_inputs(filenames)

def read_result_files(filenames, filter_expression=None, include_fields_as_scalars=False, vector_start_time=-inf, vector_end_time=inf):
    """
    Loads the simulation result files specified in the first argument
    `filenames`, and returns the filtered set of results and metadata as a
    Pandas `DataFrame`.

    The `filenames` argument specifies the set of simulation result files (.vec,
    .sca) to load.  The argument may be a single string, or a list of strings.
    Each string is interpreted as a file or directory path, and may also contain
    wildcards (`?`, `*`, `**`). See `set_inputs()` for more details on this
    format.

    It is possible to limit the set of results to return by specifying a filter
    expression, and vector start/end times.

    Parameters:
    - `filenames` (string, or list of strings): Specifies the result files to
      load.
    - `filter_expression` (string): The filter expression to select the desired
      items to load. Example: `module =~ "*host*" AND name =~ "numPacket*"`
    - `include_fields_as_scalars` (bool): Optional. If `True`, the fields of
      statistics and histograms (`:min`, `:mean`, etc.) are also returned as
      synthetic scalars.
    - `vector_start_time`, `vector_end_time` (double): Optional time limits to
      trim the data of vector type results. The unit is seconds, the interval is
      left-closed, right-open.

    Returns: a `DataFrame` in the "raw" format (see the corresponding section of
    the module documentation for details).
    """
    return impl.read_result_files(**locals())

@_guarded_result_query_func
def get_results(filter_or_dataframe="", row_types=None, omit_unused_columns=True, include_fields_as_scalars=False, start_time=-inf, end_time=inf):
    """
    Returns a filtered set of results and metadata in a Pandas `DataFrame`. The
    items can be any type, even mixed together in a single `DataFrame`. They are
    selected from the complete set of data referenced by the analysis file
    (`.anf`), including only those for which the given `filter_or_dataframe`
    evaluates to `True`.

    Parameters:

    - `filter_or_dataframe` (string or dataframe): The filter expression to
      select the desired items from the inputs, or a dataframe in the "raw"
      format. Example: `module =~ "*host*" AND name =~ "numPacket*"`
    - `row_types`: Optional. When given, filters the returned rows by type.
      Should be a unique list, containing any number of these strings:
      `"runattr"`, `"itervar"`, `"config"`, `"scalar"`, `"vector"`,
      `"statistic"`, `"histogram"`, `"param"`, `"attr"`
    - `omit_unused_columns` (bool): Optional. If `True`, all columns that would
      only contain `None` are removed from the returned DataFrame
    - `include_fields_as_scalars` (bool): Optional. If `True`, the fields of
      statistics and histograms (`:min`, `:mean`, etc.) are also returned as
      synthetic scalars.
    - `start_time`, `end_time` (double): Optional time limits to trim the data
      of vector type results. The unit is seconds, both the `vectime` and
      `vecvalue` arrays will be affected, the interval is left-closed,
      right-open.

    Returns: a `DataFrame` in the "raw" format (see the corresponding section of
    the module documentation for details).
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_results(**locals())
    else:
        if include_fields_as_scalars:
            raise ValueError("include_fields_as_scalars is not supported when filter_or_dataframe is a dataframe")

        df = filter_or_dataframe
        if row_types is not None:
            df = df[df["type"].isin(row_types)]

        if omit_unused_columns:
            df = df.dropna(axis='columns', how='all')

        df.reset_index(inplace=True, drop=True)

        if start_time != -inf or end_time != inf:
            def crop(row):
                t = row['vectime']
                v = row['vecvalue']

                from_index = np.searchsorted(t, start_time, 'left')
                to_index = np.searchsorted(t, end_time, 'left')

                row['vectime'] = t[from_index:to_index]
                row['vecvalue'] = v[from_index:to_index]

                return row
            df = df.transform(crop, axis='columns')

        return df


@_guarded_result_query_func
def get_runs(filter_or_dataframe="", include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of runs, identified by their run ID.

    Parameters:
    - `filter_or_dataframe` (string or dataframe): The filter expression to
      select the desired run from the inputs, or a dataframe in the "raw" format
      (e.g. one returned by `read_result_files()`). Example: `runattr:network =~
      "Aloha" AND config:Aloha.slotTime =~ 0`
    - `include_runattrs`, `include_itervars`, `include_param_assignments`,
      `include_config_entries` (bool): Optional. When set to `True`, additional
      pieces of metadata about the run is appended to the result, pivoted into
      columns. See the "Metadata columns" section of the module documentation
      for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - Additional metadata items (run attributes, iteration variables, etc.), as
      requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_runs(**locals())
    else:
        df = filter_or_dataframe
        runs = df[["runID"]].drop_duplicates()
        row_types = ["itervar", "runattr", "config"]
        metadf = df[df["type"].isin(row_types)]
        df = _pivot_metadata(runs, metadf, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
        df.dropna(axis='columns', how='all', inplace=True)
        return df


@_guarded_result_query_func
def get_runattrs(filter_or_dataframe="", include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of run attributes.

    The set of run attributes is fixed: `configname`, `datetime`, `experiment`,
    `inifile`, `iterationvars`, `iterationvarsf`, `measurement`, `network`,
    `processid`, `repetition`, `replication`, `resultdir`, `runnumber`,
    `seedset`.

    Parameters:

    - `filter_or_dataframe` (string or dataframe): The filter expression to
      select the desired run attributes from the inputs, or a dataframe in the
      "raw" format. Example: `name =~ *date* AND config:Aloha.slotTime =~ 0`
    - `include_runattrs`, `include_itervars`, `include_param_assignments`,
      `include_config_entries` (bool): Optional. When set to `True`, additional
      pieces of metadata about the run is appended to the result, pivoted into
      columns. See the "Metadata columns" section of the module documentation
      for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `name` (string): The name of the run attribute
    - `value` (string): The value of the run attribute
    - Additional metadata items (run attributes, iteration variables, etc.)
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_runattrs(**locals())
    else:
        df = filter_or_dataframe

        runattrs = df[df["type"] == "runattr"]
        runattrs = runattrs[["runID", "attrname", "attrvalue"]]
        runattrs.rename(columns={"attrname": "name", "attrvalue": "value"}, inplace=True)
        runattrs.reset_index(inplace=True, drop=True)

        row_types = ["itervar", "runattr", "config"]
        metadf = df[df["type"].isin(row_types)]

        df = _pivot_metadata(runattrs, metadf, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
        df.dropna(axis='columns', how='all', inplace=True)

        return df


@_guarded_result_query_func
def get_itervars(filter_or_dataframe="", include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of iteration variables.

    Parameters:

    - `filter_or_dataframe` (string or dataframe): The filter expression to
      select the desired iteration variables from the inputs, or a dataframe in
      the "raw" format. Example: `name =~ iaMean AND config:Aloha.slotTime =~ 0`
    - `include_runattrs`, `include_itervars`, `include_param_assignments`,
      `include_config_entries` (bool): Optional. When set to `True`, additional
      pieces of metadata about the run is appended to the result, pivoted into
      columns. See the "Metadata columns" section of the module documentation
      for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `name` (string): The name of the iteration variable
    - `value` (string): The value of the iteration variable.
    - Additional metadata items (run attributes, iteration variables, etc.), as
      requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_itervars(**locals())
    else:
        df = filter_or_dataframe

        itervars = df[df["type"] == "itervar"]
        itervars = itervars[["runID", "attrname", "attrvalue"]]
        itervars.rename(columns={"attrname": "name", "attrvalue": "value"}, inplace=True)
        itervars.reset_index(inplace=True, drop=True)

        row_types = ["itervar", "runattr", "config"]
        metadf = df[df["type"].isin(row_types)]

        df = _pivot_metadata(itervars, metadf, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
        df.dropna(axis='columns', how='all', inplace=True)

        return df


@_guarded_result_query_func
def get_scalars(filter_or_dataframe="", include_attrs=False, include_fields=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of scalar results.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the
      desired scalars, or a dataframe in the "raw" format. Example: `name =~
      "channelUtilization*" AND runattr:replication =~ "#0"`
    - `include_attrs` (bool): Optional. When set to `True`, result attributes
      (like `unit` or `source` for example) are appended to the DataFrame,
      pivoted into columns.
    - `include_fields` (bool): Optional. If `True`, the fields of statistics and
      histograms (`:min`, `:mean`, etc.) are also returned as synthetic scalars.
    - `include_runattrs`, `include_itervars`, `include_param_assignments`,
      `include_config_entries` (bool): Optional. When set to `True`, additional
      pieces of metadata about the run is appended to the DataFrame, pivoted
      into columns. See the "Metadata columns" section of the module
      documentation for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `module` (string): Hierarchical name (a.k.a. full path) of the module that
      recorded the result item
    - `name` (string): The name of the scalar
    - `value` (double): The value of the scalar
    - Additional metadata items (result attributes, run attributes, iteration
      variables, etc.), as requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_scalars(**locals())
    else:
        if include_fields:
            raise ValueError("include_fields is not supported when filter_or_dataframe is a dataframe")

        df = filter_or_dataframe
        row_types = ["scalar", "itervar", "runattr", "config", "attr"]
        df = df[df["type"].isin(row_types)]
        df = _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
        df.dropna(axis='columns', how='all', inplace=True)
        if "value" in df: # it might be empty
            df["value"] = pd.to_numeric(df["value"], errors="raise")
        return df


@_guarded_result_query_func
def get_parameters(filter_or_dataframe="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of parameters - actually computed values of
    individual `cPar` instances in the fully built network.

    Parameters are considered "pseudo-results", similar to scalars - except
    their values are strings. Even though they act mostly as input to the actual
    simulation run, the actually assigned value of individual `cPar` instances
    is valuable information, as it is the result of the network setup process.
    For example, even if a parameter is set up as an expression like `normal(3,
    0.4)` from `omnetpp.ini`, the returned DataFrame will contain the single
    concrete value picked for every instance of the parameter.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the
      desired parameters, or a dataframe in the "raw" format. Example: `name =~
      "x" AND module =~ Aloha.server`
    - `include_attrs` (bool): Optional. When set to `True`, result attributes
      (like `unit` for example) are appended to the DataFrame, pivoted into
      columns.
    - `include_runattrs`, `include_itervars`, `include_param_assignments`,
      `include_config_entries` (bool): Optional. When set to `True`, additional
      pieces of metadata about the run is appended to the DataFrame, pivoted
      into columns. See the "Metadata columns" section of the module
      documentation for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `module` (string): Hierarchical name (a.k.a. full path) of the module that
      recorded the result item
    - `name` (string): The name of the parameter
    - `value` (string): The value of the parameter.
    - Additional metadata items (result attributes, run attributes, iteration
      variables, etc.), as requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_parameters(**locals())
    else:
        df = filter_or_dataframe
        row_types = ["param", "itervar", "runattr", "config", "attr"]
        df = df[df["type"].isin(row_types)]
        df = _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
        df.dropna(axis='columns', how='all', inplace=True)
        return df


@_guarded_result_query_func
def get_vectors(filter_or_dataframe="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, start_time=-inf, end_time=inf):
    """
    Returns a filtered list of vector results.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the
      desired vectors, or a dataframe in the "raw" format. Example: `name =~
      "radioState*" AND runattr:replication =~ "#0"`
    - `include_attrs` (bool): Optional. When set to `True`, result attributes
      (like `unit` or `source` for example) are appended to the DataFrame,
      pivoted into columns.
    - `include_runattrs`, `include_itervars`, `include_param_assignments`,
      `include_config_entries` (bool): Optional. When set to `True`, additional
      pieces of metadata about the run is appended to the DataFrame, pivoted
      into columns. See the "Metadata columns" section of the module
      documentation for details.
    - `start_time`, `end_time` (double): Optional time limits to trim the data
      of vector type results. The unit is seconds, both the `vectime` and
      `vecvalue` arrays will be affected, the interval is left-closed,
      right-open.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `module` (string): Hierarchical name (a.k.a. full path) of the module that
      recorded the result item
    - `name` (string): The name of the vector
    - `vectime`, `vecvalue` (np.array): The simulation times and the
      corresponding values in the vector
    - Additional metadata items (result attributes, run attributes, iteration
      variables, etc.), as requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_vectors(**locals())
    else:
        df = filter_or_dataframe
        row_types = ["vector", "itervar", "runattr", "config", "attr"]
        df = df[df["type"].isin(row_types)]
        df = _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
        df.dropna(axis='columns', how='all', inplace=True)

        if start_time != -inf or end_time != inf:
            def crop(row):
                t = row['vectime']
                v = row['vecvalue']

                from_index = np.searchsorted(t, start_time, 'left')
                to_index = np.searchsorted(t, end_time, 'left')

                row['vectime'] = t[from_index:to_index]
                row['vecvalue'] = v[from_index:to_index]

                return row
            df = df.transform(crop, axis='columns')

        return df


@_guarded_result_query_func
def get_statistics(filter_or_dataframe="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of statistics results.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the
      desired statistics, or a dataframe in the "raw" format. Example: `name =~
      "collisionLength:stat" AND itervar:iaMean =~ "5"`
    - `include_attrs` (bool): Optional. When set to `True`, result attributes
      (like `unit` or `source` for example) are appended to the DataFrame,
      pivoted into columns.
    - `include_runattrs`, `include_itervars`, `include_param_assignments`,
      `include_config_entries` (bool): Optional. When set to `True`, additional
      pieces of metadata about the run is appended to the DataFrame, pivoted
      into columns. See the "Metadata columns" section of the module
      documentation for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `module` (string): Hierarchical name (a.k.a. full path) of the module that
      recorded the result item
    - `name` (string): The name of the vector
    - `count`, `sumweights`, `mean`, `stddev`, `min`, `max` (double): The
      characteristic mathematical properties of the statistics result
    - Additional metadata items (result attributes, run attributes, iteration
      variables, etc.), as requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_statistics(**locals())
    else:
        df = filter_or_dataframe
        row_types = ["statistic", "itervar", "runattr", "config", "attr"]
        df = df[df["type"].isin(row_types)]
        df = _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
        df.dropna(axis='columns', how='all', inplace=True)
        return df


@_guarded_result_query_func
def get_histograms(filter_or_dataframe="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of histogram results.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the
      desired histograms, or a dataframe in the "raw" format. Example: `name =~
      "collisionMultiplicity:histogram" AND itervar:iaMean =~ "2"`
    - `include_attrs` (bool): Optional. When set to `True`, result attributes
      (like `unit` or `source` for example) are appended to the DataFrame,
      pivoted into columns.
    - `include_runattrs`, `include_itervars`, `include_param_assignments`,
      `include_config_entries` (bool): Optional. When set to `True`, additional
      pieces of metadata about the run is appended to the DataFrame, pivoted
      into columns. See the "Metadata columns" section of the module
      documentation for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `module` (string): Hierarchical name (a.k.a. full path) of the module that
      recorded the result item
    - `name` (string): The name of the vector
    - `count`, `sumweights`, `mean`, `stddev`, `min`, `max` (double): The
      characteristic mathematical properties of the histogram
    - `binedges`, `binvalues` (np.array): The histogram edge locations and the
      weighted sum of the collected samples in each bin. `len(binedges) ==
      len(binvalues) + 1`
    - `underflows`, `overflows` (double): The weighted sum of the samples that
      fell outside of the histogram bin range in the two directions
    - Additional metadata items (result attributes, run attributes, iteration
      variables, etc.), as requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_histograms(**locals())
    else:
        df = filter_or_dataframe
        row_types = ["histogram", "itervar", "runattr", "config", "attr"]
        df = df[df["type"].isin(row_types)]
        df = _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
        df.dropna(axis='columns', how='all', inplace=True)
        return df


@_guarded_result_query_func
def get_config_entries(filter_or_dataframe, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of config entries. That is: parameter assignment
    patterns; and global and per-object config options.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the
      desired config entries, or a dataframe in the "raw" format. Example: `name
      =~ sim-time-limit AND itervar:numHosts =~ 10`
    - `include_runattrs`, `include_itervars`, `include_param_assignments`,
      `include_config_entries` (bool): Optional. When set to `True`, additional
      pieces of metadata about the run is appended to the result, pivoted into
      columns. See the "Metadata columns" section of the module documentation
      for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `name` (string): The name of the config entry
    - `value` (string): The value of the config entry
    - Additional metadata items (run attributes, iteration variables, etc.), as
      requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_config_entries(**locals())
    else:
        df = filter_or_dataframe

        configentries = df[df["type"] == "config"]
        configentries = configentries[["runID", "attrname", "attrvalue"]]
        configentries.rename(columns={"attrname": "name", "attrvalue": "value"}, inplace=True)
        configentries.reset_index(inplace=True, drop=True)

        row_types = ["itervar", "runattr", "config"]
        metadf = df[df["type"].isin(row_types)]

        df = _pivot_metadata(configentries, metadf, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
        df.dropna(axis='columns', how='all', inplace=True)

        return df


@_guarded_result_query_func
def get_param_assignments(filter_or_dataframe, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of parameter assignment patterns. The result is a
    subset of what `get_config_entries` would return with the same arguments.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the
      desired parameter assignments, or a dataframe in the "raw" format.
      Example: `name =~ **.flowID AND itervar:numHosts =~ 10`
    - `include_runattrs`, `include_itervars`, `include_param_assignments`,
      `include_config_entries` (bool): Optional. When set to `True`, additional
      pieces of metadata about the run is appended to the result, pivoted into
      columns. See the "Metadata columns" section of the module documentation
      for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `name` (string): The parameter assignment pattern
    - `value` (string): The assigned value
    - Additional metadata items (run attributes, iteration variables, etc.), as
      requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_param_assignments(**locals())
    else:
        df = filter_or_dataframe

        paramassignments = _select_param_assignments(df[df["type"] == "config"])
        paramassignments = paramassignments[["runID", "attrname", "attrvalue"]]
        paramassignments.rename(columns={"attrname": "name", "attrvalue": "value"}, inplace=True)
        paramassignments.reset_index(inplace=True, drop=True)

        row_types = ["itervar", "runattr", "config"]
        metadf = df[df["type"].isin(row_types)]

        df = _pivot_metadata(paramassignments, metadf, include_runattrs, include_itervars, include_param_assignments, include_config_entries)
        df.dropna(axis='columns', how='all', inplace=True)

        return df

