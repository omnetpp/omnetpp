"""
Provides access to the simulation results for the chart script. It lets the
chart script query simulation results and metadata referenced by the .anf file
they are in. The results are returned as Pandas `DataFrame`'s of various
formats.

Note that this module is stateful. It is set up appropriately by the OMNeT++ IDE
or `opp_charttool` before the chart script is run.

**Filter expressions**

The `filter_or_dataframe` parameters in all functions have the same syntax. It is
always evaluated independently on every loaded result item or metadata entry,
and its value determines whether the given item or piece of metadata is included
in the returned `DataFrame`.

A filter expression is composed of terms that can be combined with the `AND`,
`OR`, `NOT` operators, and parentheses. A term filters for the value of some property of
the item, and has the form `<property> =~ <pattern>`, or simply `<pattern>`. The latter
is equivalent to `name =~ <pattern>`.

The following properties are available:
- `name`: Name of the result or item.
- `module`: Full path of the result's module.
- `type`: Type of the item. Value is one of: `scalar`, `vector`, `parameter`, `histogram`, `statistics`.
- `isfield`: `true` is the item is a synthetic scalar that represents a field of statistic or a vector, `false` if not.
- `file`: File name of the result or item.
- `run`: Unique run ID of the run that contains the result or item.
- `runattr:<name>`: Run attribute of the run that contains the result or item. Example: `runattr:measurement`.
- `attr:<name>`: Attribute of the result. Example: `attr:unit`.
- `itervar:<name>`: Iteration variable of the run that contains the result or item. Example: `itervar:numHosts`.
- `config:<key>`: Configuration key of the run that contains the result or item. Example: `config:sim-time-limit`, `config:**.sendIaTime`.

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

Example: `module =~ "**.host*" AND (name =~ "pkSent*" OR name =~ "pkRecvd*"`


**Metadata columns**

Several query functions have the `include_attrs`, `include_runattrs`,
`include_itervars`, `include_param_assignments`, and `include_config_entries`
boolean options. When such an option is turned on, it will generate extra
columns into dataframe, one for each result attribute, run attribute, iteration
variable, etc. When there is a name clash among items of different types, the
column name for the second one will be modified by adding its type after
an underscore (`_runattr`, `_itervar`, `_config`, `_param`).

- `include_attrs`: Adds the attributes of the result in question
- `include_runattrs`: Adds the run attributes of the (result's) run
- `include_itervars`: Adds the iteration variables of the (result's) run
- `include_config_entries`: Adds all configuration entries of the (result's) run,
   including parameter parameter assignments and per-object configuration options.
   If this option is turned on, `include_param_assignments` has no effect.
- `include_param_assignments`: Adds the configuration entries that set module or
   channel parameters. This is a subset of the entries added by `include_config_entries`.

Note that values in metadata columns are generally strings (with missing values
represented as `None` or `nan`). The Pandas `to_numeric()` function or
`utils.to_numeric()` can be used to convert values to `float` or `int` where needed.
"""

# Technically, this module only delegates all of its functions to one of two different
# implementations of the API described here, based on whether it is running from within the IDE
# or not (for example, from opp_charttool), detected using the WITHIN_OMNETPP_IDE environment variable.

import os

if os.getenv("WITHIN_OMNETPP_IDE") == "yes":
    from omnetpp.scave.impl_ide import results as impl
else:
    from .impl_charttool import results as impl

from math import inf
from functools import wraps

import pandas as pd

from omnetpp.scave.utils import _pivot_results

def _guarded_result_query_func(func):
    @wraps(func)
    def inner(filter_or_dataframe, **rest):
        if type(filter_or_dataframe) is str and not filter_or_dataframe:
            raise ValueError("Empty filter expression")
        # TODO: add else: assert on dataframe columns
        try:
            return func(filter_or_dataframe, **rest)
        except Exception as e:
            if "Parse error in match expression: syntax error" in str(e):
                raise ValueError("Syntax error in result filter expression")
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


def load_results(filenames, filter_expression="", include_fields_as_scalars=False, vector_start_time=-inf, vector_end_time=inf):
    if not filter_expression:
        raise ValueError("Empty filter expression")
    return impl.load_results(**locals())

