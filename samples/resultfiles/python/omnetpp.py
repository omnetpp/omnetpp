import shlex
import pandas as pd
import numpy as np

# crude performance test:
#
# import timeit
# exec(open('../python/read_omnetpp.py').read())
# t = timeit.Timer(lambda: read_omnetpp('PureAlohaExperiment-numHosts=10,mean=1-#0.sca'))
# t.timeit(number=1000)

class Context:
    # constants for 'type'
    RUN = 1
    SCALAR = 2
    VECTOR = 3
    STATISTICS = 4
    HISTOGRAM = 5

    type = None
    run_id = None
    module_name = None
    result_name = None
    scalar_value = None
    vector_id = None
    vector_columns = None
    attrs = {}
    itervars = {}
    params = {}
    fields = {}
    binedges = []
    binvalues = []

    veccols = {}  # maps vector id to either "TV" or "ETV"

    # these map vector ids to numpy arrays
    vecevents = {}
    vectimes = {}
    vecvalues = {}


def read_omnetpp(filename):
    # Performance notes:
    #  (1) most CPU cycles are burnt in splitting the line to tokens (you can verify this by strategically placing 'continue' statements below)
    #  (2) creating an empty DataFrame (such as at the end of this method, with records=[]) takes surprisingly long time
    #
    records = []
    ctx = Context()
    with open(filename) as f:
        for line in f:
            line = line.lstrip()
            # continue (uncomment for profiling)
            if len(line) == 0 or line[0] == '#' or line.isspace():
                continue

            # tokenize the line. Note: this line is THE performance bottleneck.
            # shlex.split() is extra slow, but even String.split() doesn't cut it
            tokens = line.split() if line.find('"') == -1 else shlex.split(line)
            # continue # (uncomment for profiling)

            # process the line
            type = tokens[0]

            # the order of the checks is important for performance
            if type.isdigit():
                # looks like a vector data line
                vector_id = tokens[0]

                if ctx.veccols[vector_id] == "TV":
                    ctx.vectimes[vector_id].append(tokens[1])
                    ctx.vecvalues[vector_id].append(tokens[2])
                else:  # ETV
                    # ctx.vecevents[vector_id].append(tokens[1])
                    ctx.vectimes[vector_id].append(tokens[2])
                    ctx.vecvalues[vector_id].append(tokens[3])

            elif type == 'attr':
                # syntax: "attr <name> <value>"
                assert ctx.type != None, "stray 'attr' line"
                assert len(tokens) == 3, "incorrect 'attr' line -- attr <name> <value> expected"
                ctx.attrs[tokens[1]] = tokens[2]

            elif type == 'scalar':
                flush_context(ctx, records)

                # syntax: "scalar <module> <scalarname> <value>"
                assert ctx.type != None, "stray 'scalar' line, must be under a 'run'"
                assert len(tokens) == 4, "incorrect 'scalar' line -- scalar <module> <scalarname> <value> expected"
                ctx.type = Context.SCALAR
                ctx.module_name = tokens[1]
                ctx.result_name = tokens[2]
                ctx.scalar_value = tokens[3]

            elif type == 'run':
                # flush last result item in previous run
                flush_context(ctx, records)

                # "run" line, format: run <runName>
                assert len(tokens) == 2, "incorrect 'run' line -- run <runID> expected"
                ctx.type = Context.RUN
                ctx.run = tokens[1]

            elif type == 'vector':
                flush_context(ctx, records)

                # syntax: "vector <id> <module> <vectorname> [<columns>]"
                assert ctx.type != None, "stray 'vector' line, must be under a 'run'"
                assert len(tokens) == 4 or len(tokens) == 5, "incorrect 'vector' line -- vector <id> <module> <vectorname> [<columns>] expected"
                ctx.type = Context.VECTOR
                ctx.vector_id = tokens[1]
                ctx.module_name = tokens[2]
                ctx.result_name = tokens[3]
                ctx.vector_columns = "TV" if (len(tokens) < 5 or tokens[4].isnumeric()) else tokens[4]

                if ctx.vector_columns != "TV" and ctx.vector_columns != "ETV":
                    raise RuntimeError("unsupported vector column specification: '" + ctx.vector_columns + "' -- only 'TV' and 'ETV' are implemented")

                ctx.veccols[ctx.vector_id] = ctx.vector_columns
                ctx.vecevents[ctx.vector_id] = []
                ctx.vectimes[ctx.vector_id] = []
                ctx.vecvalues[ctx.vector_id] = []

            elif type == 'statistic':
                flush_context(ctx, records)

                # syntax: "statistic <module> <statisticname>"
                assert ctx.type != None, "stray 'statistic' line, must be under a 'run'"
                assert len(tokens) == 3, "incorrect 'statistic' line -- statistic <module> <statisticname> expected"
                ctx.type = Context.STATISTICS
                ctx.module_name = tokens[1]
                ctx.result_name = tokens[2]

            elif type == 'field':
                # syntax: "field <name> <value>"
                assert ctx.type == Context.STATISTICS, "stray 'field' line, must be under a 'statistic'"
                assert len(tokens) == 3, "incorrect 'field' line -- field <name> <value> expected"
                assert tokens[1] in ['count', 'mean', 'sumweights', 'min', 'max', 'stddev', 'sum', 'sqrsum'], "field name '" + tokens[1] + "' not accepted" # TODO weighted fields
                ctx.fields[tokens[1]] = float(tokens[2])

            elif type == 'bin':
                if ctx.type == Context.STATISTICS:
                    ctx.type = Context.HISTOGRAM
                if ctx.type != Context.HISTOGRAM:
                    raise RuntimeError("stray 'bin' line, must be under a 'histogram'")
                assert len(tokens) == 3, "incorrect 'bin' line -- bin <lower_edge> <value> expected"
                ctx.binedges.append(tokens[1])
                ctx.binvalues.append(tokens[2])

            elif type == 'param':
                # syntax: "param <namePattern> <value>"
                assert ctx.type == Context.RUN, "stray 'param' line, must be under a 'run' line"
                assert len(tokens) == 3, "incorrect 'param' line -- param <namePattern> <value> expected"
                ctx.params[tokens[1]] = tokens[2]

            elif type == 'itervar':
                # syntax: "itervar <name> <value>"
                assert ctx.type == Context.RUN, "stray 'itervar' line, must be under a 'run' line"
                assert len(tokens) == 3, "incorrect 'itervar' line -- itervar <name> <value> expected"
                ctx.itervars[tokens[1]] = tokens[2]

            elif type == 'version':
                assert len(tokens) == 2, "incorrect 'version' line -- version <number> expected"
                assert tokens[1] == '2', "expects version 2 expected"

            else:
                raise RuntimeError('unrecognized line type: ' + type)

    dataframe = pd.DataFrame(data=records, columns=['run', 'type', 'module', 'name', 'attrname', 'value',
                                                    'count', 'sumweights', 'mean', 'stddev', 'min', 'max',
                                                    'vectime', 'vecvalue', 'binedges', 'binvalues'])

    dataframe = dataframe.dropna(axis=1, how='all')
    return dataframe

