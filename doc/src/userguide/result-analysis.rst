

Analyzing the Results
=====================

Overview
--------

Analyzing the simulation result is a time consuming process. The result
of the simulation is recorded as scalar values, vector values,
histograms, and evaluated parameters. Information about each run, such
as iteration variables, configuration entries, and run attributes, are
also stored. You can then apply statistical methods to extract the
relevant information and to draw a conclusion. This process may include
several steps. Usually you need to filter and transform the data, and
chart the result. Automation is very important here - you do not want to
repeat the steps of recreating charts every time you rerun simulations.

In |omnet++| 6.x, the statistical analysis tool is integrated into the
Eclipse environment. Querying, processing and plotting the simulation
results is done with Python 3 scripts, using APIs provided by the IDE.
These scripts and APIs rely heavily on the NumPy, Pandas, and matplotlib
Python packages. Your scripts and settings (i.e. your recipe for finding
results from the raw data) will be recorded in analysis files (.anf) and
will become instantly reproducible. This means that if simulations need
to be rerun due to a model bug or misconfiguration, existing charts need
not be recreated all over again. Simply replacing the old result files
with the new ones will result in the charts being automatically
displayed with the new data.

When creating an analysis, the user first selects the input of the
analysis by specifying file names or file name patterns (e.g.
"adhoc-\*.vec"). Data in result files are tagged with meta information.
Experiment, measurement and replication labels are added to the result
files to make the filtering process easy. It is possible to create very
sophisticated filtering rules (e.g. all 802.11 retry counts of
host[5..10] in experiment X, where the iteration variable numHost had a
value of 7).


You can open charts from the selected data with one click. The Analysis
Editor supports bar charts, line charts, histogram charts and scatter
charts. Charts are interactive; users can zoom, scroll, and access
tooltips that give information about the data items.

Charts can be customized. Some of the customizable options include
titles, fonts, legends, grid lines, colors, line styles, and symbols.

The charts made in the IDE can also be viewed and exported later
from the command line, using a tool called ``opp_charttool``. Its
usage and operation is detailed in the Simulation Manual.

Creating Analysis Files
-----------------------

To create a new analysis file, choose :guilabel:`File \| New \| Analysis
File` from the menu. Select the folder for the new file and
enter the file name. Press :guilabel:`Finish` to create and
open an empty analysis file.

.. figure:: pictures/ANF-NewAnalysisFileDialog.png
   :alt: New Analysis File dialog

   New Analysis File dialog

There is also a quicker way to create an analysis file for a result
file. Just double-click on the result file in the :guilabel:`Project
Explorer View` to open the :guilabel:`New Analysis
File` dialog. The folder and file name is prefilled
according to the location and name of the result file. Press
:guilabel:`Finish` to create a new analysis file containing
the vector and scalar files whose names correspond to the result file.
If the name of the result file contains a numeric suffix (e.g.
``aloha-10.vec``), then all files with the same prefix will be added to
the analysis file (i.e. ``aloha-*.vec`` and ``aloha-*.sca``).

.. tip::

   If the analysis file already exists, double-clicking on the result
   file will open it.


Opening Older Analysis Files
----------------------------

The format of the Analysis files (``*.anf``) has
changed completely with |omnet++| 6.0. This means that older versions will
not be able to open analysis files created with this version at all.
This version does, however, attempt to open and convert analysis files
created by older versions. Keep in mind that this feature does not
work with all charts, and may generate incomplete, or different charts
than the originals, so manual tweaking may be necessary after the conversion.


Using the Analysis Editor
-------------------------

The usual workflow of result analysis consists of a few distinct steps.
These are: adding input files to the analysis, browsing simulation results
and selecting those of interest, creating a chart of an appropriate type from
the results, then viewing them as plots, and finally exporting data and/or
images if needed.

The Analysis Editor is implemented as a multi-page editor. What the editor
edits is the "recipe": what result files to take as inputs, and what kind of
charts to create from them. The pages (tabs on the bottom) of the editor
roughly correspond to some of the steps described above.

In the next sections, we will go through the individual pages of the editor,
and which analysis steps can be performed using them.

The Inputs Page
---------------

The first page displays the result files that serve as input for the
analysis. It contains a set of file name patterns that specify which files
to load. Under each pattern, the concrete list of matched files are shown
with their contents in a tree structure.