@_guarded_result_query_func
def get_results(filter_or_dataframe="", row_types=None, omit_unused_columns=True, include_fields_as_scalars=False, start_time=-inf, end_time=inf):
    # row_types = ['runattr', 'itervar', 'config', 'scalar', 'vector', 'statistic', 'histogram', 'param', 'attr']

    """
    Returns a filtered set of results and metadata in CSV-like format.
    The items can be any type, even mixed together in a single `DataFrame`.
    They are selected from the complete set of data referenced by the analysis file (`.anf`),
    including only those for which the given `filter_or_dataframe` evaluates to `True`.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the desired items. Example: `module =~ "*host*" AND name =~ "numPacket*"`
    - `row_types`: Optional. When given, filters the returned rows by type. Should be a unique list, containing any number of these strings:
      `"runattr"`, `"itervar"`, `"config"`, `"scalar"`, `"vector"`, `"statistic"`, `"histogram"`, `"param"`, `"attr"`
    - `omit_unused_columns` (bool): Optional. If `True`, all columns that would only contain `None` are removed from the returned DataFrame
    - `include_fields_as_scalars` (bool): Optional. If `True`, the fields of statistics and histograms (`:min`, `:mean`, etc.) are also
      returned as synthetic scalars.
    - `start_time`, `end_time` (double): Optional time limits to trim the data of vector type results.
      The unit is seconds, both the `vectime` and `vecvalue` arrays will be affected, the interval is left-closed, right-open.

    Columns of the returned DataFrame:

    - `runID` (string):  Identifies the simulation run
    - `type` (string): Row type, one of the following: scalar, vector, statistics, histogram, runattr, itervar, param, attr
    - `module` (string): Hierarchical name (a.k.a. full path) of the module that recorded the result item
    - `name` (string): Name of the result item (scalar, statistic, histogram or vector)
    - `attrname` (string): Name of the run attribute or result item attribute (in the latter case, the module and name columns identify the result item the attribute belongs to)
    - `attrvalue` (string): Value of run and result item attributes, iteration variables, saved ini param settings (runattr, attr, itervar, param)
    - `value` (double or string): Output scalar or parameter value
    - `count`, `sumweights`, `mean`, `min`, `max`, `stddev` (double): Fields of the statistics or histogram
    - `binedges`, `binvalues` (np.array): Histogram bin edges and bin values, as space-separated lists. `len(binedges)==len(binvalues)+1`
    - `underflows`, `overflows` (double): Sum of weights (or counts) of underflown and overflown samples of histograms
    - `vectime`, `vecvalue` (np.array): Output vector time and value arrays, as space-separated lists
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_results(**locals())
    else:
        df = filter_or_dataframe
        if row_types is not None:
            df = df[df["type"].isin(row_types)]

        if omit_unused_columns:
            df.dropna(axis='columns', how='all', inplace=True)

        df.reset_index(inplace=True, drop=True)
        return df




@_guarded_result_query_func
def get_runs(filter_or_dataframe="", include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of runs, identified by their run ID.

    Parameters:

    - `filter_or_dataframe`: The filter expression to select the desired runs.
      Example: `runattr:network =~ "Aloha" AND config:Aloha.slotTime =~ 0`
    - `include_runattrs`, `include_itervars`, `include_param_assignments`, `include_config_entries` (bool):
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the result, pivoted into columns.
      See the "Metadata columns" section of the module documentation for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - Additional metadata items (run attributes, iteration variables, etc.), as requested
    """
    filter_expression = filter_or_dataframe
    del filter_or_dataframe
    return impl.get_runs(**locals())


