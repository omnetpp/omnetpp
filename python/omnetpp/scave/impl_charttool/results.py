import os
import io
import glob
import subprocess
from math import inf
import numpy as np
import pandas as pd

"""
This module implements the same result querying API that is provided by the IDE to chart scripts,
using the opp_scavetool program to load the .sca and .vec files.
"""

# TODO: document
inputfiles = list()


def add_inputs(input_patterns, workspace_dir, project_paths):
    global inputfiles
    ins = list()
    for i in input_patterns:
        mapped = False
        for p in project_paths.keys():
            pv = project_paths[p]

            if not p.startswith("/"):
                p = "/" + p

            if i.startswith(p):
                pattern = pv + "/" + i[len(p):]

                if os.path.isdir(pattern):
                    pattern += "/**"

                ins.extend(glob.glob(pattern, recursive=True))
                mapped = True
                break

        # this is a sort-of default case
        if not mapped:
            ins.extend(glob.glob(workspace_dir + "/" + i))

    # turning them into absolute paths (script will cwd) and making it unique
    inputfiles = list(set(inputfiles + [os.path.abspath(item) for item in ins]))

def _parse_int(s):
    return int(s) if s else None

def _parse_float(s):
    return float(s) if s else None

def _parse_if_number(s):
    try: return int(s)
    except:
        try: return float(s)
        except: return True if s=="true" else False if s=="false" else s if s else None

def _parse_ndarray(s):
    return np.fromstring(s, sep=' ') if s else None


def get_serial():
    # return an (arbitrary) constant, as the set of loaded results doesn't change during a run of opp_charttool.
    return 1

def _get_results(filter_expression, file_extensions, result_type, *additional_args):

    filelist = [i for i in inputfiles if any([i.endswith(e) for e in file_extensions])]
    type_filter = ['-T', result_type] if result_type else []
    filter_expr_args = ['-f', filter_expression]
    command = ["opp_scavetool", "x", *filelist, *type_filter, *filter_expr_args,
                "-F", "CSV-R", "-o", "-", *additional_args]

    proc = subprocess.run(command, stderr=subprocess.PIPE, stdout=subprocess.PIPE, universal_newlines=False)
    output_bytes = proc.stdout

    if proc.returncode != 0:
        raise RuntimeError(proc.stderr.decode("utf-8").strip() + " (exit code " + str(proc.returncode) + ")")

    # TODO: stream the output through subprocess.PIPE ?
    df = pd.read_csv(io.BytesIO(output_bytes), converters = {
        'value': _parse_if_number,
        #'attrvalue': _parse_if_number, # should be optional, and done later
        'count': _parse_int,
        'min': _parse_float,
        'max': _parse_float,
        'mean': _parse_float,
        'stddev': _parse_float,
        'sumweights': _parse_float,
        'underflows': _parse_float,
        'overflows': _parse_float,
        'binedges': _parse_ndarray,
        'binvalues': _parse_ndarray,
        'vectime': _parse_ndarray,
        'vecvalue': _parse_ndarray
    })


    df.rename(columns={"run": "runID"}, inplace=True) # oh, inconsistencies...

    # TODO: convert column dtype as well?

    return df

def _split_by_types(df, types):
    result = list()
    for t in types:
        mask = df['type'] == t
        result.append(df[mask])
        df = df[~mask]
    result.append(df)
    return result

def _append_metadata_columns(df, meta, suffix):
    meta = pd.pivot_table(meta, index="runID", columns="attrname", values="attrvalue", aggfunc="first")

    if not meta.empty:
        df = df.join(meta, on="runID", rsuffix=suffix)

    return df

def _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name):
    # TODO add params
    itervars, runattrs, configs, attrs, df = _split_by_types(df, ["itervar", "runattr", "config", "attr"])


    if include_attrs and attrs is not None and not attrs.empty:
        attrs = pd.pivot_table(attrs, columns="attrname", aggfunc='first', index=["runID", "module", "name"], values="attrvalue")
        df = df.merge(attrs, left_on=["runID", "module", "name"], right_index=True, how='left')

    if include_itervars:
        df = _append_metadata_columns(df, itervars, "_itervar")
    if include_runattrs:
        df = _append_metadata_columns(df, runattrs, "_runattr")
    if include_config_entries:
        df = _append_metadata_columns(df, configs, "_config")
    # TODO maybe only params, based on include_ args

    #TODO df.join(params, on="run", rsuffix="_param")

    df.drop(['type', 'attrname', 'attrvalue'], axis=1, inplace=True)

    if merge_module_and_name:
        df["name"] = df["module"] + "." + df["name"]

    return df

def get_results(filter_expression, row_types, omit_unused_columns, start_time, end_time):
    df = _get_results(filter_expression, ['.sca', '.vec'], None)
    df = df[df["type"].isin(row_types)]
    return df

def get_scalars(filter_expression, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name):
    # TODO filter row types based on include_ args, as optimization
    df = _get_results(filter_expression, ['.sca'], 's')
    df = _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name)
    return df

def get_vectors(filter_expression, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name, start_time, end_time):
    df = _get_results(filter_expression, ['.vec'], 'v', '--start-time', str(start_time), '--end-time', str(end_time))
    df = _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name)
    return df

def get_statistics(filter_expression, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name):
    df = _get_results(filter_expression, ['.sca'], 't')
    df = _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name)
    return df

def get_histograms(filter_expression, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name, include_statistics_fields):
    df = _get_results(filter_expression, ['.sca'], 'h')
    df = _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name)
    return df

def get_parameters(filter_expression, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name, as_numeric):
    df = _get_results(filter_expression, ['.sca'], 'p')
    df = _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries, merge_module_and_name)
    if as_numeric:
        df["value"] = pd.to_numeric(df["value"], errors="ignore")
    return df