New input files can be added to the analysis by
dragging vector and scalar files from the :guilabel:`Project Explorer
View`, or by opening a dialog with the :guilabel:`New
Input...` button on the local toolbar. If the file name starts with '/,' it is
interpreted relative to the workspace root; otherwise, it is relative to
the folder of the analysis file.

.. figure:: pictures/ANF-InputsPage.png
   :alt: Specifying input files for data analysis

   Specifying input files for data analysis

The input files are loaded when the analysis file is opened.
If the files have changed on the disk (for example, because
a simulation was re-run), they can be reloaded
with the :guilabel:`Refresh Files` button on the toolbar.
This is intentional, so as not to make the IDE continuously
reload them during execution of large simulation campaigns.
After manual refreshing of files, the Charts are refreshed
automatically however.

Vector files are not loaded directly; instead, an index file (``*.vci``)
is created and the vector attributes (name, module, run, statistics, etc.) are loaded from the
index file. The index files are generated during the simulation, but can
be safely deleted without loss of information. If the index file is
missing or the vector file was modified, the IDE rebuilds the index in
the background.

.. tip::

   The :guilabel:`Progress View` displays the progress of the
   indexing process if it takes a long time.

The Browse Data Page
--------------------

The second page of the Analysis editor displays results (parameters,
scalars, histograms, and vectors) from all files in tables and lets the
user browse them. Results can be sorted and filtered. Simple filtering
is possible with combo boxes, or when that is not enough, the user can
write arbitrarily complex filters using a generic pattern-matching
expression language. Selected or filtered data can be immediately
plotted.

.. tip::

   You can switch between the :guilabel:`All`, :guilabel:`Parameters`,
   :guilabel:`Scalars`, :guilabel:`Histograms`, and :guilabel:`Vectors`
   pages using the underlined keys (Alt+KEY combination) or the
   Ctrl+PgUp and Ctrl+PgDown keys.

After pressing the :guilabel:`Filter Expression`
button, you can enter an arbitrary filter expression.
More details about the language used here are included later in this chapter.

.. figure:: pictures/ANF-BrowseDataPageAll.png
   :alt: Browsing all data generated by the simulation

   Browsing all data generated by the simulation

The :guilabel:`All` tab shows a tree containing all loaded result items.
The structure of this tree can be altered with the :guilabel:`Tree Levels`
and :guilabel:`Flat Module Tree` options on the local toolbar and in the
context menu.

The other tabs show tables containing the values and attributes of
all results of the given type. To hide or show table columns, open
:guilabel:`Choose table columns...` from the context menu and select
the columns to be displayed. The settings are persistent and applied
in each subsequently opened editor. The table rows can be sorted by
clicking on the column name.

.. figure:: pictures/ANF-BrowseDataPageTable.png
   :alt: Browsing result items generated by the simulation

   Browsing result items generated by the simulation

Individual fields of composite results (eg. the `:mean` and `:count` fields
of statistics, histograms or vectors) can also be included as scalars by
enabling the :guilabel: `Show Statistics / Vector Fields as Scalars` option.

You can display the selected data items on a chart. To open the chart,
choose one of the :guilabel:`Plot ...` items from the context menu,
or press Enter (double-click also works for single data lines).

When selecting a vector, its data can also be displayed in a table.
Make sure that the :guilabel:`Output Vector View` is opened. If it is
not open, you can open it from the context menu (:guilabel:`Show Output
Vector View`). This view always shows the contents of the selected vector.

.. tip::

   You can switch between the :guilabel:`Inputs`, :guilabel:`Browse Data`
   and :guilabel:`Charts` pages using the Alt+PgUp and Alt+PgDown keys.

Exporting Results
^^^^^^^^^^^^^^^^^

Selected results can be exported to files in different data formats
using the :guilabel:`Export Data` context menu option. After selecting
the data format, a dialog to select the output file and configure additional
exporting options is shown.

The Charts Page
---------------

The third page displays the charts created during the analysis.

This page works much like a usual graphical file manager. Each icon
represents a chart, and the charts can be selected, reordered by dragging,
copied, pasted, renamed, deleted, opened, or their context menu accessed.

.. figure:: pictures/ANF-ChartsPage.png
   :alt: Charts Page

   Charts Page


Associated Views
----------------

In addition to the main analysis editor, the IDE comes with a few
supplementary Views.

Outline View
^^^^^^^^^^^^

