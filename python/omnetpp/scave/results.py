"""
This module lets the scripts that power the charts in the IDE query
any simulation results and metadata referenced by the .anf file they
are in, returned as as Pandas DataFrames in various formats.

The `filter_expressions` parameter in all functions has the same syntax.
It is always evaluated independently on every loaded result item or metadata entry, and its value
determines whether the given item or piece of metadata is included in the returned `DataFrame`.
"""

# Alternatively: The results module gives access to the simulation results loaded into the IDE.
# TODO: document


# Technically, this module only delegates all of its functions to one of two different
# implementations of the API described here, based on whether it is running from within the IDE
# or not (for example, from opp_charttool), detected using the WITHIN_OMNETPP_IDE environment variable.

import os

if os.getenv("WITHIN_OMNETPP_IDE", "no") == "yes":
    from omnetpp.scave.impl_ide import results as impl
else:
    from .impl_charttool import results as impl

from math import inf

def get_results(filter_expression="", row_types=['runattr', 'itervar', 'config', 'scalar', 'vector', 'statistic', 'histogram', 'param', 'attr'], omit_unused_columns=True, start_time=-inf, end_time=inf):
    """
    Returns a filtered set of results and metadata in CSV-like format.
    The items can be any type, even mixed together in a single `DataFrame`.
    They are selected from the complete set of data referenced by the analysis file (`.anf`),
    including only those for which the given `filter_expression` evaluates to `True`.

    # Parameters

    - **filter_expression** *(string)*: The filter expression to select the desired items. Example: `module =~ "*host*" AND name =~ "numPacket*"`
    - **row_types**: Optional. When given, filters the returned rows by type. Should be a unique list, containing any number of these strings:
      `"runattr"`, `"itervar"`, `"config"`, `"scalar"`, `"vector"`, `"statistic"`, `"histogram"`, `"param"`, `"attr"`
    - **omit_unused_columns** *(bool)*: Optional. If `True`, all columns that would only contain `None` are removed from the returned DataFrame
    - **start_time**, **end_time** *(double)*: Optional time limits to trim the data of vector type results.
      The unit is seconds, both the `vectime` and `vecvalue` arrays will be affected, the interval is left-closed, right-open.

    # Columns of the returned DataFrame

    - **runID** *(string)*:  Identifies the simulation run
    - **type** *(string)*: Row type, one of the following: scalar, vector, statistics, histogram, runattr, itervar, param, attr
    - **module** *(string)*: Hierarchical name (a.k.a. full path) of the module that recorded the result item
    - **name** *(string)*: Name of the result item (scalar, statistic, histogram or vector)
    - **attrname** *(string)*: Name of the run attribute or result item attribute (in the latter case, the module and name columns identify the result item the attribute belongs to)
    - **attrvalue** *(string)*: Value of run and result item attributes, iteration variables, saved ini param settings (runattr, attr, itervar, param)
    - **value** *(double or string)*:  Output scalar or attribute value
    - **count**, **sumweights**, **mean**, **min**, **max**, **stddev** *(double)*: Fields of the statistics or histogram
    - **binedges**, **binvalues** *(np.array)*: Histogram bin edges and bin values, as space-separated lists. `len(binedges)==len(binvalues)+1`
    - **underflows**, **overflows** *(double)*: Sum of weights (or counts) of underflown and overflown samples of histograms
    - **vectime**, **vecvalue** *(np.array)*: Output vector time and value arrays, as space-separated lists
    """
    return impl.get_results(**locals())