@_guarded_result_query_func
def get_runattrs(filter_or_dataframe="", include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of run attributes.

    The set of run attributes is fixed: `configname`, `datetime`, `experiment`,
    `inifile`, `iterationvars`, `iterationvarsf`, `measurement`, `network`,
    `processid`, `repetition`, `replication`, `resultdir`, `runnumber`, `seedset`.

    Parameters:

    - `filter_or_dataframe`: The filter expression to select the desired run attributes.
      Example: `name =~ *date* AND config:Aloha.slotTime =~ 0`
    - `include_runattrs`, `include_itervars`, `include_param_assignments`, `include_config_entries` (bool):
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the result, pivoted into columns.
      See the "Metadata columns" section of the module documentation for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `name` (string): The name of the run attribute
    - `value` (string): The value of the run attribute
    - Additional metadata items (run attributes, iteration variables, etc.)
    """
    filter_expression = filter_or_dataframe
    del filter_or_dataframe
    return impl.get_runattrs(**locals())


@_guarded_result_query_func
def get_itervars(filter_or_dataframe="", include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of iteration variables.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the desired iteration variables.
      Example: `name =~ iaMean AND config:Aloha.slotTime =~ 0`
    - `include_runattrs`, `include_itervars`, `include_param_assignments`, `include_config_entries` (bool):
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the result, pivoted into columns.
      See the "Metadata columns" section of the module documentation for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `name` (string): The name of the iteration variable
    - `value` (string): The value of the iteration variable.
    - Additional metadata items (run attributes, iteration variables, etc.), as requested
    """
    filter_expression = filter_or_dataframe
    del filter_or_dataframe
    return impl.get_itervars(**locals())


@_guarded_result_query_func
def get_scalars(filter_or_dataframe="", include_attrs=False, include_fields=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False):
    """
    Returns a filtered list of scalar results.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the desired scalars.
      Example: `name =~ "channelUtilization*" AND runattr:replication =~ "#0"`
    - `include_attrs` (bool): Optional. When set to `True`, result attributes (like `unit`
      or `source` for example) are appended to the DataFrame, pivoted into columns.
    - `include_fields` (bool): Optional. If `True`, the fields of statistics and histograms
      (`:min`, `:mean`, etc.) are also returned as synthetic scalars.
    - `include_runattrs`, `include_itervars`, `include_param_assignments`, `include_config_entries` (bool):
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the DataFrame, pivoted into columns.
      See the "Metadata columns" section of the module documentation for details.
    - `merge_module_and_name` (bool): Optional. When set to `True`, the value in the `module` column
      is prepended to the value in the `name` column, joined by a period, in every row.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `module` (string): Hierarchical name (a.k.a. full path) of the module that recorded the result item
    - `name` (string): The name of the scalar
    - `value` (double): The value of the scalar
    - Additional metadata items (result attributes, run attributes, iteration variables, etc.), as requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_scalars(**locals())
    else:
        df = filter_or_dataframe
        return _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name)


@_guarded_result_query_func
def get_parameters(filter_or_dataframe="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False):
    """
    Returns a filtered list of parameters - actually computed values of individual `cPar` instances in the fully built network.

    Parameters are considered "pseudo-results", similar to scalars - except their values are strings. Even though they act
    mostly as input to the actual simulation run, the actually assigned value of individual `cPar` instances is valuable information,
    as it is the result of the network setup process. For example, even if a parameter is set up as an expression like `normal(3, 0.4)`
    from `omnetpp.ini`, the returned DataFrame will contain the single concrete value picked for every instance of the parameter.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the desired parameters.
      Example: `name =~ "x" AND module =~ Aloha.server`
    - `include_attrs` (bool): Optional. When set to `True`, result attributes (like `unit` for
      example) are appended to the DataFrame, pivoted into columns.
    - `include_runattrs`, `include_itervars`, `include_param_assignments`, `include_config_entries` (bool):
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the DataFrame, pivoted into columns.
      See the "Metadata columns" section of the module documentation for details.
    - `merge_module_and_name` (bool): Optional. When set to `True`, the value in the `module` column
      is prepended to the value in the `name` column, joined by a period, in every row.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `module` (string): Hierarchical name (a.k.a. full path) of the module that recorded the result item
    - `name` (string): The name of the parameter
    - `value` (string): The value of the parameter.
    - Additional metadata items (result attributes, run attributes, iteration variables, etc.), as requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_parameters(**locals())
    else:
        df = filter_or_dataframe
        return _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name)


@_guarded_result_query_func
def get_vectors(filter_or_dataframe="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False, start_time=-inf, end_time=inf):
    """
    Returns a filtered list of vector results.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the desired vectors.
      Example: `name =~ "radioState*" AND runattr:replication =~ "#0"`
    - `include_attrs` (bool): Optional. When set to `True`, result attributes (like `unit`
      or `source` for example) are appended to the DataFrame, pivoted into columns.
    - `include_runattrs`, `include_itervars`, `include_param_assignments`, `include_config_entries` (bool):
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the DataFrame, pivoted into columns.
      See the "Metadata columns" section of the module documentation for details.
    - `merge_module_and_name` (bool): Optional. When set to `True`, the value in the `module` column
      is prepended to the value in the `name` column, joined by a period, in every row.
    - `start_time`, `end_time` (double): Optional time limits to trim the data of vector type results.
      The unit is seconds, both the `vectime` and `vecvalue` arrays will be affected, the interval is left-closed, right-open.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `module` (string): Hierarchical name (a.k.a. full path) of the module that recorded the result item
    - `name` (string): The name of the vector
    - `vectime`, `vecvalue` (np.array): The simulation times and the corresponding values in the vector
    - Additional metadata items (result attributes, run attributes, iteration variables, etc.), as requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_vectors(**locals())
    else:
        df = filter_or_dataframe
        return _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name)



@_guarded_result_query_func
def get_statistics(filter_or_dataframe="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False):
    """
    Returns a filtered list of statistics results.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the desired statistics.
      Example: `name =~ "collisionLength:stat" AND itervar:iaMean =~ "5"`
    - `include_attrs` (bool): Optional. When set to `True`, result attributes (like `unit`
      or `source` for example) are appended to the DataFrame, pivoted into columns.
    - `include_runattrs`, `include_itervars`, `include_param_assignments`, `include_config_entries` (bool):
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the DataFrame, pivoted into columns.
      See the "Metadata columns" section of the module documentation for details.
    - `merge_module_and_name` (bool): Optional. When set to `True`, the value in the `module` column
      is prepended to the value in the `name` column, joined by a period, in every row.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `module` (string): Hierarchical name (a.k.a. full path) of the module that recorded the result item
    - `name` (string): The name of the vector
    - `count`, `sumweights`, `mean`, `stddev`, `min`, `max` (double): The characteristic mathematical properties of the statistics result
    - Additional metadata items (result attributes, run attributes, iteration variables, etc.), as requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_statistics(**locals())
    else:
        df = filter_or_dataframe
        return _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name)



