from omnetpp.scave import chart

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

def sanitize_and_compare_csv(df, ref_filename):
    df = sanitize(df)
    csv = df.to_csv(None)
    with open(ref_filename) as f:
        if csv == str(f.read()):
            return True
        else:
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
    
    chart.set_property("Graph.Title", "See console for test results.")
    
    faileds = ", ".join([n for n, s in outcomes.items() if not s])
    import sys
    print("FAILED TESTS: " + (faileds or "NONE"), file=sys.stderr)