def flush_context(ctx, records):
    if ctx.type == Context.RUN:
        for key, value in ctx.attrs.items():
            records.append({'run': ctx.run, 'type': 'runattr', 'attrname': key, 'value': value})
        for key, value in ctx.itervars.items():
            records.append({'run': ctx.run, 'type': 'itervar', 'attrname': key, 'value': value})
        for key, value in ctx.params.items():
            records.append({'run': ctx.run, 'type': 'param', 'attrname': key, 'value': value})
    if ctx.type == Context.SCALAR:
        records.append({'run': ctx.run, 'type': 'scalar', 'module': ctx.module_name, 'name': ctx.result_name, 'value': ctx.scalar_value})
        for key, value in ctx.attrs.items():
            records.append({'run': ctx.run, 'type': 'attr', 'module': ctx.module_name, 'name': ctx.result_name, 'attrname': key, 'value': value})
    if ctx.type == Context.VECTOR:
        records.append({'run': ctx.run, 'type': 'vector', 'module': ctx.module_name, 'name': ctx.result_name, 'vectime': ctx.vectimes[ctx.vector_id], 'vecvalue': ctx.vecvalues[ctx.vector_id]})
        for key, value in ctx.attrs.items():
            records.append({'run': ctx.run, 'type': 'attr', 'module': ctx.module_name, 'name': ctx.result_name, 'attrname': key, 'value': value})
    if ctx.type == Context.STATISTICS:
        records.append({'run': ctx.run, 'type': 'statistic', 'module': ctx.module_name, 'name': ctx.result_name, **ctx.fields})
        for key, value in ctx.attrs.items():
            records.append({'run': ctx.run, 'type': 'attr', 'module': ctx.module_name, 'name': ctx.result_name, 'attrname': key, 'value': value})
    if ctx.type == Context.HISTOGRAM:
        records.append({'run': ctx.run, 'type': 'histogram', 'module': ctx.module_name, 'name': ctx.result_name, **ctx.fields, 'binedges': np.array(ctx.binedges), 'binvalues': np.array(ctx.binvalues)})
        for key, value in ctx.attrs.items():
            records.append({'run': ctx.run, 'type': 'attr', 'module': ctx.module_name, 'name': ctx.result_name, 'attrname': key, 'value': value})
    ctx.attrs = {}
    ctx.itervars = {}
    ctx.params = {}
    ctx.fields = {}
    ctx.binedges = []
    ctx.binvalues = []

