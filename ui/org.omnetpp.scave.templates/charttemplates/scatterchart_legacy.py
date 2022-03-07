from omnetpp.scave import results, chart, utils, ideplot
import numpy as np
import pandas as pd
import re
from distutils.util import strtobool

props = chart.get_properties()
utils.preconfigure_plot(props)

filter_expression = props["filter"]

def module_name_runattr_from_pattern(pattern):
    module, name, runattr = None, None, None

    def get_field_from_pattern(fieldname):
        match = re.search(fr'{fieldname} =~ (["\w_]+)', pattern)
        if match:
            fieldpattern = match.group(1)

            if fieldpattern.startswith('"') and fieldpattern.endswith('"'):
                return fieldpattern[1:-1]
            else:
                return fieldpattern
        else:
            return None

    module = get_field_from_pattern("module")
    name = get_field_from_pattern("name")

    if pattern.endswith(" =~ *"):
        runattr = pattern[:-5]
    if pattern.endswith(" =~ \"*\""):
        runattr = pattern[:-7]

    # ??? is this always reasonable? When the runattrs are recorded in the file as scalars?
    if module == '_runattrs_' or module == '.':
        runattr = name
        module = None
        name = None

    if runattr and runattr.startswith("attr:"):
        runattr = runattr[5:]

    return module, name, runattr

x_pattern = props["x_pattern"]
x_module, x_name, x_runattr = module_name_runattr_from_pattern(x_pattern)

iso_pattern = props["iso_patterns"].split(";")[0]
iso_module, iso_name, iso_runattr = module_name_runattr_from_pattern(iso_pattern)

avg_repls = bool(strtobool(props['average_replications']))
try:
    sc = results.get_scalars(filter_expression, include_fields=True, include_itervars=True, include_runattrs=True)
    iv = results.get_itervars("(" + filter_expression + ") AND NOT name =~ " + x_runattr, include_itervars=True, include_runattrs=True)
except results.ResultQueryError as e:
    raise chart.ChartScriptError("Error while querying results: " + str(e))
iv['module'] = ""

df = pd.concat([sc]) # , iv ?

if df.empty:
    raise chart.ChartScriptError("The result filter returned no data.")

utils.assert_columns_exist(df, ["value"])
df['value'] = pd.to_numeric(df['value'])

if x_runattr and iso_runattr:
    utils.assert_columns_exist(df, [x_runattr, iso_runattr])
    df = pd.pivot_table(df, index=[x_runattr], columns=[iso_runattr], values="value")
    df.reset_index(inplace=True)
    x_column = x_runattr
elif iso_runattr and x_module and x_name:
    utils.assert_columns_exist(df, [iso_runattr, x_module, x_name])
    cols = ["experiment", "measurement"] if avg_repls else ["runID", "experiment", "measurement", "replication"]

    df = pd.pivot_table(df, columns=["module", "name"], index=cols, values="value")
    lbl = ("", iso_runattr)
    # where to put iso and x? support multiple isos!
    df = pd.pivot_table(df, columns=[lbl], index=cols + [(x_module, x_name)])

    x_column = x_module + ":" + x_name

    # TODO: rest is ugly
    df.reset_index(level=2 if avg_repls else 4, inplace=True)
    df.columns = [x_column] + list(df.columns.get_level_values(2).values)[1:]

for i, c in enumerate(df.columns):
    if c != x_column:
        label = ", ".join([ "=".join(t) for t in zip(df.columns.names, c)])
        style = utils._make_line_args(props, c, df)
        ideplot.plot(df[x_column], df[c].values, label=label, **style)

# utils.set_plot_title(scalar_name + " vs. " + xaxis_itervar)

utils.postconfigure_plot(props)

utils.export_image_if_needed(props)
utils.export_data_if_needed(df, props)