The :guilabel:`Outline View` shows an overview of the current
analysis. Clicking on an element will select the corresponding element
in the current editor. Tree editing operations also work in this view.

.. figure:: pictures/ANF-OutlineView.png
   :alt: Outline View of the analysis

   Outline View of the analysis

..
   TODO this is not really an added view, only the content provider...

Output Vector View
^^^^^^^^^^^^^^^^^^

The :guilabel:`Output Vector View` shows the content of the
selected vector. It displays the serial number, simulation time and
value of the data points. When event numbers are recorded during the
simulation, they are also displayed. Large output files are handled
efficiently; only the visible part of the vector is read from the disk.
To navigate to a specific line, use the scroll bar or the menu of the
view.

.. figure:: pictures/ANF-OutputVectorView.png
   :alt: Output Vector View with Context Menu

   Output Vector View with Context Menu

..
  TODO: properties view?


Working with Charts
-------------------

This section will walk you through how to work with chart,
starting from the very basics, all the way to more advanced
topics involving Python scripting.

Creating charts
^^^^^^^^^^^^^^^

Various types of charts are available. There are a number of
built-in ones, and you can also add your own to your projects.


From the Charts Page
~~~~~~~~~~~~~~~~~~~~

Right clicking in an empty area on the Charts page, and opening
the :guilabel:`New` submenu, lists all the available chart templates.
Clicking on one creates a new chart from that template.

The :guilabel:`New Chart` button on the toolbar opens a gallery-like
dialog, where more information (with a short description and some screenshots)
is shown about each of the chart templates. Selecting one and pressing
:guilabel:`OK` instantiates that template into a new chart.

Any chart created in any of these two ways will be initially empty,
as no result selection filter expression was configred for them yet.

From the Browse Data Page
~~~~~~~~~~~~~~~~~~~~~~~~~

Another way of creating new chart is by starting from a set of
simulation results, selected on the :guilabel:`Browse Data` page.

Right clicking on a selected set of results presents you
with a choice of chart templates, showing only those that accept
the given set of results as input.

The :guilabel:`Choose from Template Gallery` menu item shows
the same filtered list of templates in the gallery dialog.

Simply double-clicking on a result, or selecting some and pressing
Enter, will also open a suitable chart.

Charts opened this way are not saved into the analysis.
To add such a chart to the analysis, choose :guilabel:`Save
Chart` from the context menu of the chart or from the
toolbar on the chart's page.


Chart Types
~~~~~~~~~~~

The Analysis Tool offers two main ways of plotting: with Matplotlib,
or with the built-in ("native") plot widgets.
The former offers more functionality, and can draw basically anything;
while the latter is more performant but is only available
in three forms: bar chart, line chart, or histogram chart.


Using Charts
^^^^^^^^^^^^

This section introduces you to the basics of working with charts in the
|omnet++| IDE. It shows how to navigate on plots, how to configure their
appearance, and export data and images.

Navigation
~~~~~~~~~~

The mouse pointer can work in two different modes on plots. In Pan mode, you
can scroll with the mouse wheel and drag the chart. In Zoom mode, the user can
zoom in on the chart by left-clicking and zoom out by doing a
Shift +left+ click, or using the mouse wheel. Dragging selects a rectangular
area for zooming. The toolbar icons and switch between Pan and Zoom modes.
You can also find toolbar buttons to zoom in, zoom out and zoom to fit.
Zooming and moving actions are remembered in the navigation history.

The navigation of Matplotlib charts is slightly different from this,
as that follows how Matplotlib charts usually handle navigation.
One addition compared to that is that scrolling, Shift+scrolling and
Ctrl+scrolling pans vertically/horizontally, or zooms. There is also
a third mode, called interactive mode, which is used to manipulate
interactive elements on the plot, such as widgets, if present.

Properties Dialog
~~~~~~~~~~~~~~~~~

Charts have a set of properties that define their behaviour and looks.
These properties can be edited in a configuration dialog, accessible
from the :guilabel:`Configure Chart...` toolbar button and context menu item.

Each chart type has a different dialog layout, with the individual
properties categorized into tabs.

One common property (and configuration page) across all chart types is
the result filter expression that selects which data should the chart show.

.. figure:: pictures/ANF-ChartPropertiesDialog.png
   :alt: Chart Properties Dialog

   Chart Properties Dialog

For many input fields, autocompletion and smart suggestions are available
by pressing Ctrl+Space.

Filter Expressions
~~~~~~~~~~~~~~~~~~