def _get_metadata(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    # TODO: factor out common parts of the ones below here
    pass

def get_runs(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    command = ["opp_scavetool", "q", *inputfiles, "-r", '-f',
                filter_expression, "-g"]

    output = subprocess.check_output(command)

    if len(output.decode("utf-8").splitlines()) == 0:
        print("<!> HINT: opp_scavetool returned an empty result. Consider adding a project name to directory mapping, for example: -p /aloha=../aloha")

    # with open("output.csv", "tw") as outp:
    #     outp.write(str(output))

    # TODO: stream the output through subprocess.PIPE ?
    df = pd.read_csv(io.BytesIO(output),  header=None, names=["runID"])

    # TODO: convert column dtype as well?

    if include_itervars:
        iv = get_itervars("*", False, False, False, False, True)
        iv.rename(columns={"name": "attrname", "value" : "attrvalue"}, inplace=True) # oh, inconsistencies...
        df = _append_metadata_columns(df, iv, "_itervar")

    if include_runattrs:
        ra = get_runattrs("*", False, False, False, False)
        ra.rename(columns={"name": "attrname", "value": "attrvalue"}, inplace=True) # oh, inconsistencies...
        df = _append_metadata_columns(df,ra, "_runattr")

    if include_config_entries:
        ce = get_config_entries("*", False, False, False, False)
        ce.rename(columns={"name": "attrname", "value" : "attrvalue"}, inplace=True) # oh, inconsistencies...
        df = _append_metadata_columns(df, ce, "_config")

    # TODO maybe only params, based on include_ args
    #TODO df.join(params, on="run", rsuffix="_param")

    return df

def get_runattrs(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    command = ["opp_scavetool", "q", *inputfiles, "-a", "-g", "--tabs"]

    output = subprocess.check_output(command)

    if len(output.decode('utf-8').splitlines()) == 1:
        print("<!> HINT: opp_scavetool returned an empty result. Consider adding a project name to directory mapping, for example: -p /aloha=../aloha")

    # TODO: stream the output through subprocess.PIPE ?
    df = pd.read_csv(io.BytesIO(output), sep='\t', header=None, names=["runID", "name", "value"])

    if include_itervars:
        iv = get_itervars("*", False, False, False, False, True)
        iv.rename(columns={"name": "attrname", "value" : "attrvalue"}, inplace=True) # oh, inconsistencies...
        df = _append_metadata_columns(df, iv, "_itervar")

    if include_runattrs:
        ra = get_runattrs("*", False, False, False, False)
        ra.rename(columns={"name": "attrname", "value" : "attrvalue"}, inplace=True) # oh, inconsistencies...
        df = _append_metadata_columns(df, ra, "_runattr")

    if include_config_entries:
        ce = get_config_entries("*", False, False, False, False)
        ce.rename(columns={"name": "attrname", "value" : "attrvalue"}, inplace=True) # oh, inconsistencies...
        df = _append_metadata_columns(df, ce, "_config")

    return df


def get_itervars(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries, as_numeric):
    command = ["opp_scavetool", "q", *inputfiles, "-i", "-g", "--tabs"]

    output = subprocess.check_output(command)

    if len(output.decode('utf-8').splitlines()) == 1:
        print("<!> HINT: opp_scavetool returned an empty result. Consider adding a project name to directory mapping, for example: -p /aloha=../aloha")

    # TODO: stream the output through subprocess.PIPE ?
    df = pd.read_csv(io.BytesIO(output), sep='\t', header=None, names=["runID", "name", "value"])

    if include_itervars:
        iv = get_itervars("*", False, False, False, False, as_numeric)
        iv.rename(columns={"name": "attrname", "value" : "attrvalue"}, inplace=True) # oh, inconsistencies...
        df = _append_metadata_columns(df, iv, "_itervar")

    if include_runattrs:
        ra = get_runattrs("*", False, False, False, False)
        ra.rename(columns={"name": "attrname", "value" : "attrvalue"}, inplace=True) # oh, inconsistencies...
        df = _append_metadata_columns(df, ra, "_runattr")

    if include_config_entries:
        ce = get_config_entries("*", False, False, False, False)
        ce.rename(columns={"name": "attrname", "value" : "attrvalue"}, inplace=True) # oh, inconsistencies...
        df = _append_metadata_columns(df, ce, "_config")

    return df

def get_config_entries(filter_expression, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    command = ["opp_scavetool", "q", *inputfiles, "-j", "-g", "--tabs"]

    output = subprocess.check_output(command)

    if len(output.decode('utf-8').splitlines()) == 1:
        print("<!> HINT: opp_scavetool returned an empty result. Consider adding a project name to directory mapping, for example: -p /aloha=../aloha")

    # TODO: stream the output through subprocess.PIPE ?
    df = pd.read_csv(io.BytesIO(output), sep='\t', header=None, names=["runID", "name", "value"])

    if include_itervars:
        iv = get_itervars("*", False, False, False, False, True)
        iv.rename(columns={"name": "attrname", "value" : "attrvalue"}, inplace=True) # oh, inconsistencies...
        df = _append_metadata_columns(df, iv, "_itervar")

    if include_runattrs:
        ra = get_runattrs("*", False, False, False)
        ra.rename(columns={"name": "attrname", "value" : "attrvalue"}, inplace=True) # oh, inconsistencies...
        df = _append_metadata_columns(df, ra, "_runattr")

    if include_config_entries:
        ce = get_config_entries("*", False, False, False, False)
        ce.rename(columns={"name": "attrname", "value" : "attrvalue"}, inplace=True) # oh, inconsistencies...
        df = _append_metadata_columns(df, ce, "_config")

    return df
