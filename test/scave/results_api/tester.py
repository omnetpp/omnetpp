from omnetpp.scave import chart, ideplot
import os
import sys
from csv import QUOTE_NONNUMERIC
import difflib
import numpy as np

def sanitize_row(row):
    if "attrname" in row and row["attrname"] in ["datetime", "datetimef", "processid"]:
        row["attrvalue"] = "REDACTED"
    if "name" in row and row["name"] in ["datetime", "datetimef", "processid"]:
        row["value"] = "REDACTED"
    row["runID"] = "-".join(row["runID"].split("-")[:2]) + "-REDACTED"
    if "processid" in row:
        row["processid"] = "REDACTED"
    if "datetime" in row:
        row["datetime"] = "REDACTED"
    if "datetimef" in row:
        row["datetimef"] = "REDACTED"
    return row

def sanitize(df):
    df = df.apply(sanitize_row, axis=1)
    df = df.sort_values(axis=0, by=list(filter(lambda c: c in df, ["runID", "module", "type", "name", "attrname"])))
    df = df.reset_index(drop=True)
    df = df.round(6)
    return df


def compressOneDiff(tmpDiff, maxLines):
    if len(tmpDiff) <= 2 * maxLines + 3:
        return tmpDiff
    pre = tmpDiff[0][0]
    return tmpDiff[:maxLines] \
            + [ pre + ".\n", pre + ". <snip>\n", pre + ".\n"] \
            + tmpDiff[-maxLines:]

def compressDiff(diff):
    maxLines = 3
    tmpDiff = []
    prevStart = ''
    for line in diff:
        if prevStart == line[0:2]:
            tmpDiff.append(line)
        else:
            for l in compressOneDiff(tmpDiff, maxLines):
                yield l
            prevStart = line[0:2]
            tmpDiff = [line]

    for l in compressOneDiff(tmpDiff, maxLines):
        yield l

def colorDiff(diff):
    for line in diff:
        if line.startswith('+ '):
            yield '\033[0;32m' + line + "\033[0;0m"     # GREEN
        elif line.startswith('+.'):
            yield "\033[2;32m" + line + "\033[0;0m"     # dark green
        elif line.startswith('- '):
            yield "\033[1;31m" + line + "\033[0;0m"     # RED
        elif line.startswith('^ '):
            yield "\033[1;34m" + line + "\033[0;0m"     # BLUE
        elif line.startswith('? '):
            yield "\033[1;33m" + line + "\033[0;0m"     # YELLOW
        elif line.startswith(' .'):
            yield "\033[2;37m" + line + "\033[0;0m"     # dark white
        else:
            yield line

def green(text):
    return "\033[0;32m" + text + "\033[0;0m"

def red(text):
    return "\033[1;31m" + text + "\033[0;0m"

def addMissingNewLine(str):
    return str if str.endswith("\n") else str + "\n"

def sanitize_and_compare_csv(df, ref_filename):
    df = sanitize(df)
    actual = str([str(n) + ": " + str(t) for n, t in zip(df.columns.values, df.dtypes.values)]) + "\n"
    actual += df.to_csv(None, index=False, quoting=QUOTE_NONNUMERIC, line_terminator="\n")

    os.makedirs("actual_output", exist_ok=True)
    with open("actual_output/" + ref_filename, "wt") as f2:
        f2.write(actual)

    with open("expected_output/" + ref_filename) as f:

        expected = str(f.read())
        if actual == expected:
            return True
        else:
            #print("expected: " + expected + " actual: " + actual)

            diff = difflib.ndiff(addMissingNewLine(expected).splitlines(True), addMissingNewLine(actual).splitlines(True))
            diff = compressDiff(diff)
            diff = colorDiff(diff)
            difftxt = ''.join(diff)

            print(difftxt)

            return False

    return False

def _assert(condition, message):
    if not condition:
        raise Exception("Assertion failed on: " + message)

def _assert_sequential_index(df):
    _assert((df.index.values == np.arange(len(df))).all(), "DataFrame index should be sequential integers, starting from 0")

def run_tests(locals):
    outcomes = dict()
    def add_outcome(name, successful):
        if name in outcomes:
            raise RuntimeError("Duplicate test name: " + name)
        outcomes[name] = successful

    loc = list(locals.keys())
    for l in loc:
        if l.startswith("test_"):
            print(l, end=": ")
            f = locals[l]
            try:
                o = f()
            except Exception as e:
                print("EXCEPTION: " + str(e))
                import traceback
                traceback.print_exc()
                o = False
            passed = (o is None) or o # "void" methods are successful if they don't raise an Exception
            print(green("PASS") if passed else red("FAIL"))
            add_outcome(l, passed)

    ideplot.set_property("Plot.Title", "See console for test results.")

    failed_names = [n for n, s in outcomes.items() if not s]
    faileds = ", ".join(failed_names)

    if faileds:
        print("FAILED TESTS: " + faileds, file=sys.stderr)

    if faileds:
        raise Exception(str(len(failed_names)) + " tests failed")