A filter expression is composed of terms that can be combined with the `AND`,
`OR`, `NOT` operators, and parentheses. A term filters for the value of some property of
the item, and has the form `<property> =~ <pattern>`, or simply `<pattern>`. The latter
is equivalent to `name =~ <pattern>`.

The following properties are available:
 - `name`: Name of the result or item.
 - `module`: Full path of the result's module.
 - `type`: Type of the item. Value is one of: `scalar`, `vector`, `parameter`, `histogram`, `statistics`.
 - `isfield`: `true` is the item is a synthetic scalar that represents a field of statistic or a vector, `false` if not.
 - `file`: File name of the result or item.
 - `run`: Unique run ID of the run that contains the result or item.
 - `runattr:<name>`: Run attribute of the run that contains the result or item. Example: `runattr:measurement`.
 - `attr:<name>`: Attribute of the result. Example: `attr:unit`.
 - `itervar:<name>`: Iteration variable of the run that contains the result or item. Example: `itervar:numHosts`.
 - `config:<key>`: Configuration key of the run that contains the result or item. Example: `config:sim-time-limit`, `config:**.sendIaTime`.

Patterns may contain the following wildcards:
 - `?` matches any character except '.'
 - `*` matches zero or more characters except '.'
 - `**` matches zero or more characters (any character)
 - `{a-z}` matches a character in range a-z
 - `{^a-z}` matches a character not in range a-z
 - `{32..255}` any number (i.e. sequence of digits) in range 32..255 (e.g. `99`)
 - `[32..255]` any number in square brackets in range 32..255 (e.g. `[99]`)
 - `\\` takes away the special meaning of the subsequent character

Patterns only need to be surrounded with quotes if they contain whitespace or
other characters that would cause ambiguity in parsing the expression.

Example: `module =~ "**.host*" AND (name =~ "pkSent*" OR name =~ "pkRecvd*")`

.. tip::

   Content Assist is available in text fields where you can enter filter
   expressions, vector operations, run metadata selectors, advanced styling
   options, and similar. Press Ctrl+Space to get a list of appropriate
   suggestions at the cursor position.

Vector Operations
~~~~~~~~~~~~~~~~~

The charts that show vector results offer a selection of operations
to transform the data before plotting.

These can be added to the chart under the :guilabel:`Apply...` or
:guilabel:`Compute...` context menu items.
Both ways of adding operations compute new vectors from existing
ones. The difference between them is that Apply replaces the original
data with the computation result, while Compute keeps both.

Some operations have parameters that can be edited before adding it,
in the confirmation dialog that pops up after selecting the operation.

Most operations perform a fairly simple transformation on each individual
vector independently: summation, windowed average, etc. ``expression``
facilitates a more complex, freeform computation in one go. And two exceptions
are ``aggregate`` and ``merge``, as these operate on the entire DataFrame,
combining multiple vectors into one.

For example, see the screenshots illustrating the effects of the following
vector operations:

.. code-block::

  apply:sum
  apply:diffquot
  apply:movingavg(alpha=0.05)

.. figure:: pictures/ANF-VectorOperations-1.png
   :alt: Vector Operations - before

   Vector Operations - before

.. figure:: pictures/ANF-VectorOperations-2.png
   :alt: Vector Operations - after

   Vector Operations - after

See a description of all built-in vector operations in the Simulation Manual.

Exporting Data
~~~~~~~~~~~~~~

Both the input data used by a chart, and the final result after any processing,
can be exported.

The first one is essentially the same as the result exporting option on the
:guilabel:`Browse Data` page, except that it uses the result filter expression
of the given chart to select which results to export. This is available under the
:guilabel:`Export Chart Input As` context menu item of charts.

The second one includes any transformations the chart might perform on the data
before plotting it, and is available under the common :guilabel:`Export Chart...`
option, as discussed later.

..
  TODO: which ways support which data formats? (csv+json only, all that pandas has to offer)

Exporting Images
~~~~~~~~~~~~~~~~

You can copy the chart to the clipboard by selecting :guilabel:`Copy to
Clipboard` from the context menu. The chart is copied as a
bitmap image the same size as the chart on the screen,
taking the current navigation state into account.

The :guilabel:`Save Image` option saves the currently shown part
of the chart into a vector graphics (``.svg``) file.

Finally, the :guilabel:`Export Chart...` option opens the common
image/data exporting dialog (see later) for this chart only.

