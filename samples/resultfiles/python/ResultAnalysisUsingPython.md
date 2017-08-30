Analysing Simulation Results With Python
========================================

1. When to use Python?
----------------------

The Analysis Tool in the OMNeT++ IDE is best suited for casual exploration of
simulation results. If you are doing sophisticated result analysis, you will
notice after a while that you have outgrown the IDE. The need for customized
charts, the necessity of multi-step computations to produce chart input, or the 
sheer volume of raw simulation results might all be causes to make you look for 
something else.

If you are an R or Matlab expert, you'll probably reach for those tools, but for
everyone else, Python with the right libraries is pretty much the best choice.
Python has a big momentum for data science, and in addition to having excellent
libraries for data analysis and visualization, it is also a great general-purpose
programming language. Python is used for diverse problems ranging from building
desktop GUIs to machine learning and AI, so the knowledge you gain by learning
it will be convertible to other areas.

This tutorial will walk you through the initial steps of using Python for
analysing simulation results, and shows how to do some of the most common tasks.
The tutorial assumes that you have a working knowledge of OMNeT++ with regard
to result recording, and basic familiarity with Python.


2. Setting up
-------------

Before we can start, you need to install the necessary software.
First, make sure you have Python, either version 2.x or 3.x (they are
slightly incompatible.) If you have both versions available on your system,
we recommend version 3.x. You also need OMNeT++ version 5.2 or later.

