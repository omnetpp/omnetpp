from omnetpp.scave import results, chart, plot
import numpy as np
import pandas as pd
import re
from distutils.util import strtobool

params = chart.get_properties()

filter_expression = params["filter"]

def module_name_runattr_from_pattern(pattern):
    module, name, runattr = None, None, None

    mm = re.search(r"module\((.*?)\)", pattern)
    if mm:
        module = mm.group(1)

    nm = re.search(r"name\((.*?)\)", pattern)
    if nm:
        name = nm.group(1)

    if pattern.endswith("(*)"):
        runattr = pattern[:-3]

    # ??? is this always reasonable? When the runattrs are recorded in the file as scalars?
    if module == '_runattrs_' or module == '.':
        runattr = name
        module = None
        name = None
    if runattr and runattr.startswith("attr:"):
        runattr = runattr[5:]

    return module, name, runattr

x_pattern = params["x_pattern"]
x_module, x_name, x_runattr = module_name_runattr_from_pattern(x_pattern) 

iso_pattern = params["iso_patterns"].split(";")[0]
iso_module, iso_name, iso_runattr = module_name_runattr_from_pattern(iso_pattern)


print(x_module, x_name, x_runattr)
print(iso_module, iso_name, iso_runattr)

avg_repls = bool(strtobool(params['average_replications']))

sc = results.get_scalars(filter_expression, include_itervars=True, include_runattrs=True)
iv = results.get_itervars("(" + filter_expression + ") AND NOT name(" + x_runattr + ")", include_itervars=True, include_runattrs=True)
iv['module'] = ""

df = pd.concat([sc, iv])
df['value'] = pd.to_numeric(df['value'])


if x_runattr and iso_runattr:
    print(df)
    df = pd.pivot_table(df, index=[x_runattr], columns=[iso_runattr, "replication"], values="value")
    df.reset_index(inplace=True)
    print(df)
    df.rename({x_runattr:'time'}, axis="columns", inplace=True) # TODO: ugly
    df.rename({0:'time'}, axis="columns", inplace=True)
elif iso_runattr and x_module and x_name:
    cols = ["experiment", "measurement"] if avg_repls else ["runID", "experiment", "measurement", "replication"]

    df = pd.pivot_table(df, columns=["module", "name"], index=cols, values="value")
    print(df)
    lbl = ("", iso_runattr)
    # where to put iso and x? support multiple isos!
    df = pd.pivot_table(df, columns=[lbl], index=cols + [(x_module, x_name)])

    # TODO: rest is ugly
    df.reset_index(level=2 if avg_repls else 4, inplace=True)
    df.columns = ["time"] + list(df.columns.get_level_values(2).values)[1:]
    df.rename({0:'time'}, axis="columns", inplace=True)

# TODO: make this optional
df = df.sort_values("time")

lbls = []

for i, c in df.iteritems():
    tuples = list(zip(df.columns.names, c.name))
    lbls.append(", ".join([ "=".join(t) for t in tuples]))

df.columns = ["time"] + lbls[1:]
print(df)

plot.plot_vectors(df)

plot.set_properties(chart.get_properties())