..
  TODO: which ways support which image formats? (png only, svg only, all that mpl has to offer)

Batch Export
~~~~~~~~~~~~

When exporting multiple charts, or when selecting the `Export Chart...`
option for a single chart, a common export dialog is opened.

.. figure:: pictures/ANF-ExportCharts.png
   :alt: Export Charts Dialog

   Export Charts Dialog


Some additional parameters of the images/data exported this way can
be configured on the :guilabel:`Export` tab of the property configuration
dialog of each chart.

Note that native charts exported this way will look a bit different than in the
IDE, because they will be drawn by Matplotlib during the export procedure.

This is also the way ``opp_charttool`` exports charts from the command line.

Chart Programming
^^^^^^^^^^^^^^^^^

All charts have a piece of Python 3 script at their hearts. To see or edit
this script, click the :guilabel:`Show Code Editor` button on the toolbar of
an open chart.

The property configuration dialog of each chart can also be modified. It is
made up of some number of pages (tabs), each of which is built as an XSWT form.
To see or edit the pages and forms within, click the :guilabel:`Edit Dialog Pages`
button on the property editor dialog.

Editing the Chart Script
~~~~~~~~~~~~~~~~~~~~~~~~

With the code editor open, you are free to make any changes to the
chart's script. It is automatically re-executed after each edit (with some
delay), if the auto-update button is enabled.
Otherwise you can refresh (execute) the chart manually by pressing
:guilabel:`Refresh` on the toolbar.

The output of the script (written to the standard output streams, by the print
function for example) is visible in the Console view.
Each chart has a console of its own in the view, which is activated when
switching to the chart's page in the editor.

The integrated editor is that of the PyDev project. It provides syntax
highlighting, code navigation (go to definition, etc.), helpful tooltips
(using docstrings), and content assist (completion suggestions).

.. figure:: pictures/ANF-ChartScriptEditor.png
   :alt: Chart Script Editor

   Chart Script Editor

Data processing in chart scripts is based on the NumPy and Pandas packages.
Refer to the Simulation Manual for details on the functionality provided
by the IDE and the API of the available Python modules.

Any uncaught errors are marked in the source code with a red squiggle and a sidebar
icon. Hover over them to see a tooltip describing the error. The errors
are also entered into the Problems view. Double clicking these problem
entries will reveal the line in the code editor where the error came from.

.. figure:: pictures/ANF-ChartScriptError.png
   :alt: A Python error is marked on the GUI

   A Python error is marked on the GUI

Editing Dialog Pages
~~~~~~~~~~~~~~~~~~~~

The Chart Dialog Pages editor lets you modify the
contents of the property configuration dialog of a chart.

You can add, remove, reorder and rename tabs; and edit the XSWT layout
of each tab. A preview for the tab layout being edited is also shown.

XSWT is an XML-based UI description language for SWT, the widget toolkit
of Eclipse on which the |omnet++| IDE is based. The content of XSWT files
closely mirror SWT widget trees.

.. figure:: pictures/ANF-EditChartPages.png
   :alt: Editing Chart Properties Editor Pages

   Editing Chart Properties Editor Pages

Attributes on widget nodes starting with ``x:id.`` serve special functions.
They either bind the contents of the widget to a chart property;
or provide a suggestion for the IDE about the kind of data shown in the
annotated widget, so it can help the user with featres like autocompletion;
or make the layout have a little dynamic behaviour (like checkboxes
disabling groups of widgets).

Reset to Template
~~~~~~~~~~~~~~~~~

In some cases, after making changes to a chart, it might prove necessary to
start fresh.

The :guilabel:`Reset to Template` option in the charts context menu
does lets you select some aspects of the chart to be restored to
its original state, as it is in its template: the code of the charts script,
the values of its properties, and/or the layout of its property editor dialog.

.. figure:: pictures/ANF-ResetChartToTemplate.png
   :alt: Resetting Chart to Template

   Resetting Chart to Template

Available Python Modules
~~~~~~~~~~~~~~~~~~~~~~~~

The chart scripts can access some functionality of the IDE through a couple
modules under the ``omnetpp.scave`` package.
These include: ``chart``, ``results``, ``plot``, ``vectorops``, and ``utils``.
The complete API of these modules is described in the Simulation Manual.

The ``chart`` module exposes information about the chart object (as
part of the analysis, and visible on the :guilabel:`Charts` page), most
importantly its set of properties, but also its name, and what type
of chart it is.

