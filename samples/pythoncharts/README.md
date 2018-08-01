# Installation

At the moment Python plotting is only supported on Linux (Ubuntu, Fedora,
etc.).

In addition to the usual requirements of OMNeT++, you will also need a Python
3 interpreter (in `$PATH`, invokable as `python3`), and some packages that can
be easily installed via `pip`, or a similar Python package manager.

If you don't already have them, you can most likely install `python3` and
`pip` from the standard software repositories of your distribution. For
example on Ubuntu, you can use this command:

    sudo apt-get install python3 python3-pip

Required Python packages:

- `py4j (version 0.10.7)`
- `numpy`
- `pandas`
- `matplotlib`

Optional Python packages:

- `prettyplotlib`
- `seaborn`

Once `python3` and pip are available, a simple command to install all of these
would be:

    python3 -m pip install --upgrade --user py4j==0.10.7 numpy pandas matplotlib prettyplotlib seaborn

Installing all of these inside a virtual environment (like `virtualenv`) will
work fine, you just have to start the `omnetpp` IDE from a shell in which the
virtual environment is active.

# Scripting

The Python script of every chart has access to two implicitly declared
(effectively built-in) global objects when executed in the IDE: `results` and
`chart`. The former can be used to query and process simulation results, and
the latter to plot data using the built-in charts of the IDE. Plotting using
`matplotlib` can be done with its own API (usually `pyplot`) without
modification.

## Imported modules

In addition to the two global objects provided by OMNeT++ itself, some useful,
well known Python modules are always implicitly imported under their commonly
used aliases. You can think of this as if each chart script had this header
prepended to it:

```python
import numpy as np
import pandas as pd
import matplotlib as mpl
import matplotlib.pyplot as plt

# Only if available, otherwise aliases set to None:
import prettyplotlib as ppl
import seaborn as sbs

# The results and chart objects are also available.
```

Since the chart scripts run almost exactly like an ordinary Python program,
any other module available on the system can be imported, and is expected to
work normally.

This also means that any chart script can access any part of the system it
runs on (file system, network, other devices, etc.), with the permissions of
the user that started the IDE. We are aware that this might pose some security
issues (to put it lightly) with `.anf` files acquired from untrusted sources,
and plan to look into possible sandboxing solutions to restrict the chart
scripts' access to the system somewhat.

If the Python script gets stuck for some reason (too long computation, not
enough memory, deadlock, etc.), the IDE is supposed to remain responsive, and
the red "Stop" button can be used to kill the Python interpreter process.

# Accessing simulation results