def get_runs(filter_expression="", include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of runs, identified by their run ID.

    # Parameters

    - **filter_expression**: The filter expression to select the desired runs.
      Example: `runattr:network =~ "Aloha" AND config:Aloha.slotTime =~ 0`
    - **include_runattrs**, **include_itervars**, **include_param_assignments**, **include_config_entries** *(bool)*:
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the result, pivoted into columns.

    # Columns of the returned DataFrame

    - **runID** (string): Identifies the simulation run
    - Additional metadata items (run attributes, iteration variables, etc.), as requested
    """
    return impl.get_runs(**locals())


def get_runattrs(filter_expression="", include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of run attributes.

    The set of run attributes is fixed: `configname`, `datetime`, `experiment`,
    `inifile`, `iterationvars`, `iterationvarsf`, `measurement`, `network`,
    `processid`, `repetition`, `replication`, `resultdir`, `runnumber`, `seedset`.

    # Parameters

    - **filter_expression**: The filter expression to select the desired run attributes.
      Example: `name =~ *date* AND config:Aloha.slotTime =~ 0`
    - **include_runattrs**, **include_itervars**, **include_param_assignments**, **include_config_entries** *(bool)*:
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the result, pivoted into columns.

    # Columns of the returned DataFrame

    - **runID** *(string)*: Identifies the simulation run
    - **name** *(string)*: The name of the run attribute
    - **value** *(string)*: The value of the run attribue
    - Additional metadata items (run attributes, iteration variables, etc.), as requested
    """
    return impl.get_runattrs(**locals())


def get_itervars(filter_expression="", include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, as_numeric=False):
    """
    Returns a filtered list of iteration variables.

    # Parameters

    - **filter_expression** *(string)*: The filter expression to select the desired iteration variables.
      Example: `name =~ iaMean AND config:Aloha.slotTime =~ 0`
    - **include_runattrs**, **include_itervars**, **include_param_assignments**, **include_config_entries** *(bool)*:
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the result, pivoted into columns.
    - **as_numeric** *(bool)*: Optional. When set to `True`, the returned values will be converted to `double`.
      Non-numeric values will become `NaN`.

    # Columns of the returned DataFrame

    - **runID** *(string)*: Identifies the simulation run
    - **name** *(string)*: The name of the iteration variable
    - **value** *(string or double)*: The value of the iteration variable. Its type depends on the `as_numeric` parameter.
    - Additional metadata items (run attributes, iteration variables, etc.), as requested
    """
    return impl.get_itervars(**locals())


def get_scalars(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False):
    """
    Returns a filtered list of scalar results.

    # Parameters

    - **filter_expression** *(string)*: The filter expression to select the desired scalars.
      Example: `name =~ "channelUtilization*" AND runattr:replication =~ "#0"`
    - **include_attrs** *(bool)*: Optional. When set to `True`, result attributes (like `unit`
      or `source` for example) are appended to the DataFrame, pivoted into columns.
    - **include_runattrs**, **include_itervars**, **include_param_assignments**, **include_config_entries** *(bool)*:
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the DataFrame, pivoted into columns.
    - **merge_module_and_name** *(bool)*: Optional. When set to `True`, the value in the `module` column
      is prepended to the value in the `name` column, joined by a period, in every row.

    # Columns of the returned DataFrame

    - **runID** *(string)*: Identifies the simulation run
    - **name** *(string)*: The name of the scalar
    - **value** *(double)*: The value of the scalar
    - Additional metadata items (result attributes, run attributes, iteration variables, etc.), as requested
    """
    return impl.get_scalars(**locals())


def get_parameters(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False, as_numeric=False):
    """
    Returns a filtered list of parameters - actually computed values of individual `cPar` instances in the fully built network.

    Parameters are considered "pseudo-results", similar to scalars - except their values are strings. Even though they act
    mostly as input to the actual simulation run, the actually assigned value of individual `cPar` instances is valuable information,
    as it is the result of the network setup process. For example, even if a parameter is set up as an expression like `normal(3, 0.4)`
    from `omnetpp.ini`, the returned DataFrame will contain the single concrete value picked for every instance of the parameter.

    # Parameters

    - **filter_expression** *(string)*: The filter expression to select the desired parameters.
      Example: `name =~ "x" AND module =~ Aloha.server`
    - **include_attrs** *(bool)*: Optional. When set to `True`, result attributes (like `unit` for
      example) are appended to the DataFrame, pivoted into columns.
    - **include_runattrs**, **include_itervars**, **include_param_assignments**, **include_config_entries** *(bool)*:
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the DataFrame, pivoted into columns.
    - **merge_module_and_name** *(bool)*: Optional. When set to `True`, the value in the `module` column
      is prepended to the value in the `name` column, joined by a period, in every row.
    - **as_numeric** *(bool)*: Optional. When set to `True`, the returned values will be converted to `double`.
      Non-numeric values will become `NaN`.

    # Columns of the returned DataFrame

    - **runID** *(string)*: Identifies the simulation run
    - **name** *(string)*: The name of the parameter
    - **value** *(string or double)*: The value of the parameter. Its type depends on the `as_numeric` parameter.
    - Additional metadata items (result attributes, run attributes, iteration variables, etc.), as requested
    """
    return impl.get_parameters(**locals())


def get_vectors(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False, start_time=-inf, end_time=inf):
    """
    Returns a filtered list of vector results.

    # Parameters

    - **filter_expression** *(string)*: The filter expression to select the desired vectors.
      Example: `name =~ "radioState*" AND runattr:replication =~ "#0"`
    - **include_attrs** *(bool)*: Optional. When set to `True`, result attributes (like `unit`
      or `source` for example) are appended to the DataFrame, pivoted into columns.
    - **include_runattrs**, **include_itervars**, **include_param_assignments**, **include_config_entries** *(bool)*:
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the DataFrame, pivoted into columns.
    - **merge_module_and_name** *(bool)*: Optional. When set to `True`, the value in the `module` column
      is prepended to the value in the `name` column, joined by a period, in every row.
    - **start_time**, **end_time** *(double)*: Optional time limits to trim the data of vector type results.
      The unit is seconds, both the `vectime` and `vecvalue` arrays will be affected, the interval is left-closed, right-open.

    # Columns of the returned DataFrame

    - **runID** *(string)*: Identifies the simulation run
    - **name** *(string)*: The name of the vector
    - **vectime**, **vecvalue** *(np.array)*: The simulation times and the corresponding values in the vector
    - Additional metadata items (result attributes, run attributes, iteration variables, etc.), as requested
    """
    return impl.get_vectors(**locals())


def get_statistics(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False):
    """
    Returns a filtered list of statistics results.

    # Parameters

    - **filter_expression** *(string)*: The filter expression to select the desired statistics.
      Example: `name =~ "collisionLength:stat" AND itervar:iaMean =~ "5"`
    - **include_attrs** *(bool)*: Optional. When set to `True`, result attributes (like `unit`
      or `source` for example) are appended to the DataFrame, pivoted into columns.
    - **include_runattrs**, **include_itervars**, **include_param_assignments**, **include_config_entries** *(bool)*:
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the DataFrame, pivoted into columns.
    - **merge_module_and_name** *(bool)*: Optional. When set to `True`, the value in the `module` column
      is prepended to the value in the `name` column, joined by a period, in every row.

    # Columns of the returned DataFrame

    - **runID** *(string)*: Identifies the simulation run
    - **name** *(string)*: The name of the vector
    - **count**, **sumweights**, **mean**, **stddev**, **min**, **max** *(double)*: The characteristic mathematical properties of the statistics result
    - Additional metadata items (result attributes, run attributes, iteration variables, etc.), as requested
    """
    return impl.get_statistics(**locals())


def get_histograms(filter_expression="", include_attrs=False, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False, merge_module_and_name=False, include_statistics_fields=False):
    """
    Returns a filtered list of histogram results.

    # Parameters

    - **filter_expression** *(string)*: The filter expression to select the desired histogram.
      Example: `name =~ "collisionMultiplicity:histogram" AND itervar:iaMean =~ "2"`
    - **include_attrs** *(bool)*: Optional. When set to `True`, result attributes (like `unit`
      or `source` for example) are appended to the DataFrame, pivoted into columns.
    - **include_runattrs**, **include_itervars**, **include_param_assignments**, **include_config_entries** *(bool)*:
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the DataFrame, pivoted into columns.
    - **merge_module_and_name** *(bool)*: Optional. When set to `True`, the value in the `module` column
      is prepended to the value in the `name` column, joined by a period, in every row.

    # Columns of the returned DataFrame

    - **runID** *(string)*: Identifies the simulation run
    - **name** *(string)*: The name of the vector
    - **count**, **sumweights**, **mean**, **stddev**, **min**, **max** *(double)*: The characteristic mathematical properties of the histogram
    - **binedges**, **binvalues** *(np.array)*: The histogram edge locations and the weighted sum of the collected samples in each bin. `len(binedges) == len(binvalues) + 1`
    - **underflows**, **overflows** *(double)*: The weighted sum of the samples that fell outside of the histogram bin range in the two directions
    - Additional metadata items (result attributes, run attributes, iteration variables, etc.), as requested
    """
    return impl.get_histograms(**locals())


def get_config_entries(filter_expression, include_runattrs=False, include_itervars=False, include_param_assignments=False, include_config_entries=False):
    """
    Returns a filtered list of config entries. That is: parameter assignment patterns; and global and per-object config options.

    # Parameters

    - **filter_expression** *(string)*: The filter expression to select the desired config entries.
      Example: `name =~ sim-time-limit AND itervar:numHosts =~ 10`
    - **include_runattrs**, **include_itervars**, **include_param_assignments**, **include_config_entries** *(bool)*:
      Optional. When set to `True`, additional pieces of metadata about the run is appended to the result, pivoted into columns.

    # Columns of the returned DataFrame

    - **runID** *(string)*: Identifies the simulation run
    - **name** *(string)*: The name of the config entry
    - **value** *(string or double)*: The value of the config entry
    - Additional metadata items (run attributes, iteration variables, etc.), as requested
    """
    return impl.get_config_entries(**locals())