The ``results`` module provides access to the set of result items (and
corresponding metadata) currently loaded in the analysis in the IDE.
This data is accessible through a set of query functions, each taking
a filter expression, and returning a Pandas DataFrame.

The ``plot`` module implements a subset of the popular ``matplotlib.pyplot``
module, as a common interface to both the native widgets and matplotlib.

The ``vectorops`` module contains the implementations of the built-in vector
operations.

The ``utils`` module is a collection of helpful utility functions
that come in handy both for processing, and for plotting data.

Additionally, the well known ``numpy``, ``pandas``, ``matplotlib``, and
sometimes the ``scipy`` and ``seaborn`` packages are often utilized.
All other packages installed on the system are also fully available.

Factoring out Common Parts
~~~~~~~~~~~~~~~~~~~~~~~~~~

In a more complicated analysis, some custom functionality (either for data
processing or plotting) might prove useful in multiple charts. Instead of
duplicating this common code in the script of all charts that need it, it's
possible to import ``.py`` files placed next to the ``.anf`` file as modules
in the chart scripts. This also makes it possible to use external code editors
for authoring chart scripts.

Tips and Tricks, Common Pitfalls
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section is a collection of tips for use cases that might
come up often when working with charts, especially when editing
their scripts.

 - The order of items appearing in the legend, and in which they are drawn,
   is determined by their order in the data used for plotting. They can
   therefore be reordered by sorting the rows of the DataFrame before plotting it.
   For example: ``df = df.sort_values(by=['iaMean'])``

 - It's possible to add new data items to the queried results before plotting.
   These can be either computed from existing items, or synthesized from a
   formula. Example uses:

    - Computing derived results:

      ``df["bitrate"] = df["txBytes"] / df["sim-time-limit"]``

    - Adding analytical references, like theoretical values in an ideal scenario:

      ``df["analytical"] =  df["p"] * (1 - df["p"]) ** (df["N"]-1)``

    - Affecting how an item appears in the legend:

      ``df["comment"] = "iaMean=" + df["iaMean"]``

    - Summarizing results:

      ``df["mean"] = df["vecvalues"].map(np.mean)``

 - Instead of coming up with an elaborate filter expression, it is sometimes more
   straightforward to query results multiple times within a script, and combine
   them with ``pd.concat``, ``pd.join`` or ``pd.merge``. Other functions like
   ``pf.pivot`` and ``pd.pivot_table`` are also often useful in these cases.

 - You can define your own vector operations by injecting them into the ``vectorops`` module.
   Even if this injection is done in an external module (``.py`` file imported from the directory of the ``.anf`` file)

   .. code-block:: python3

      from omnetpp.scave import vectorops
      def myoperation(row, sigma):
        row["vecvalue"] = row["vecvalue"] + sigma
        return row
      vectorops.myoperation = myoperation

   After injection, use it like any other vector operation - on the :guilabel:`Input` page of Line Charts for example:
   ``apply: myoperation(sigma=4)``

 - If the built-in image/data exporting facilities are not sufficient for your
   use case, you can always add your own export code, either by manually
   ``open()``-ing a file, or by utilizing a data exporter library/function of
   your liking. Functions such as ``plt.savefig()`` and ``df.to_*()`` can be
   useful for this.

 - Since the entire chart script is executed on every chart refresh, even if only a visual property
   has changed, it can sometimes help to cache the result of some expensive data querying or processing
   procedure in the script. And because every execution is in a fresh Python process, this can only really
   be done on the disk. There are existing packages that can help you in this, such as ``diskcache``, ``cache.py`` or ``memozo``.
   The ``results.get_serial()`` function aids in properly invalidating caches built this way.

 - In charts using Matplotlib, the whole range of its functionality is available:

     - Arbitrary plots can be drawn (heatmaps, violin plots, geographical maps, 3D curves, etc.)
     - Advanced functionality like mouse event handlers, graphical effects, animations, and widgets, all works
     - It's also possible to just add small customizations, like annotations
     - Any extension library on top of Matplotlib can be used, such as: seaborn, ggplot, holoviews, plotnine, cartopy, geoplot
     - The built-in plotting capability of Pandas DataFrames (under ``df.plot``) works too

 - For native plots, properties affecting individual data items can be specified
   with the following additional syntax: ``<propertyname>/<itemkey>``.
   For example (on the Advanced tab in the property editor dialog of a line chart):
   ``Line.Color/2 : #FF0000``
   Unless overridden manually, the ``key`` of data items are sequentially
   increasing integers, starting with ``1``.

 - Even though PyDev offers a variety of tools for debugging Python scripts,
   these unfortunately don't work on chart scripts. Limited debugging can be
   performed using print statements, throwing exceptions, and dumping stack traces.
   If significant (bug-prone) data processing is required, it is a good idea to split it
   out into a standalone ``.py`` file, so it can also be imported, tested, and debugged
   by a separate IDE or interpreter.

 - Scripts and dialog pages can be shared by multiple chart templates, even across
   the matplotlib and native types, so it's possible to make them "portable",
   for example by not calling `matplotlib` functions directly.

 - The navigation state (view rectangle) is usually preserved after refreshing a chart.
   So if the displayed data changes significantly (because the script was changed, or
   a vector operation was added), it's possible that nothing will be visible after the
   automatic refresh. Push the :guilabel:`Home` button in these cases to bring all plotted elements into view.