The global `results` object has four methods that can be used to query
simulation results: `getScalars`, `getVectors`, `getStatistics`, and
`getHistograms`. (It has some other methods as well, which are useful to
transform the returned data; more on those later.) All of the querying methods
take a string as their only argument. This string is a filter expression to
select which results should be returned. The syntax of this expression is the
same as used on the "Browse Data" tab of the Analysis tool in the IDE. All of
these methods return a Pandas DataFrame, each in a fitting format, as
discussed below. The main structure of the DataFrames is similar to the one
used in the ["Result Analysis with Python"
tutorial](https://docs.omnetpp.org/tutorials/pandas/#4-exploring-the-data-frame),
with some differences.

## Data format of the results

The following sections show the format of the DataFrames containing the
simulation results, as returned by the methods of the `results` object.

They all have a simple (single-layer) column structure, and no index other
than the default row numbering; very much like a CSV file.

The `run` column always has a value in every row, identifying the simulation
run to which the given row belongs. It is the unique [run ID](https://omnetpp.org/doc/omnetpp/manual/#sec:config-sim:predefined-variables)
in the form of a simple string.

The second important column is `type` which denotes what kind of information
the row contains. Its value is one of `runattr`, `itervar`, `param`, `scalar`,
`vector`, `statistic`, `histogram`, `attr`. Which subsequent columns have
data, and how they should be interpreted, depends on the value in this column.

### Common elements

Some metadata about the simulation runs that produced the results is stored
along with the actual data in every returned DataFrame. These are of type
`runattr`, `itervar`, and `param`. The `runattr` rows contain some generic
information about the simulation run (the name of the network, the process ID
of the simulator, the date and time of the simulation), the `itervar` type
rows contain the names and values of the iteration variables, and the `param`
rows contain the unexpanded (potentially wildcard) parameter assignments read
from the `.ini` file.

|  | run | type | attrname | attrvalue |
|--|--|--|--|--|--|--|
| 0 | PureAlohaExperiment-3-20170627-20:42:20-22739 | runattr | configname | PureAlohaExperiment |
| 1 | PureAlohaExperiment-3-20170627-20:42:20-22739 | runattr | datetime | 20170627-20:42:20 |
| 14 | PureAlohaExperiment-37-20180518-14:29:59-31710 | itervar | iaMean | 5 |
| 15 | PureAlohaExperiment-37-20180518-14:29:59-31710 | itervar | numHosts | 20 |
| 24 | PureAlohaExperiment-3-20170627-20:42:20-22739 | param | **.animationHoldTimeOnCollision | 0s |
| 25 | PureAlohaExperiment-3-20170627-20:42:20-22739 | param | **.idleAnimationSpeed | 1 |

On top of these, every kind of result dataframe might have a number of rows of
type `attr`. These add some metadata to individual result items. The value of
the attribute is in the `attrvalue` column.

|  | run | type | module | name | attrname | attrvalue |
|--|--|--|--|--|--|--|
| 3261 | PureAlohaExperiment-23-20170627-20:42:20-22757 | attr | Aloha.server | collisionLength:mean | title | collision length, mean |
| 3263 | PureAlohaExperiment-23-20170627-20:42:20-22757 | attr | Aloha.server | collisionLength:sum | title | collision length, sum |

The actual result items will add some columns to the common ones listed above
to store their data. These columns are filled with `None` or `NaN` in the rows
where they are not used.

### Scalars

The scalar type results only uses the `value` column, containing simply the
value of the scalar.

|  | run | type | module | name | attrname | attrvalue | value |
|--|--|--|--|--|--|--|--|
| 3262 | PureAlohaExperiment-23-20170627-20:42:20-22757 | **scalar** | Aloha.server | collisionLength:sum | None | None | **470.089473** |
| 3263 | PureAlohaExperiment-23-20170627-20:42:20-22757 | attr | Aloha.server | collisionLength:sum | title | collision length, sum | NaN |
| 3264 | PureAlohaExperiment-23-20170627-20:42:20-22757 | **scalar** | Aloha.server | collisionLength:max | None | None | **0.424681** |
| 3265 | PureAlohaExperiment-23-20170627-20:42:20-22757 | attr | Aloha.server | collisionLength:max | title | collision length, max | NaN |

### Vectors

Vector type results add the `vectime` and `vecvalue` columns, each containing
Numpy arrays (of pairwise equal length), holding the collection time and the
collected value of the vector results, respectively.

|  | run | type | module | name | attrname | attrvalue | vectime | vecvalue |
|--|--|--|--|--|--|--|--|--|
| 453 | Net5SaturatedQueue-0-20180518-14:30:05-31772 | vector | Net5.rte[1].app | hopCount:vector | None | None | [1.0700600435950003, 1.5654095763330003, 1.568... | [0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, ... |
| 455 | Net5SaturatedQueue-0-20180518-14:30:05-31772 | attr | Net5.rte[1].app | hopCount:vector | title | hop count of arrived packets, vector | None | None |
| 456 | Net5SaturatedQueue-0-20180518-14:30:05-31772 | vector | Net5.rte[1].app | sourceAddress:vector | None | None | [1.0700600435950003, 1.5654095763330003, 1.568... | [1.0, 1.0, 2.0, 0.0, 2.0, 0.0, 4.0, 3.0, 0.0, ... |
| 458 | Net5SaturatedQueue-0-20180518-14:30:05-31772 | attr | Net5.rte[1].app | sourceAddress:vector | title | source address of arrived packets, vector | None | None |

### Statistics

The statistic type results add the `count`, `sumweights`, `mean`, `stddev`,
`min`, and `max` columns, the content of each being pretty self-explanatory.

|  | run | type | module | name | attrname | attrvalue | count | sumweights | mean | stddev | min | max |
|--|--|--|--|--|--|--|--|--|--|--|--|--|
| 3668 | PureAlohaExperiment-41-20180523-14:00:38-31129 | statistic | Aloha.host[13] | radioState:stats | None | None | 1177.0 | 1177.0 | 0.499575 | 0.500212 | 0.0 | 1.0 |
| 3669 | PureAlohaExperiment-41-20180523-14:00:38-31129 | attr | Aloha.host[13] | radioState:stats | enum | IDLE=0,TRANSMIT=1 | NaN | NaN | NaN | NaN | NaN | NaN |
| 3672 | PureAlohaExperiment-41-20180523-14:00:38-31129 | statistic | Aloha.host[14] | radioState:stats | None | None | 1221.0 | 1221.0 | 0.499590 | 0.500205 | 0.0 | 1.0 |
| 3673 | PureAlohaExperiment-41-20180523-14:00:38-31129 | attr | Aloha.host[14] | radioState:stats | enum | IDLE=0,TRANSMIT=1 | NaN | NaN | NaN | NaN | NaN | NaN |

### Histograms

Histograms are an extension of the statistic type results, so all of the
columns described above are included, plus the `binedges` and `binvalues`
columns. Each of these two contain Numpy arrays of pairwise equal length. The
meaning of these pairs is a bit different from what the `chart.plotHistograms`
method expects, because this includes the under- and overflown values as well.
Here, each pair of "edge" and "value" elements define a single bin, with its
left edge and its value. Two pseudo-bins are also included, one on each side,
to hold the underflown and overflown values.

The conversion simply consists of dropping the first value of the `binedges`
(which is always `-inf`), and the first and last values of the `binvalues`
array (which are the sum of underflown and overflown weights, respectively).
This makes the edge array one element longer than the value array.


|  | run | type | module | name | attrname | attrvalue | count | sumweights | mean | stddev | min | max | binedges | binvalues |
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
| 17870 | Net10Experiment-29-20180523-14:00:59-31156 | histogram | Net10.rte[9].queue[2] | txBytes:histogram | None | None | 3850.0 | 3850.0 | 4096.000000 | 0.000000 | 4096.000000 | 4096.000000 |  | [-inf, 4095.0, 4096.0, 4097.0] | [0.0, 0.0, 3850.0, 0.0] |
| 17873 | Net10Experiment-29-20180523-14:00:59-31156 | attr | Net10.rte[9].queue[2] | txBytes:histogram | unit | bytes | NaN | NaN | NaN | NaN | NaN | NaN | None | None |
| 17874 | PureAlohaExperiment-30-20180523-14:00:34-31124 | histogram | Aloha.server | collisionLength:histogram | None | None | 12489.0 | 12489.0 | 0.204422 | 0.093191 | 0.099178 | 1.151026 | [-inf, 0.0, 0.02, 0.04, 0.06, 0.08, 0.1, 0.12,... | [0.0, 0.0, 0.0, 0.0, 0.0, 81.0, 1613.0, 1543.0...
| 17875 | PureAlohaExperiment-30-20180523-14:00:34-31124 | attr | Aloha.server | collisionLength:histogram | title | collision length, histogram | NaN | NaN | NaN | NaN | NaN | NaN | None | None |


### Transforming the DataFrame

To turn this into more practical formats, you can use the `pivotScalars`
function of the `results` object, like this:

```python
df = results.getScalars('name("*throughput")')
df = results.pivotScalars(df)
```

This results in a DataFrame in a different format that is much easier to process:

_TODO add example transformed DataFrame _

The `pivotScalars` method also has two optional parameters, to specify which
labels should be used to form the column and row headers (indices).

The available labels are:

- `name`
- `module`
- `run`
- `experiment`
- `measurement`
- `replication`
- etc...


TODO make it work with other types of results, not just scalars


# Plotting

There are two main ways to plot data from charts: `matplotlib`, and the custom
(native) plots of the IDE. The type of the Chart (Matplotlib or Bar,
Histogram, etc.) determines which one is available.

## Plotting with matplotlib

The full API of `matplotlib` (including `pyplot`) can be used in the scripts
of Matplotlib charts without modification. Calling `plt.show()` at the end of
the scripts is not necessary.

At the moment every chart must draw onto a single figure only. Subplots and
multiple axes work fine though.

Navigation, and interaction via mouse event handlers (cursors, widgets, or
tooltips for example) is supported.

The usual set of tool buttons (Home) are available on the local toolbar of the
chart.

Extensions for `matplotlib`, like `prettyplotlib` or `seaborn` can also be used.

All file formats supported by Matplotlib (PDF, PS, PNG, JPG, SVG,
etc.) are available during Export.

## Plotting using the built-in charts

The IDE provides built-in charts of three different types: scalar (for bar
charts), XY (for vector/line/scatter charts), and histogram (for... well...
histograms). This is done through the `chart` global object. It has one or two
methods for every type of chart: `plotScalar`, `plotVector[s]`, and
`plotHistogram[s]`. Which ones are available depends on the type of the chart:
`plotScalars` can only be used in Bar charts, `plotVector` and `plotVectors`
in Line and Scatter charts, `plotHistogram` and `plotHistograms` in Histogram
charts. They each take data in multiple formats, as discussed below.

These methods each accept data in a variety of fitting formats. All of them
can consume data in the same format as returned by the corresponding querying
methods of the `results` object, so for example this always works:

```python
histograms = results.getHistograms("*")
chart.plotHistograms(histograms)
```

### Plotting Scalars

The following code sample demonstrates all possible invocations of the
`plotScalars` method, with the comments providing a short explanation for each
of them:

```python

# just plain values, not so useful
chart.plotScalars([10, 13, 18, 20])

# values and labels
chart.plotScalars([2, 11, 34], labels=["smol", "medium", "larg"])

# values and labels, plus a row_label
chart.plotScalars([10, 13, 18], labels=["a little", "a bit more", "quite a lot"], row_label="amount of milk")

# a simple dataframe
d1 = pd.DataFrame(
    [[10, 30],
     [12, 34]],
    columns=["X", "Y"],
    index=["alpha", "beta"])
#         X   Y
# alpha  10  30
# beta   12  34
chart.plotScalars(df1)

# a dataframe with multi-level index and column names
df2 = pd.DataFrame(
    [[10, 300, 11, 320],
     [12, 345, 10, 305],
     [11, 325, 12, 348],
     [10, 302, 11, 370]],
    columns=pd.MultiIndex.from_product([["X", "Y"], ["x", "y"]]),
    index=pd.MultiIndex.from_product([["A", "B"], ["a", "b"]]))
#       X        Y
#       x    y   x    y
# A a  10  300  11  320
#   b  12  345  10  305
# B a  11  325  12  348
#   b  10  302  11  370
chart.plotScalars(df2)


# dataframe in raw scave format
df3 = results.getScalars("name(rxBytes:sum) OR name(txBytes:sum)")
chart.plotScalars(df3)

# scave dataframes with custom row and column labels
d4 = results.getScalars("name(rxBytes:sum) OR name(txBytes:sum)")
d4 = results.pivotScalars(df4, columns=["name", "replication"], index=["module"])
chart.plotScalars(df4)

```

### Plotting Vectors

The following code sample demonstrates all possible invocations of the
`plotVector` and `plotVectors` methods, with the comments providing a short
explanation for each of them:

```python

# a single line with a label, x and y coords as lists
chart.plotVector("trend", [0, 1, 2, 3], [3,6,7,9])

# this can be called multiple times in a single chart, to plot multiple lines
chart.plotVector("trend", [0, 1, 2, 3], [3,6,7,9])
chart.plotVector("tronb", [0, 1, 2, 3], [4,8,7,3])
chart.plotVector("trang", [0, 1.5, 2.5, 3], [9,7,4,1])

# a list of tuples, each with a set of parameters, similar to the above, except in a single invokation
data = [
    ("apples", [2000, 2010, 2020], [300, 367, 341]),
    ("oranges", [2002, 2014, 2019], [170, 190, 220]),
    ("pears", [1999, 2003, 2014], [235, 340, 320])
]
chart.plotVectors(data)


# a simple dataframe, each column being a vector
df1 = pd.DataFrame({
    "N": [120, 161, 283],
    "Y": [300, 320, 345]
})
#      N    Y
# 0  120  300
# 1  161  320
# 2  283  345
chart.plotVectors(df1)

# same, but with the "time" column added, to act as a common X axis for both vectors
df2 = pd.DataFrame({
    "N": [120, 161, 283],
    "Y": [300, 320, 345],
    "time": [0.1, 0.2, 0.45]
})
#     N    Y  time
# 0  10  300  0.10
# 1  11  320  0.20
# 2  13  345  0.45
chart.plotVectors(df2)

# dataframe in scave format
df3 = results.getVectors("name(hopCount:vector)")
chart.plotVectors(df3)
```

TODO: frames with hierarchical index?

### Plotting Histograms

The following code sample demonstrates all possible invocations of the
`plotHistogram` and `plotHistograms` methods, with the comments providing a
short explanation for each of them:

```python

# a simple histogram with bin edges and values (always 1 more edge than value)
chart.plotHistogram(label="IQ", edges=[80, 90, 100, 110, 120, 130], values=[2, 3, 7, 6, 4])

# this can be called multiple times, and bin sizes don't have to be uniform
chart.plotHistogram("A", [80, 90, 105, 118, 130], [2, 3, 7, 4])
chart.plotHistogram("B", [83, 97, 101, 110, 124, 137], [0.6, 0.8, 1.5, 1.1, 0.9])

# there is no histogram computation from samples, can be done with numpy if needed
samples = np.random.normal(loc=42, scale=6.28, size=1000000)
values, edges = np.histogram(samples, bins=64)
chart.plotHistogram("bell", edges, values)

# dataframe in scave format
df = results.getHistograms("name(collisionLength:histogram)")
chart.plotHistograms(df)

```

TODO: add other dataframe formats for histograms?

### Chart properties

The script has access to the name of the chart it implements. For native
charts, it can also access and mofify the style and formatting properties,
which are also configurable in their edit dialogs. For Matplotlib charts, only
the `getName()` method is available.

Properties are always returned and accepted as strings, with properties of
numeric, color, or font type encoded in a simple textual format.

```python

chart.getName()

chart.getProperties()

chart.getProperty("key")

chart.getDefaultProperties()

chart.setProperty("key", "value")
chart.setProperties({"key1": "value1", "key2": "value2"})
vhart.setProperties(key1="value1", key2="value2") # Usually not permitted by Python syntax due to periods in property keys

```

TODO: what property names are accepted, what they do.
maybe provide mapping with simple methods, like `setLineStyle("datarate", "dots")`?

## Using external scripts

We realize that currently it is really inconvenient to edit the script source
inside that dialog, using a plain text editor. To make this manageable, the
working directory of the python process is changed to the directory where the
`.anf` file (containing the chart) is located. This makes it easy to put the
code of the charts into a separate `.py` file and import it.

This makes it possible to use any external editor, or even [PyDev](http://www.pydev.org/)
for example, to edit the bulk of chart script the more conveniently.

PyDev is an Eclipse plugin with excellent support for editing Python code,
with syntax highlight, content assist, and so on - but its built-in debugging
support won't work charts unfortunately. You just have to install it into the
IDE.

One caveat with this is that the implicit objects (`results`, and `chart`) are
not globally available in the external source file, so they have to be passed
in as function parameters for example. Matplotlib can be used with no
modification, it just has to be imported manually, as usual. Also, none of the
other implicitly imported modules are availeble automatically either, but can
be imported of course.

See the example charts with `importing` in the name for examples.

## Comparing the two ways of plotting

With native charts, the Python process is killed right after it is done
executing the script, as it is not needed anymore, and there is no way to
interact with it.

With Matplotlib charts, the process has to be kept alive for as long as the
chart is used, because there is an option for interactivity there, and even
all navigation (zooming, panning) is done by Matplotlib itself.

#### Matplotlib:

Pros:
 - A wide variety of plot types and extensions available
 - Supports subplots and subaxes
 - Plots can be interactive (via mouse event callbacks)
 - Well known library with extensive documentation and lots of examples
 - Looks pretty most of the time
 - Can export charts in many file formats (JPG, PNG, PDF, SVG, PS, etc...)

Cons:
 - Can be slow
 - A whole new API to learn, if not yet familiar
 - All customization/styling must be done in Python code

#### Native plots:

Pros:
 - Usually faster, more responsive
 - Can be more easily configured in a dialog

Cons:
 - No interactivity (except for viewport manipulation of course)
 - Fixed functionality (three or four specific types of charts)

# Future plans

Adding ability to export charts into standalone scripts, to be run outside the
IDE. This will need pure Python implementation of the result acquisition
object (`results`) with the data loaded from files, and the native plotting
object (`chart`), probably reimplemented with Matplotlib.

Integration of PyDev for better support of chart script editing, console
output, error handling, and maybe standard input and debugging?
