from omnetpp.scave import results, chart, vectorops as ops

params = chart.get_properties()

# This expression selects the results (you might be able to logically simplify it)

filter_expression = params["filter"]

# The data is returned as a Pandas DataFrame
df = results.get_vectors(filter_expression, include_attrs=True, include_itervars=True)

# This is where the vector operations will be added:
# <|> vectorops marker <|>

# You can perform any transformations on the data here

# TODO: factor this out to the vectorops module? also put the marker after it.

import re
def convert(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    lower = str(re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower())

    try:
        return int(lower)
    except:
        pass

    try:
        return float(lower)
    except:
        pass

    return lower

operations = params["vector_operations"]

if operations:
    for op in operations.split(';'):
        type, oper = op.split(':')
        fun = oper.split(",")

        params = dict()
        for p in fun[1:]:
            key, value = p.split("=")
            params[convert(key)] = convert(value)

        op_fun = ops.__dict__["vector_" + fun[0]]

        if type == "apply":
            df = ops.apply(df, op_fun, **params)
        elif type == "compute":
            df = ops.compute(df, op_fun, **params)

print(df)

# Finally, the results are plotted
chart.plot_vectors(df)