Chart Templates
^^^^^^^^^^^^^^^

When charts are created, they are instantiated from a template.
The IDE offers a gallery to choose from the available templates,
showing a description and often also sample images for each.

.. figure:: pictures/ANF-ChartTemplateGallery.png
   :alt: Chart Template Gallery

   Chart Template Gallery

A chart template consists of several parts, describing the initial contents
of charts created from it: what kind of drawing widget it needs (Matplotlib or one of the native widgets),
what's the script it executes, how does its configuration dialog look like,
what properties it has, what types of result items it can process/show,
and which icon should be used for it.

..
  TODO: show a few built-in ones, incl. dialog options, what changes what?

Exporting a Chart as Template
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In addition to the built-in chart templates, projects can include custom ones.
The easiest way of creating a custom chart template is by customizing
a chart, then saving it as a template. The :guilabel:`Save as Template` option in a charts
context menu writes the contents of the given chart into several files under
the ``charttemplates`` directory in the project root, to be used as a template.

Per-Project Templates
~~~~~~~~~~~~~~~~~~~~~

Custom chart templates in the ``charttemplates`` directory of a project - or
of any referenced projects - are available for analyses in that project.

Under the Hood
^^^^^^^^^^^^^^

This section details the internal workings of the Python integration in the
Analysis Tool. Its contents are not directly useful for most users, only for
those who are curious about the technicalities, or want to troubleshoot an issue.

Chart scripts are executed by separate Python processes, launched from the
``python3[.exe]`` found in ``$PATH``. This decision was made so a rogue chart script
can't make the entire IDE unresponsive, or crash it. Also, it's possible to
put resource or permission constraints on these processes without hindering the
IDE itself, and can be killed at any time with no major consequences to the rest
of the Analysis Tool - for example, in the event of a deadlock or thrashing.

These processes are ephemeral, a fresh one is used for each refresh,
so no interpreter state preserved across executions. A small number of
processes are kept pre-spawned in a pool, so they can be put to use quickly
when needed.

If you wish to utilize virtual environments, start the entire IDE from a shell
in which the environment to use has been activated. This way the spawned Python
interpreter processes will also run in that environment.

The level of flexibility offered by this arbitrary scripting unfortunately
comes with its own dangers too. Note that the scripts running in charts have
full access to everything on your computer without any sandboxing, so they can
read/write/delete files, open graphical windows, make network connections,
utilize any hardware resources, etc.! Because of this, make sure to only ever
open analysis files from sources you trust! (Or files from untrusted sources
only on systems that are not critical.)

Communication between the Eclipse IDE and the spawned Python processes
is done via the Py4J project, through an ordinary network (TCP) socket.

To avoid the CPU and RAM inefficiencies caused by the string-based nature of
the Py4J protocol, bulk data is transferred in shared memory (POSIX SHM,
or unnamed file mappings on Windows) instead of the socket. Without this,
binary data would have to be base64 encoded, then represented as UTF-16,
which would be about 3x the size on top of the original content, which is
already present in both processes. Data passed this way includes any queried
results (in pickle format), and in the other direction, the data to plot on
native widgets, or the raw pixel data rendered by ``matplotlib``.

Many other kinds of information, like GUI events or smaller pieces of data
(like chart properties) are passed through the Py4J socket, as regular function
call parameters.
