from omnetpp.scave import chart, plot
import difflib

def sanitize_row(row):
    if "attrname" in row and row["attrname"] in ["datetime", "processid"]:
        row["attrvalue"] = "REDACTED"
    if "name" in row and row["name"] in ["datetime", "processid"]:
        row["value"] = "REDACTED"
    row["runID"] = "-".join(row["runID"].split("-")[:2]) + "-REDACTED"
    if "processid" in row:
        row["processid"] = "REDACTED"
    if "datetime" in row:
        row["datetime"] = "REDACTED"
    return row

def sanitize(df):
    df = df.apply(sanitize_row, axis=1)
    df = df.sort_values(axis=0, by=list(filter(lambda c: c in df, ["runID", "module", "type", "name", "attrname"])))
    df = df.reset_index(drop=True)
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


def addMissingNewLine(str):
    return str if str.endswith("\n") else str + "\n"

def sanitize_and_compare_csv(df, ref_filename):
    df = sanitize(df)
    csv = df.to_csv(None)
    with open(ref_filename) as f:
        actual = csv
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


            with open(ref_filename + ".out", "wt") as f2:
                f2.write(csv)
            return False

def _assert(condition, message):
    if not condition:
        raise Exception("Assertion failed on: " + message)

def run_tests(locals):
    outcomes = dict()
    def add_outcome(name, successful):
        if name in outcomes:
            raise RuntimeError("Duplicate test name: " + name)
        outcomes[name] = successful

    loc = list(locals.keys())
    for l in loc:
        if l.startswith("test_"):
            print("---- running: " + l + " ----")
            f = locals[l]
            try:
                o = f()
            except Exception as e:
                print("EXCEPTION: " + str(e))
                o = False
            passed = (o is None) or o # "void" methods are successful if they don't raise an Exception
            print("PASS" if passed else "FAIL")
            add_outcome(l, passed)

    plot.set_property("Graph.Title", "See console for test results.")

    failed_names = [n for n, s in outcomes.items() if not s]
    faileds = ", ".join(failed_names)
    import sys
    print("FAILED TESTS: " + (faileds or "NONE"), file=sys.stderr)

    if faileds:
        raise Exception(str(len(failed_names)) + " tests failed")
