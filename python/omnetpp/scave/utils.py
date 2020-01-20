

def extract_label_columns(df):

    """
    Utility function to make a reasonable guess as to which column of
    the given DataFrame is most suitable to act as a chart title and
    which ones can be used as legend labels.

    Ideally a "title column" should be one in which all lines have the same
    value, and can be reasonably used as a title. Some often used candidates
    are: `title`, `name`, and `module`.

    Label columns should be a minimal set of columns whose corresponding
    value tuples uniquely identify every line in the DataFrame. These will
    primarily be iteration variables and run attributes.

    # Returns:

    A pair of a string and a list; the first value is the name of the
    "title" column, and the second one is a list of pairs, each
    containing the index and the name of a "label" column.

    Example: `('title', [(8, 'numHosts'), (7, 'iaMean')])`
    """

    titles = ["title", "name", "module", "experiment", "measurement", "replication"]
    legends = ["title", "name", "module", "experiment", "measurement", "replication"]

    blacklist = ["runID", "value", "attrvalue", "vectime", "vecvalue",
                 "binedges", "binvalues", "underflows", "overflows",
                 "count", "sumweights", "mean", "stddev", "min", "max",
                 "processid", "iterationvars", "iterationvarsf", "datetime",
                 "source", "interpolationmode", "enum", "title", 'runnumber', 'seedset']

    title_col = None

    for title in titles:
        if title in df and len(df[title].unique()) == 1:
            title_col = title
            break
    if title_col == None:
        if "name" in titles:
            title_col = "name"

    legend_cols = []

    for legend in legends:
        if legend in df and len(df[legend].unique()) == len(df):
            legend_cols = [(list(df.columns.values).index(legend), legend)]
            break

    if legend_cols:
        return title_col, legend_cols

    last_len = None
    for i, col in list(enumerate(df)):
        if col not in blacklist and col != title_col:
            new_len = len(df.groupby([i2 for i1, i2 in legend_cols] + [col]))
            if new_len == len(df) or not last_len or new_len > last_len:
                legend_cols.append((i, col))
                last_len = new_len
            if new_len == len(df):
                break

    # filter out columns which only have a single value in them (this can happen in the loop above, with the first considered column)
    legend_cols = list(filter(lambda icol: len(df[icol[1]].unique()) > 1, legend_cols))

    """
    if not legend_cols:
        last_len = None
        for i, col in reversed(list(enumerate(df))):
            if col not in blacklist and col != title_col:
                new_len = len(df.groupby([i2 for i1, i2 in legend_cols] + [col]))
                if new_len > 1:
                    legend_cols.append((i, col))
    """

    # TODO: use runID (or iterationvars?) as last resort (if present)

    # at this point, ideally this should hold: len(df.groupby([i2 for i1, i2 in legend_cols])) == len(df)
    return title_col, legend_cols


def make_legend_label(legend_cols, row):
    """
    Produces a reasonably good label text (to be used in a chart legend) for a result row from
    a DataFrame, given a list of selected columns as returned by `extract_label_columns()`.
    """

    if len(legend_cols) == 1:
        return str(row[legend_cols[0][0]])
    return ", ".join([col + "=" + str(row[i]) for i, col in legend_cols])


def make_chart_title(df, title_col, legend_cols):
    """
    Produces a reasonably good chart title text from a result DataFrame, given a selected "title"
    column, and a list of selected "legend" columns as returned by `extract_label_columns()`.
    """
    if df is None or df.empty or title_col not in df:
        return "None"

    what = str(list(df[title_col])[0]) if title_col else "Data"
    if title_col and len(df[title_col].unique()) > 1:
        what += " and other variables"
    by_what = (" (by " + ", ".join([id[1] for id in legend_cols]) + ")") if legend_cols else ""
    return what + by_what
