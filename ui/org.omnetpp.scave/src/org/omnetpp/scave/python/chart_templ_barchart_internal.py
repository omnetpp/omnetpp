from omnetpp.scave import results, chart
import pandas as pd

params = chart.get_properties()

filter_expression = params["filter"]

groups = params["groups"]
bars = params["bars"]

if not groups:
    groups = "module,experiment"

if not bars:
    bars = "name,measurement"

# TODO: make sure no column is present in both lists 

# The data is returned as a Pandas DataFrame
df = results.get_scalars(filter_expression, include_attrs=True, include_itervars=True, include_runattrs=True)
print(df)

# You can perform any transformations on the data here
df = pd.pivot_table(df, values="value", index=groups.split(","), columns=bars.split(","))

# You can perform any transformations on the data here

# Finally, the results are plotted
chart.plot_scalars(df)