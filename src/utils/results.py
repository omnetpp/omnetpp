import io
import glob
import subprocess
import numpy as np
import pandas as pd

wd = "."
inputs = list()

def parse_if_number(s):
    try: return float(s)
    except: return True if s=="true" else False if s=="false" else s if s else None

def parse_ndarray(s):
    return np.fromstring(s, sep=' ') if s else None


def getVectors(filter_expression):
    # print("wd: " + wd + " inputs " + ", ".join(inputs))

    inputfiles = [glob.glob(wd + i) for i in inputs]

    # flattening the list of lists and making it unique
    inputfiles = list(set([item for sublist in inputfiles for item in sublist if item.endswith('.vec')]))
    # print(inputfiles)

    output = subprocess.check_output(["opp_scavetool", "x", *inputfiles, '-f', filter_expression, "-F", "CSV-R", "-o", "-"], cwd=wd)
    # print(output.decode('utf-8'))

    df = pd.read_csv(io.BytesIO(output), converters = {
        'attrvalue': parse_if_number,
        'binedges': parse_ndarray,
        'binvalues': parse_ndarray,
        'vectime': parse_ndarray,
        'vecvalue': parse_ndarray
    })
    # print(df)

    return df