We will heavily rely on three Python packages: [NumPy](http://www.numpy.org/),
[Pandas](http://pandas.pydata.org/), and [Matplotlib](https://matplotlib.org/).
There are also optional packages that will be useful for certain tasks:
[SciPy](https://www.scipy.org/),
[PivotTable.js](https://github.com/nicolaskruchten/pivottable).
We also recommend that you install [IPython](https://ipython.org/) and
[Jupyter](https://jupyter.org/), because they let you work much more comfortably
than the bare Python shell.

On most systems, these packages can be installed with `pip`, the Python package
manager (if you go for Python 3, replace `pip` with `pip3` in the commands
below):

    sudo pip install ipython jupyter
    sudo pip install numpy pandas matplotlib
    sudo pip install scipy pivottablejs

As packages continually evolve, there might be incompatibilities between 
versions. We used the following versions when writing this tutorial:
Pandas 0.20.2, NumPy 1.12.1, SciPy 0.19.1, Matplotlib 1.5.1, PivotTable.js 0.8.0.
An easy way to determine which versions you have installed is using the `pip list` 
command. (Note that the last one is the version of the Python interface library, 
the PivotTable.js main Javascript library uses different version numbers, e.g. 
2.7.0.)



3. Getting your simulation results into Python
----------------------------------------------

OMNeT++ result files have their own file format which is not directly
digestible by Python. There are a number of ways to get your data
inside Python:

  1. Export from the IDE. The Analysis Tool can export data in a number of
  formats, the ones that are useful here are CSV and Python-flavoured JSON.
  In this tutorial we'll use the CSV export, and read the result into Pandas
  using its `read_csv()` function.

  2. Export using scavetool. Exporting from the IDE may become tedious
  after a while, because you have to go through the GUI every time your
  simulations are re-run. Luckily, you can automate the exporting with
  OMNeT++'s scavetool program. scavetool exposes the same export
  functionality as the IDE, and also allows filtering of the data.

  3. Read the OMNeT++ result files directly from Python. Development
  of a Python package to read these files into Pandas data frames is
  underway, but given that these files are line-oriented text files
  with a straightforward and well-documented structure, writing your
  own custom reader is also a perfectly feasible option.

  4. SQLite. Since version 5.1, OMNeT++ has the ability to record simulation
  results int SQLite3 database files, which can be opened directly from
  Python using the [sqlite](https://docs.python.org/3/library/sqlite3.html)
  package. This lets you use SQL queries to select the input data for your
  charts or computations, which is kind of cool! You can even use GUIs like
  [SQLiteBrowser](http://sqlitebrowser.org/) to browse the database and
  craft your SELECT statements. Note: if you configure OMNeT++ for SQLite3
  output, you'll still get `.vec` and `.sca` files as before, only their
  format will change from textual to SQLite's binary format. When querying
  the contents of the files, one issue  to deal with is that SQLite does not
  allow cross-database queries, so you either need to configure OMNeT++
  to record everything into one file (i.e. each run should append instead
  of creating a new file), or use scavetool's export functionality to
  merge the files into one.

  5. Custom result recording. There is also the option to instrument
  the simulation (via C++ code) or OMNeT++ (via custom result recorders)
  to produce files that Python can directly digest, e.g. CSV.
  However, in the light of the above options, it is rarely necessary
  to go this far.

With large-scale simulation studies, it can easily happen that the
full set of simulation results do not fit into the memory at once.
There are also multiple approaches to deal with this problem:

  1. If you don't need all simulation results for the analysis, you can
  configure OMNeT++ to record only a subset of them. Fine-grained control
  is available.
  2. Perform filtering and aggregation steps before analysis. The IDE and
  scavetool are both capable of filtering the results before export.
  3. When the above approaches are not enough, it can help to move
  part of the result processing (typically, filtering and aggregation)
  into the simulation model as dedicated result collection modules.
  However, this solution requires significantly more work than the previous
  two, so use with care.

In this tutorial, we'll work with the contents of the `samples/resultfiles`
directory distributed with OMNeT++. The directory contains result
files produced by the Aloha and Routing sample simulations, both
of which are parameter studies. We'll start by looking at the Aloha results.

As the first step, we use OMNeT++'s *scavetool* to convert Aloha's scalar files
to CSV. Run the following commands in the terminal (replace `~/omnetpp` with 
the location of your OMNeT++ installation):

    cd ~/omnetpp/samples/resultfiles/aloha
    scavetool x *.sca -o aloha.csv

In the scavetool command line, `x` means export, and the export format is 
inferred from the output file's extension. (Note that scavetool supports 
two different CSV output formats. We need *CSV Records*, or CSV-R for short, 
which is the default for the `.csv` extension.)

Let us spend a minute on what the export has created. The CSV file
has a fixed number of columns named `run`, `type`, `module`, `name`,
`value`, etc. Each result item, i.e. scalar, statistic, histogram
and vector, produces one row of output in the CSV. Other items such
as run attributes, iteration variables of the parameter study and result
attributes also generate their own rows. The content of the `type` column
determines what type of information a given row contains. The `type`
column also determines which other columns are in use. For example,
the `binedges` and `binvalues` columns are only filled in for histogram
items. The colums are:

- *run*: Identifies the simulation run
- *type*: Row type, one of the following: `scalar`, `vector`, `statistics`,
  `histogram`, `runattr`, `itervar`, `param`, `attr`
- *module*: Hierarchical name (a.k.a. full path) of the module that recorded the
  result item
- *name*: Name of the result item (scalar, statistic, histogram or vector)
- *attrname*: Name of the run attribute or result item attribute (in the latter
  case, the `module` and `name` columns identify the result item the attribute
  belongs to)
- *attrvalue*: Value of run and result item attributes, iteration variables,
  saved ini param settings (`runattr`, `attr`, `itervar`, `param`)
- *value*: Output scalar value
- *count*, *sumweights*, *mean*, *min*, *max*, *stddev*: Fields of the statistics 
  or histogram
- *binedges*, *binvalues*: Histogram bin edges and bin values, as space-separated
  lists. *len(binedges)==len(binvalues)+1*
- *vectime*, *vecvalue*: Output vector time and value arrays, as space-separated
  lists

When the export is done, you can start Jupyter server with the following command:

    jupyter notebook

Open a web browser with the displayed URL to access the Jupyter GUI. Once there,
choose *New* -> *Python3* in the top right corner to open a blank notebook.
The notebook allows you to enter Python commands or sequences of commands,
run them, and view the output. Note that *Enter* simply inserts a newline;
hit *Ctrl+Enter* to execute the commands in the current cell, or *Alt+Enter*
to execute them and also insert a new cell below.

If you cannot use Jupyter for some reason, a terminal-based Python shell 
(`python` or `ipython`) will also allow you to follow the tutorial.

On the Python prompt, enter the following lines to make the functionality of
Pandas, NumpPy and Matplotlib available in the session. The last, `%matplotlib`
line is only needed for Jupyter. (It is a "magic command" that arranges plots
to be displayed within the notebook.)

```{.python .input}
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
%matplotlib inline
```

We utilize the `read_csv()` function to import the contents of the
CSV file into a data frame. The data frame is the central concept of
Pandas. We will continue to work with this data frame throughout
the whole tutorial.

```{.python .input}
aloha = pd.read_csv('../aloha/aloha.csv')
```

4. Exploring the data frame
---------------------------

You can view the contents of the data frame by simply entering the name
of the variable (`aloha`). Alternatively, you can use the `head()` method
of the data frame to view just the first few lines.

```{.python .input}
aloha.head()
```

You can see that the structure of the data frame, i.e. rows and columns,
directly corresponds to the contents of the CSV file. Column names have
been taken from the first line of the CSV file. Missing values are
represented with NaNs (not-a-number).

The complementary `tail()` method shows the last few lines. There is also
an `iloc` method that we use at places in this tutorial to show rows
from the middle of the data frame. It accepts a range: `aloha.iloc[20:30]`
selects 10 lines from line 20, `aloha.iloc[:5]` is like `head()`, and
`aloha.iloc[-5:]` is like `tail()`.

```{.python .input}
aloha.iloc[1200:1205]
```

Hint: If you are in the terminal and you find that the data frame printout does
not make use of the whole width of the terminal, you can increase the display 
width for better readability with the following commands:

```{.python .input}
pd.set_option('display.width', 180)
pd.set_option('display.max_colwidth', 100)
```

If you have not looked at any Pandas tutorial yet, now is a very good
time to read one. (See References at the bottom of this page for hints.)
Until you finish, here are some basics for your short-term survival.

You can refer to a column as a whole with the array index syntax: `aloha['run']`.
Alternatively, the more convenient member access syntax (`aloha.run`) can
also be used, with restrictions. (E.g. the column name must be valid as a Python
identifier, and should not collide with existing methods of the data frame.
Names that are known to cause trouble include `name`, `min`, `max`, `mean`).

```{.python .input}
aloha.run.head()  # .head() is for limiting the output to 5 lines here
```

Selecting multiple columns is also possible, one just needs to use a list of
column names as index. The result will be another data frame. (The double
brackets in the command are due to the fact that both the array indexing and 
the list syntax use square brackets.)

```{.python .input}
tmp = aloha[['run', 'attrname', 'attrvalue']]
tmp.head()
```

The `describe()` method can be used to get an idea about the contents of a 
column. When applied to a non-numeric column, it prints the number of 
non-null elements in it (`count`), the number of unique values (`unique`),
the most frequently occurring value (`top`) and its multiplicity (`freq`),
and the inferred data type (more about that later.)

```{.python .input}
aloha.module.describe()
```

You can get a list of the unique values using the `unique()` method. For example,
the following command lists the names of modules that have recorded any statistics:

```{.python .input}
aloha.module.unique()
```

When you apply `describe()` to a numeric column, you get a statistical summary
with things like mean, standard deviation, minimum, maximum, and various 
quantiles.

```{.python .input}
aloha.value.describe()
```

Applying `describe()` to the whole data frame creates a similar report about
all numeric columns.

```{.python .input}
aloha.describe()
```

Let's spend a minute on data types and column data types. Every column has a
data type (abbreviated *dtype*) that determines what type of values it may
contain. Column dtypes can be printed with `dtypes`:

```{.python .input}
aloha.dtypes
```

The two most commonly used dtypes are *float64* and *object*. A *float64* column
contains floating-point numbers, and missing values are represented with NaNs. 
An *object* column may contain basically anything -- usually strings, but we'll
also have NumPy arrays (`np.ndarray`) as elements in this tutorial. 
Numeric values and booleans may also occur in an *object* column. Missing values
in an *object* column are usually represented with `None`, but Pandas also
interprets the floating-point NaN like that.
Some degree of confusion arises from fact that some Pandas functions check 
the column's dtype, while others are already happy if the contained elements
are of the required type. To clarify: applying `describe()` to a column 
prints a type inferred from the individual elements, *not* the column dtype.
The column dtype type can be changed with the `astype()` method; we'll see an
example for using it later in this tutorial.

The column dtype can be accessed as the `dtype` property of a column, for example
`aloha.stddev.dtype` yields `dtype('float64')`. There are also convenience
functions such as `is_numeric_dtype()` and `is_string_dtype()` for checking 
column dtype. (They need to be imported from the `pandas.api.types` package
though.)

Another vital thing to know, especially due of the existence of the *type*
column in the OMNeT++ CSV format, is how to filter rows. Perhaps surprisingly, 
the array index syntax can be used here as well. For example, the following expression 
selects the rows that contain iteration variables: `aloha[aloha.type == 'itervar']`. 
With a healthy degree of sloppiness, here's how it works: `aloha.type` yields 
the values in the `type` column as an array-like data structure; 
`aloha.type=='itervar'` performs element-wise comparison and produces an array
of booleans containing `True` where the condition holds and `False` where not;
and indexing a data frame with an array of booleans returns the rows that 
correspond to `True` values in the array.

Conditions can be combined with AND/OR using the "`&`" and "`|`" operators, but
you need parentheses because of operator precedence. The following command
selects the rows that contain scalars with a certain name and owner module:

```{.python .input}
tmp = aloha[(aloha.type=='scalar') & (aloha.module=='Aloha.server') & (aloha.name=='channelUtilization:last')]
tmp.head()
```

You'll also need to know how to add a new column to the data frame. Now that is
a bit controversial topic, because at the time of writing, there is a "convenient"
syntax and an "official" syntax for it. The "convenient" syntax is a simple
assignment, for example:

```{.python .input}
aloha['qname'] = aloha.module + "." + aloha.name
aloha[aloha.type=='scalar'].head()  # print excerpt
```

It looks nice and natural, but it is not entirely correct. It often results in 
a warning: *SettingWithCopyWarning: A value is trying to be set on a copy of a 
slice from a DataFrame...*. The message essentially says that the operation
(here, adding the new column) might have been applied to a temporary object 
instead of the original data frame, and thus might have been ineffective. 
Luckily, that is not the case most of the time (the operation *does* take 
effect). Nevertheless, for production code, i.e. scripts, the "official" 
solution, the `assign()` method of the data frame is recommended, like this:

```{.python .input}
aloha = aloha.assign(qname = aloha.module + "." + aloha.name)
aloha[aloha.type=='scalar'].head()
```

For completeness, one can remove a column from a data frame using either the
`del` operator or the `drop()` method of the data frame. Here we show the former
(also to remove the column we added above, as we won't need it for now):

```{.python .input}
del aloha['qname']
```


5. Revisiting CSV loading
-------------------------

The way we have read the CSV file has one small deficiency: all data in the
`attrvalue` column are represented as strings, event though many of them
are really numbers, for example the values of the `iaMean` and `numHosts`
iteration variables. You can verify that by printing the unique values (
`aloha.attrvalue.unique()` -- it will print all values with quotes), or using 
the `type()` operator on an element:

```{.python .input}
type( aloha[aloha.type=='scalar'].iloc[0].value )
```

The reason is that `read_csv()` infers data types of columns from the data
it finds in them. Since the `attrvalue` column is shared by run attributes, 
result item attributes, iteration variables and some other types of rows,
there are many non-numeric strings in it, and `read_csv()` decides that it is
a string column.

A similar issue arises with the `binedges`, `binvalues`, `vectime`, `vecvalue`
columns. These columns contain lists of numbers separated by spaces, so they
are read into strings as well. However, we would like to store them as NumPy
arrays (`ndarray`) inside the data frame, because that's the form we can use
in plots or as computation input.

Luckily, `read_csv()` allows us to specify conversion functions for each column.
So, armed with the following two short functions:

```{.python .input}
def parse_if_number(s):
    try: return float(s)
    except: return True if s=="true" else False if s=="false" else s if s else None

def parse_ndarray(s):
    return np.fromstring(s, sep=' ') if s else None
```

we can read the CSV file again, this time with the correct conversions:

```{.python .input}
aloha = pd.read_csv('../aloha/aloha.csv', converters = {
    'attrvalue': parse_if_number,
    'binedges': parse_ndarray,
    'binvalues': parse_ndarray,
    'vectime': parse_ndarray,
    'vecvalue': parse_ndarray})
```

You can verify the result e.g. by printing the unique values again.


6. Load-time filtering
----------------------

If the CSV file is large, you may want to skip certain columns or rows when
reading it into memory. (File size is about the only valid reason for using
load-time filtering, because you can also filter out or drop rows/columns
from the data frame when it is already loaded.)

To filter out columns, you need to specify in the `usecols` parameter
the list of columns to keep:

```{.python .input}
tmp = pd.read_csv('../aloha/aloha.csv', usecols=['run', 'type', 'module', 'name', 'value'])
```

There is no such direct support for filtering out rows based on their content,
but we can implement it using the iterator API that reads the CSV file 
in chunks. We can filter each chunk before storing and finally concatenating 
them into a single data frame:

```{.python .input}
iter = pd.read_csv('../aloha/aloha.csv', iterator=True, chunksize=100)
chunks = [ chunk[chunk['type']!='histogram'] for chunk in iter ]  # discards type=='histogram' lines
tmp = pd.concat(chunks)
```


7. Plotting scalars
-------------------

Scalars can serve as input for many different kinds of plots. Here we'll show
how one can create a "throughput versus offered load" type plot. We will plot
the channel utilization in the Aloha model in the function of the packet 
generation frequency. Channel utilization is also affected by the number of 
hosts in the network -- we want results belonging to the same number of hosts
to form iso lines. Packet generation frequency and the number of hosts are
present in the results as iteration variables named `iaMean` and `numHosts`;
channel utilization values are the `channelUtilization:last` scalars saved 
by the `Aloha.server` module. The data contains the results from two simulation
runs for each *(iaMean, numHosts)* pair done with different seeds; we want
to average them for the plot.

The first few steps are fairly straightforward. We only need the scalars and the
iteration variables from the data frame, so we filter out the rest. Then we
create a `qname` column from other columns to hold the names of our variables:
the names of scalars are in the `module` and `name` columns (we want to join them
with a dot), and the names of iteration variables are in the `attrname` column.
Since `attrname` is not filled in for scalar rows, we can take `attrname` as`qname`
first, then fill in the holes with *module.name*. We use the `combine_first()`
method for that: `a.combine_first(b)` fills the holes in `a` using the 
corresponding values from `b`.

The similar issue arises with values: values of output scalars are in the `value` 
column, while that of iteration variables are in the `attrvalue` column.
Since `attrvalue` is unfilled for scalar rows, we can again utilize 
`combine_first()` to merge two. There is one more catch: we need to change 
the dtype of the `attrvalue` to `float64`, otherwise the resulting `value`
column also becomes `object` dtype. (Luckily, all our iteration variables are 
numeric, so the dtype conversion is possible. In other simulations that contain
non-numeric itervars, one needs to filter those out, force them into numeric
values somehow, or find some other trick to make things work.)

```{.python .input}
scalars = aloha[(aloha.type=='scalar') | (aloha.type=='itervar')]  # filter rows
scalars = scalars.assign(qname = scalars.attrname.combine_first(scalars.module + '.' + scalars.name))  # add qname column 
scalars.value = scalars.value.combine_first(scalars.attrvalue.astype('float64'))  # merge value columns
scalars[['run', 'type', 'qname', 'value', 'module', 'name', 'attrname']].iloc[80:90]  # print an excerpt of the result
```

To work further, it would be very convenient if we had a format where each
simulation run corresponds to one row, and all variables produced by that
run had their own columns. We can call it the *wide* format, and it can be
produced using the `pivot()` method:

```{.python .input}
scalars_wide = scalars.pivot('run', columns='qname', values='value')
scalars_wide.head()
```

We are interested in only three columns for our plot:

```{.python .input}
scalars_wide[['numHosts', 'iaMean', 'Aloha.server.channelUtilization:last']].head()
```

Since we have our *x* and *y* data in separate columns now, we can utilize the
scatter plot feature of the data frame for plotting it:

```{.python .input}
scalars_wide.plot.scatter('iaMean', 'Aloha.server.channelUtilization:last')
plt.show()
```
NOTE: Although `plt.show()` is not needed in Jupyter (`%matplotlib inline`
turns on immediate display), we'll continue to include it in further code
fragments, so that they work without change when you use another Python shell.

The resulting chart looks quite good as the first attempt. However, it has some 
shortcomings: 

- Dots are not connected. The dots that have the same `numHosts` value should
  be connected with iso lines.
- As the result of having two simulation runs for each *(iaMean,numHosts)* pair,
  the dots appear in pairs. We'd like to see their averages instead.
  
Unfortunately, scatter plot can only take us this far, we need to look for 
another way.

What we really need as chart input is a table where rows correspond to different
`iaMean` values, columns correspond to different `numHosts` values, and cells
contain channel utilization values (the average of the repetitions). 
Such table can be produced from the "wide format" with another pivoting
operation. We use `pivot_table()`, a cousin of the `pivot()` method we've seen above.
The difference between them is that `pivot()` is a reshaping operation (it just
rearranges elements), while `pivot_table()` is more of a spreadsheet-style 
pivot table creation operation, and primarily intended for numerical data. 
`pivot_table()` accepts an aggregation function with the default being *mean*,
which is quite convenient for us now (we want to average channel utilization
over repetitions.)

```{.python .input}
aloha_pivot = scalars_wide.pivot_table(index='iaMean', columns='numHosts', values='Aloha.server.channelUtilization:last')  # note: aggregation function = mean (that's the default)
aloha_pivot.head()
```

Note that rows correspond to various `iaMean` values (`iaMean` serves as index);
there is one column for each value of `numHosts`; and that data in the table
are the averages of the channel utilizations produced by the simulations 
performed with the respective `iaMean` and `numHosts` values. 

For the plot, every column should generate a separate line (with the *x* values
coming from the index column, `iaMean`) labelled with the column name.
The basic Matplotlib interface cannot create such plot in one step. However,
the Pandas data frame itself has a plotting interface which knows how to
interpret the data, and produces the correct plot without much convincing:

```{.python .input}
aloha_pivot.plot.line()
plt.ylabel('channel utilization')
plt.show()
```


8. Interactive pivot tables
---------------------------

Getting the pivot table right is not always easy, so having a GUI where 
one can drag columns around and immediately see the result is definitely 
a blessing. Pivottable.js presents such a GUI inside a browser, and 
although the bulk of the code is Javascript, it has a Python frond-end 
that integrates nicely with Jupyter. Let's try it!

```{.python .input}
import pivottablejs as pj
pj.pivot_ui(scalars_wide)
```

An interactive panel containing the pivot table will appear. Here is how
you can reproduce the above "Channel utilization vs iaMean" plot in it:

1. Drag `numHosts` to the "rows" area of the pivot table.
   The table itself is the area on the left that initially only displays "Totals | 42", 
   and the "rows" area is the empty rectangle directly of left it. 
   The table should show have two columns (*numHosts* and *Totals*) and 
   five rows in total after dragging.
2. Drag `iaMean` to the "columns" area (above the table). Columns for each value
   of `iaMean` should appear in the table.
3. Near the top-left corner of the table, select *Average* from the combo box
   that originally displays *Count*, and select `ChannelUtilization:last` 
   from the combo box that appears below it.
4. In the top-left corner of the panel, select *Line Chart* from the combo box
   that originally displays *Table*.

If you can't get to see it, the following command will programmatically
configure the pivot table in the appropriate way:

```{.python .input}
pj.pivot_ui(scalars_wide, rows=['numHosts'], cols=['iaMean'], vals=['Aloha.server.channelUtilization:last'], aggregatorName='Average', rendererName='Line Chart')
```

If you want experiment with Excel's or LibreOffice's built-in pivot table 
functionality, the data frame's `to_clipboard()` and `to_csv()` methods
will help you transfer the data. For example, you can issue the 
`scalars_wide.to_clipboard()` command to put the data on the clipboard, then 
paste it into the spreadsheet. Alternatively, type `print(scalars_wide.to_csv())`
to print the data in CSV format that you can select and then copy/paste.
Or, use `scalars_wide.to_csv("scalars.csv")` to save the data into a file
which you can import.


9. Plotting histograms
----------------------

In this section we explore how to plot histograms recorded by the simulation.
Histograms are in rows that have `"histogram"` in the `type` column. 
Histogram bin edges and bin values (counts) are in the `binedges` and 
`binvalues` columns as NumPy array objects (`ndarray`).

Let us begin by selecting the histograms into a new data frame for convenience.

```{.python .input}
histograms = aloha[aloha.type=='histogram']
len(histograms)
```

We have 84 histograms. It makes no sense to plot so many histograms on one chart,
so let's just take one on them, and examine its content.

```{.python .input}
hist = histograms.iloc[0]  # the first histogram
hist.binedges, hist.binvalues
```

The easiest way to plot the histogram from these two arrays is to look at it
as a step function, and create a line plot with the appropriate drawing style.
The only caveat is that we need to add an extra `0` element to draw the right 
side of the last histogram bin.

```{.python .input}
plt.plot(hist.binedges, np.append(hist.binvalues, 0), drawstyle='steps-post')   # or maybe steps-mid, for integers
plt.show()
```

Another way to plot a recorded histogram is Matplotlib's `hist()` method, 
although that is a bit tricky. Instead of taking histogram data, `hist()` 
insists on computing the histogram itself from an array of values -- but we only
have the histogram, and not the data it was originally computed from.
Fortunately, `hist()` can accept a bin edges array, and another array as weights 
for the values. Thus, we can trick it into doing what we want by passing 
in our `binedges` array twice, once as bin edges and once as values, and 
specifying `binvalues` as weights.

```{.python .input}
plt.hist(bins=hist.binedges, x=hist.binedges[:-1], weights=hist.binvalues)
plt.show()
```

`hist()` has some interesting options. For example, we can change the plotting
style to be similar to a line plot by setting `histtype='step'`. To plot the
normalized version of the histogram, specify `normed=True` or `density=True` 
(they work differently; see the Matplotlib documentation for details). 
To draw the cumulative density function, also specify `cumulative=True`. 
The following plot shows the effect of some of these options.

```{.python .input}
plt.hist(bins=hist.binedges, x=hist.binedges[:-1], weights=hist.binvalues, histtype='step', normed=True)
plt.show()
```

To plot several histograms, we can iterate over the histograms and draw them
one by one on the same plot. The following code does that, and also adds a 
legend and adjusts the bounds of the x axis.

```{.python .input}
somehistograms = histograms[histograms.name == 'collisionLength:histogram'][:5]
for row in somehistograms.itertuples():
    plt.plot(row.binedges, np.append(row.binvalues, 0), drawstyle='steps-post')
plt.legend(somehistograms.module + "." + somehistograms.name)
plt.xlim(0, 0.5)
plt.show()
```

Note, however, that the legend contains the same string for all histograms, 
which is not very meaningful. We could improve that by including some 
characteristics of the simulation that generated them, i.e. the number of hosts
(`numHosts` iteration variable) and frame interarrival times (`iaTime` iteration
variable). We'll see in the next section how that can be achieved.


10. Adding iteration variables as columns
-----------------------------------------

In this step, we add the iteration variables associated with the simulation
run to the data frame as columns. There are several reasons why this is a
good idea: they are very useful for generating the legends for plots of
e.g. histograms and vectors (e.g. "collision multiplicity histogram for
numHosts=20 and iaMean=2s"), and often needed as chart input as well.

First, we select the iteration variables vars as a smaller data frame.

```{.python .input}
itervars_df = aloha.loc[aloha.type=='itervar', ['run', 'attrname', 'attrvalue']]
itervars_df.head()
```

We reshape the result by using the `pivot()` method. The following statement
will convert unique values in the `attrname` column into separate columns:
`iaMean` and `numHosts`. The new data frame will be indexed with the run id.

```{.python .input}
itervarspivot_df = itervars_df.pivot(index='run', columns='attrname', values='attrvalue')
itervarspivot_df.head()
```

Now, we only need to add the new columns back into the original dataframe, using
`merge()`. This operation is not quite unlike an SQL join of two tables on the
`run` column.

```{.python .input}
aloha2 = aloha.merge(itervarspivot_df, left_on='run', right_index=True, how='outer')
aloha2.head()
```

For plot legends, it is also useful to have a single `iterationvars` column with
string values like `numHosts=10, iaMean=2`. This is easier than the above: we
can just select the rows containing the run attribute named `iterationvars`
(it contains exactly the string we need), take only the `run` and `attrvalue`
columns, rename the `attrvalue` column to `iterationvars`, and then merge back the
result into the original data frame in a way we did above.

The selection and renaming step can be done as follows. (Note: we need
`.astype(str)` in the condition so that rows where `attrname` is not filled in
do not cause trouble.)

```{.python .input}
itervarscol_df = aloha.loc[(aloha.type=='runattr') & (aloha.attrname.astype(str)=='iterationvars'), ['run', 'attrvalue']]
itervarscol_df = itervarscol_df.rename(columns={'attrvalue': 'iterationvars'})
itervarscol_df.head()
```

In the merging step, we join the two tables (I mean, data frames) on the `run`
column:

```{.python .input}
aloha3 = aloha2.merge(itervarscol_df, left_on='run', right_on='run', how='outer')
aloha3.head()
```

To see the result of our work, let's try plotting the same histograms again,
this time with a proper legend:

```{.python .input}
histograms = aloha3[aloha3.type=='histogram']
somehistograms = histograms[histograms.name == 'collisionLength:histogram'][:5]
for row in somehistograms.itertuples():
    plt.plot(row.binedges, np.append(row.binvalues, 0), drawstyle='steps-post')
plt.title('collisionLength:histogram')
plt.legend(somehistograms.iterationvars)
plt.xlim(0, 0.5)
plt.show()
```


11. Plotting vectors
--------------------

This section deals with basic plotting of output vectors. Output vectors
are basically time series data, but values have timestamps instead
of being evenly spaced. Vectors are in rows that have `"vector"`
in the `type` column. The values and their timestamps are in the
`vecvalue` and `vectime` columns as NumPy array objects (`ndarray`).

We'll use a different data set for exploring output vector plotting, one from
the *routing* example simulation. There are pre-recorded result files in the
`samples/resultfiles/routing` directory; change into it in the terminal, and
issue the following command to convert them to CSV:

    scavetool x *.sca *.vec -o routing.csv

Then we read the the CSV file into a data frame in the same way we saw with the
*aloha* dataset:

```{.python .input}
routing = pd.read_csv('../routing/routing.csv', converters = {
    'attrvalue': parse_if_number,
    'binedges': parse_ndarray,
    'binvalues': parse_ndarray,
    'vectime': parse_ndarray,
    'vecvalue': parse_ndarray})
```

Let us begin by selecting the vectors into a new data frame for convenience.

```{.python .input}
vectors = routing[routing.type=='vector']
len(vectors)
```

Our data frame contains results from one run. To get some idea what vectors
we have, let's print the list unique vector names and module names:

```{.python .input}
vectors.name.unique(), vectors.module.unique()
```

A vector can be plotted on a line chart by simply passing the `vectime` and
`vecvalue` arrays to `plt.plot()`:

```{.python .input}
vec = vectors[vectors.name == 'qlen:vector'].iloc[4]  # take some vector
plt.plot(vec.vectime, vec.vecvalue, drawstyle='steps-post')
plt.xlim(0,100)
plt.show()
```

When several vectors need to be placed on the same plot, one can simply
use a `for` loop.

```{.python .input}
somevectors = vectors[vectors.name == 'qlen:vector'][:5]
for row in somevectors.itertuples():
    plt.plot(row.vectime, row.vecvalue, drawstyle='steps-post')
plt.title(somevectors.name.values[0])
plt.legend(somevectors.module)
plt.show()
```


12. Vector Filtering
--------------------

Plotting vectors "as is" is often not practical, as the result will be a crowded
plot that's difficult to draw conclusions from. To remedy that, one can apply
some kind of filtering before plotting, or plot a derived quantity such as the
integral, sum or running average instead of the original. Such things can easily
be achieved with the help of NumPy.

Vector time and value are already stored in the data frame as NumPy arrays
(`ndarray`), so we can apply NumPy functions to them. For example, let's
try `np.cumsum()` which computes cumulative sum:

```{.python .input}
x = np.array([8, 2, 1, 5, 7])
np.cumsum(x)
```

```{.python .input}
for row in somevectors.itertuples():
    plt.plot(row.vectime, np.cumsum(row.vecvalue))
plt.show()
```

Plotting cumulative sum against time might be useful e.g. for an output
vector where the simulation emits the packet length for each packet
that has arrived at its destination. There, the sum would represent
"total bytes received".

Plotting the count against time for the same output vector would
represent "number of packets received". For such a plot, we can utilize
`np.arange(1,n)` which simply returns the numbers 1, 2, .., n-1
as an array:

```{.python .input}
for row in somevectors.itertuples():
    plt.plot(row.vectime, np.arange(1, row.vecvalue.size+1), '.-', drawstyle='steps-post')
plt.xlim(0,5); plt.ylim(0,20)
plt.show()
```

Note that we changed the plotting style to "steps-post", so
that for any *t* time the plot accurately represents the number
of values whose timestamp is less than or equal to *t*.

As another warm-up exercise, let's plot the time interval
that elapses between adjacent values; that is, for each element
we want to plot the time difference between the that element
and the previous one.
This can be achieved by computing `t[1:] - t[:-1]`, which is the
elementwise subtraction of the `t` array and its shifted version.
Array indexing starts at 0, so `t[1:]` means "drop the first element".
Negative indices count from the end of the array, so `t[:-1]` means
"without the last element". The latter is necessary because the
sizes of the two arrays must match. or convenience, we encapsulate
the formula into a Python function:

```{.python .input}
def diff(t):
    return t[1:] - t[:-1]

# example
t = np.array([0.1, 1.5, 1.6, 2.0, 3.1])
diff(t)
```

We can now plot it. Note that as `diff()` makes the array one element
shorter, we need to write `row.vectime[1:]` to drop the first element
(it has no preceding element, so `diff()` cannot be computed for it.)
Also, we use dots for plotting instead of lines, as it makes more
sense here.

```{.python .input}
for row in somevectors.itertuples():
    plt.plot(row.vectime[1:], diff(row.vectime), 'o')
plt.xlim(0,100)
plt.show()
```

We now know enough NumPy to be able to write a function that computes
running average (a.k.a. "mean filter"). Let's try it out in a plot
immediately.

```{.python .input}
def running_avg(x):
    return np.cumsum(x) / np.arange(1, x.size + 1)

# example plot:
for row in somevectors.itertuples():
    plt.plot(row.vectime, running_avg(row.vecvalue))
plt.xlim(0,100)
plt.show()
```

For certain quantities such as queue length or on-off status,
weighted average (with time intervals used as weights) makes
more sense. Here is a function that computes running time-average:

```{.python .input}
def running_timeavg(t,x):
    dt = t[1:] - t[:-1]
    return np.cumsum(x[:-1] * dt) / t[1:]

# example plot:
for row in somevectors.itertuples():
    plt.plot(row.vectime[1:], running_timeavg(row.vectime, row.vecvalue))
plt.xlim(0,100)
plt.show()
```

Computing the integral of the vector as a step function is very similar
to the `running_timeavg()` function. (Note: Computing integral in other
ways is part of NumPy and SciPy, if you ever need it. For example,
`np.trapz(y,x)` computes integral using the trapezoidal rule.)

```{.python .input}
def integrate_steps(t,x):
    dt = t[1:] - t[:-1]
    return np.cumsum(x[:-1] * dt)

# example plot:
for row in somevectors.itertuples():
    plt.plot(row.vectime[1:], integrate_steps(row.vectime, row.vecvalue))
plt.show()
```

As the last example in this section, here is a function that computes
moving window average. It relies on the clever trick of subtracting
the cumulative sum of the original vector from its shifted version
to get the sum of values in every *N*-sized window.

```{.python .input}
def winavg(x, N):
    xpad = np.concatenate((np.zeros(N), x)) # pad with zeroes
    s = np.cumsum(xpad)
    ss = s[N:] - s[:-N]
    ss[N-1:] /= N
    ss[:N-1] /= np.arange(1, min(N-1,ss.size)+1)
    return ss

# example:
for row in somevectors.itertuples():
    plt.plot(row.vectime, winavg(row.vecvalue, 10))
plt.xlim(0,200)
plt.show()
```

You can find further hints for smoothing the plot of an output vector
in the signal processing chapter of the SciPy Cookbook (see References).


Resources
---------

The primary and authentic source of information on Pandas, Matplotlib and other
libraries is their official documentation. I do not link them here because they 
are trivial to find via Google. Instead, here is a random collection of other 
resources that I found useful while writing this tutorial (not counting all the 
StackOverflow pages I visited.)

- Pandas tutorial from Greg Reda: 
  http://www.gregreda.com/2013/10/26/working-with-pandas-dataframes/
- On reshaping data frames: 
  https://pandas.pydata.org/pandas-docs/stable/reshaping.html#reshaping
- Matplotlib tutorial of Nicolas P. Rougier:
  https://www.labri.fr/perso/nrougier/teaching/matplotlib/
- Creating boxplots with Matplotlib, from Bharat Bhole:
  http://blog.bharatbhole.com/creating-boxplots-with-matplotlib/
- SciPy Cookbook on signal smoothing: 
  http://scipy-cookbook.readthedocs.io/items/SignalSmooth.html
- Visual Guide on Pandas (video): 
  https://www.youtube.com/watch?v=9d5-Ti6onew
- Python Pandas Cookbook (videos):
  https://www.youtube.com/playlist?list=PLyBBc46Y6aAz54aOUgKXXyTcEmpMisAq3


Acknowledgements
----------------

I would like to thank the participants of the 2016 OMNeT++ Summit for the 
valuable feedback, and especially Dr Kyeong Soo (Joseph) Kim for bringing
my attention to Pandas and Jupyter.


Author
------

Andras Varga