@_guarded_result_query_func
def get_histograms(filter_or_dataframe="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False):
    """
    Returns a filtered list of histogram results.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the desired histogram.
      Example: `name =~ "collisionMultiplicity:histogram" AND itervar:iaMean =~ "2"`
    - `include_attrs` (bool): Optional. When set to `True`, result attributes (like `unit`
      or `source` for example) are appended to the DataFrame, pivoted into columns.
    - `include_runattrs`, `include_itervars`, `include_param_assignments`, `include_config_entries` (bool):
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the DataFrame, pivoted into columns.
      See the "Metadata columns" section of the module documentation for details.
    - `merge_module_and_name` (bool): Optional. When set to `True`, the value in the `module` column
      is prepended to the value in the `name` column, joined by a period, in every row.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `module` (string): Hierarchical name (a.k.a. full path) of the module that recorded the result item
    - `name` (string): The name of the vector
    - `count`, `sumweights`, `mean`, `stddev`, `min`, `max` (double): The characteristic mathematical properties of the histogram
    - `binedges`, `binvalues` (np.array): The histogram edge locations and the weighted sum of the collected samples in each bin. `len(binedges) == len(binvalues) + 1`
    - `underflows`, `overflows` (double): The weighted sum of the samples that fell outside of the histogram bin range in the two directions
    - Additional metadata items (result attributes, run attributes, iteration variables, etc.), as requested
    """
    if type(filter_or_dataframe) is str:
        filter_expression = filter_or_dataframe
        del filter_or_dataframe
        return impl.get_histograms(**locals())
    else:
        df = filter_or_dataframe
        return _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name)



@_guarded_result_query_func
def get_config_entries(filter_or_dataframe, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of config entries. That is: parameter assignment patterns; and global and per-object config options.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the desired config entries.
      Example: `name =~ sim-time-limit AND itervar:numHosts =~ 10`
    - `include_runattrs`, `include_itervars`, `include_param_assignments`, `include_config_entries` (bool):
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the result, pivoted into columns.
      See the "Metadata columns" section of the module documentation for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `name` (string): The name of the config entry
    - `value` (string): The value of the config entry
    - Additional metadata items (run attributes, iteration variables, etc.), as requested
    """

    filter_expression = filter_or_dataframe
    del filter_or_dataframe
    return impl.get_config_entries(**locals())


@_guarded_result_query_func
def get_param_assignments(filter_or_dataframe, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of parameter assignment patterns. The result is a subset of what `get_config_entries`
    would return with the same arguments.

    Parameters:

    - `filter_or_dataframe` (string): The filter expression to select the desired parameter assignments.
      Example: `name =~ **.flowID AND itervar:numHosts =~ 10`
    - `include_runattrs`, `include_itervars`, `include_param_assignments`, `include_config_entries` (bool):
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the result, pivoted into columns.
      See the "Metadata columns" section of the module documentation for details.

    Columns of the returned DataFrame:

    - `runID` (string): Identifies the simulation run
    - `name` (string): The parameter assignment pattern
    - `value` (string): The assigned value
    - Additional metadata items (run attributes, iteration variables, etc.), as requested
    """
    filter_expression = filter_or_dataframe
    del filter_or_dataframe
    return impl.get_param_assignments(**locals())

