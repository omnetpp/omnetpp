What's New
==========

This file summarizes OMNeT++ changes in each release. For changes related to
simulation model compatibility, see doc/API-Changes. For more detailed info
about all changes, see include/ChangeLog, src/*/ChangeLog, and ide/ChangeLog.


OMNeT++ 6.1.1 (January 2025)
----------------------------


OMNeT++ 6.1 (September 2024)
----------------------------

This update marks a significant refinement of OMNeT++ since version 6.0,
impacting virtually every aspect of the simulation framework, including the IDE
and simulation library. Among the various enhancements, the Analysis Tool stands
out with major improvements to both its user interface and its underlying chart
templates and Python library. For example, the new, highly customizable number
and quantity formatting options facilitate the interpretation of raw simulation
results displayed in the user interface; chart legends became much more
customizable; and the enhanced plotting of enum-valued vectors allows much more
appropriate visualization.

Additional notable updates include the ability to denote ini file sections as
"abstract" (meaning they merely serve as base for further configurations); the
validation of parameter values according to the `@enum` property; a new
bookmarking feature and other enhancements in the Qtenv log window; a more
streamlined operation of Project Features in the IDE; the possibility to have
simulation programs dump the stack trace in the case of crashes; a new Python
library for reading and querying NED files; and much more. Below is a detailed
summary of these key enhancements and other important changes in this release.

Important: Please read the installation instructions even if you are familiar
with OMNeT++, as there are new dependencies and the recommended way of
installing Python packages has also changed. There is a new `install.sh` script
in the root of the source distribution that will help you install OMNeT++ on
your machine.

Simulation kernel:

  - SimTime: Added multiple new constructors that accept a value and a unit,
    with one constructor implemented for each numeric type. Previously, there
    was only such constructor, for `int64_t`, which was a potential source of
    usage errors since C++ implicitly converts fractional types such as `double`
    into `int64_t` without issuing a warning about potential data loss.

  - Module parameters: Implemented `@enum` checking for `string` parameters. If
    the parameter is annotated with `@enum` in the NED file, assigning an
    unlisted value to it now triggers a runtime error.

  - Module parameters: It is now possible to assign XML data, produced using the
    `xml()` and `xmldoc()` functions, to NED `object` parameters. Implementing
    this feature necessitated some further changes, such as `cXMLElement` being
    made a subclass from `cOwnedObject` instead of `cObject`.

  - Module parameters: `cPar` has been updated to remember the exact file and
    line location in ini or NED files from which it was assigned. This
    facilitates troubleshooting issues associated with parameter assignments.

  - Introduced the `componentId()` NED function, which returns the ID of the
    current module or channel in context.

  - The `cEnum` class was enhanced to support C++11 features, including enum
    classes and enum base types. These changes may require minor adjustments in
    simulation models.

  - Added `Register_Enum_WithVar()` and `Register_Enum_Custom()` macros.
    `Register_Enum2()` was declared obsolete.

  - Result recorders: Enhanced to expand the `enumname` result attribute (which
    indicates that values are from a specified registered C++ enum) to the `enum`
    attribute that lists enum names alongside their corresponding numeric
    values. This update allows more appropriate visualization in the Analysis
    and Sequence Chart Tools in the IDE.

    For example, with the C++ code `enum LinkState {UP, DOWN, FAULTY};`
    registered as `Register_Enum(LinkState, (UP, DOWN, FAULTY))` and used in NED
    with `@statistic[linkState](record=vector; enumname=LinkState);`, the output
    vector file now also contains the `attr enum "UP=0, DOWN=1, FAULTY=2"` line
    in addition to `attr enumname LinkState`.

  - Ini files: Sections that only serve as a base for other configurations can
    now be marked as "abstract" by adding `abstract=true` to them. Abstract sections
    are not offered for running by Qtenv and the Simulation IDE.

  - Statistics recording: Disabled result recorders, as well as result filters
    whose outputs are unused or not recorded, are now removed before the
    simulation is run. This may result in significant performance and memory
    usage improvement.

  - Output Vector Recording: Empty vectors are now included in the output vector
    file. Previously, output vectors were only recorded into the `.vec` file
    when the first data item was written. As a result, vectors that were
    registered but never written to did not appear in the `.vec` file. This
    behavior caused uncertainty during simulation result analysis, as users
    could not distinguish between events (e.g., packet drops) that never
    occurred during the simulation and those that were simply not measured or
    recorded. Now, empty vectors are also written to the .vec file, eliminating
    this uncertainty and providing more complete information. This also fixes
    the issue that no `.vec` file was written by a simulation that only created
    empty vectors but never wrote data into them. The old behavior can be
    requested with the `vector-record-empty=false` configuration option, in case
    it is ever needed. Implementing this change necessitated some adjustments to
    the `cOutVector`, `cEnvir` and `cFingerprintCalculator` classes. Details can
    be found in the `API-changes.txt` and `ChangeLog` files.

  - Fingerprint: Resolved discrepancies in fingerprints caused by the IEEE
    not-a-number (NaN) having multiple representations, which led to
    inconsistencies across various CPU architectures and math library
    implementations.

  - Fingerprint: Resolved the issue that simulation results computed but not
    eventually recorded into the result files were inaccurately included in the
    "statistics" ingredient of the fingerprint.

  - Deprecated the `getAncestorPar()` method of `cComponent` because it violates
    the encapsulation principle, making its usage generally inadvisable.

  - Display strings: Improved the parsing of embedded expressions (`${...}`).

  - Added support for pretty-printed stack traces on crashes, which can be
    enabled by setting `WITH_BACKTRACE=yes` in `configure.user`. This feature is
    enabled by default to aid in more effective troubleshooting. By default,
    only function names and addresses are printed; to get enhanced stack frames
    with line numbers and source snippets on Linux, you must install
    `libdw-dev`, `libdw-devel`, or `elfutils`, depending on your package
    manager; see the Install Guide for specific instructions.

  - Added two new configuration options: `print-instantiated-ned-types` prints
    the names of all instantiated NED types at the end of the simulation,
    facilitating the creation of coverage tests for model libraries; and
    `print-unused-parameters` lists all NED parameters that have not been
    accessed during the simulation, pointing to possible deficiencies in the
    model.

  - Several smaller changes and improvements.

NED:

  - Allow inline properties in pattern assignments.
  - The excluded-packages list to use the same separator char as NED.
    path. Practically, it now also accepts `:` on non-Windows systems.
  - Fix "Network not found" errors when INET was in a symlinked directory.
  - Fix errors not being reported when a `@display` string literal had a badly
    escaped character (like "\,") in it.

MSG files, `opp_msgtool`:

  - Extended the parser so that enums defined in the msg file now may have properties.
  - Add support for generating C++11 enum classes and specifying the enum's
    underlying data type, using the `@class`, `@underlyingType` properties.
  - Fixed code generation error with targeted cplusplus blocks.

Qtenv:

  - Added "Set Bookmark" / "Go to Bookmark", "Go to simulation time" and "Go to
    event" functionalities to the log viewer, accessible via the context menu
    and keyboard shortcuts. Also improved the context menu by ensuring that it
    conveniently lists all available operations.

  - Log window search improvements: Improved the speed of text searches in the
    log window. Also, the "Find" dialog now displays an error message if the
    entered regular expression is invalid.

  - The Run Selection dialog now hides configurations not meant for running, as
    indicated by `abstract=true` in the ini file.

  - Improved animation in Fast mode: Keep showing builtin animations in Fast
    mode. Previously, when resuming normal Run mode after running in Fast mode,
    no animations were visible even when there were messages propagating through
    the network. With this change, this is no longer the case.

  - Several bug fixes.

Sequence Chart:

  - Refined toolbar contents.
  - Fixed several minor issues.

NED Editor:

  - Implemented "Snap to Grid". Grid size is fixed at 10 pixels.
  - Fix: Do not store coordinates with unnecessary precision into the display
    string after placing/moving a submodule with the mouse.
  - Fixed accumulation of rounding errors when painting the map grid.
  - Allow suppressing warnings by adding `@suppressWarning` property on an
    element.
  - Enhanced resolving the effective type of a submodule during NED analysis.
  - Remove superfluous error messages. If there is already an error message
    saying "No such module type", there is no point in spelling out the
    consequences as well.
  - Validate string param assignments to `@enum` properties. It is an error if the
    value is not one of the values listed in the parameter's `@enum` property.
  - Add autocomplete proposals for `@enum` in the text editor.
  - Converted the "Imported NED type not found" error into a warning. This
    change addresses scenarios where a compound module, defined in the NED file,
    conditionally uses a NED type from a currently disabled project feature.
    Although the unavailability of the NED type does not impact runtime when the
    simulation is configured correctly, the persistent "Imported NED type not
    found" error could not previously be eliminated. This change ensures smoother
    management of such issues.
  - Fixed an issue where rows were displayed at double height in the Module
    Hierarchy view due to extra newline characters in the displayed text.
  - Fixed an issue where the property sheet in the graphical NED editor
    disappeared after selecting an item.
  - Handle `${expressions}` in display strings better.

NED Documentation Generator:

  - In the documentation comment syntax, backtick quoting is now accepted to
    denote monospace. This is useful as we transition the documentation markup
    syntax to Markdown.

  - Added description column to the "Signals and Statistics" table. The
    description column is filled with the comment of the relevant NED element.

Ini File Editor:

  - Unknown configuration options are now flagged as warnings instead of errors.
    This change is made because the editor cannot determine whether an unknown
    option is a typo or a legitimate custom option registered in the model's C++
    code that the IDE does not recognize. To eliminate even these warnings, add
    the custom option to the list of known custom options in Preferences ->
    OMNeT++ -> Ini File Editor.

  - Added support for "abstract" inifile sections, as denoted by `abstract=true`.
    Abstract sections are displayed with an empty (outline-only) folder icon.

  - Update for recently added configuration options, and general bug fixing.

Analysis Tool:

  - Introduced quantity formatting to improve the display of numeric simulation
    results, whether recorded with or without measurement units. Options include
    controlling the number of significant digits, rounding, regular/scientific
    notation, choice of decimal separator, choice of preferred unit (e.g., bits
    vs bytes, mW vs dBm), and more. This feature allows results to be shown in
    the most appropriate format. Multiple display formats can be defined and
    applied in specific contexts using filter expressions.

  - Browse Data page: Enhanced various aspects of data tables including adding
    syntax highlighting of parameter values on the Parameters tab, a new context
    menu in the header row for adjusting column alignment and width, new options
    to hide or dim less important details such as network name and result
    suffix, and adding the IsWeighted column to pages listing statistics and
    histograms.

  - In result queries, enhanced the syntax of filter expressions in the chart
    Input pages to allow comment lines starting with `#`. This addition enables
    users to comment out lines from multi-line expressions, improving clarity
    and manageability.

  - Improve the way the "Plot vectors on separate axes" chart template handles
    enum-valued vectors. Such vectors are drawn with colored rectangles, with
    the value name in each. It requires the `enum` attr to be present on the
    vector.

  - Since output vectors can now be empty, a new option was added to relevant
    charts to leave out empty vectors from the plot.

  - Added support for ordering the series in charts, affecting both the chart
    presentation and the legend.

  - When creating a new analysis from a `.sca` or `.vec` file, use relative
    paths in the `.anf` file if practical.

  - Introduced the "Compare Chart to Template" functionality, which is
    particularly useful for porting charts created with earlier versions of
    OMNeT++ to the current version, ensuring compatibility and ease of update.

  - Miscellaneous smaller enhancements, such as assigning numbered names to new
    charts, adding customization hints to "Lines," "Bars," "Histograms" dialog
    pages, updating icons for tree nodes like parameter, field, and attribute,
    marking temporary charts with asterisks in chart tab labels for clearer
    identification, and also accepting `-->` in legend label replacements for
    improved readability.

  - Various bug fixes, such as the Properties view not updating under some
    circumstances, the "Sum weights" column displaying an error in the
    Histograms tab for weighted histograms, etc.

  - A large number of improvements and bug fixes in the Python library
    underlying the chart scripts, see the list of changes to the `omnetpp.scave`
    package below.

Project Features:

  - Introduced classifications for dependencies within project features,
    differentiating between "required" and "recommended" dependencies to clarify
    essential and suggested configurations. Previously, all dependencies were
    treated as "required".

  - Force C++ and NED exclusions of a project to be fully determined by which
    project features are enabled/disabled. This means, among others, that
    settings on the NED Source Folders property page are now uneditable in the
    presence of Project Features.

  - Improved the wording and behavior of the Project Features, Makemake
    and NED Source Folders project property pages.

  - Reduce the chance of race conditions associated with the IDE updating
    the `features.h` file.

IDE General:

  - When creating new launch configurations, the IDE now uses the `opp_dbgmi`
    command to launch the debugger. `opp_dbgmi` is a new (internally used)
    command that launches the `lldbmi2` debugger if present (currently only on
    macOS) or falls back to use `gdb` (on Linux and Windows). The bundled
    `lldbmi2` debugger is built from https://github.com/omnetpp/lldbmi2, a
    fork of Didier Bertrand's lldbmi2 project with numerous enhancements from
    the OMNeT++ team.

  - Added support for executing a BeanShell script on startup
    (`.metadata/startup.bsh` ), which can be used for auto-importing projects
    and similar tasks.

  - Upgrade to Eclipse 2024-09 (4.33), PyDev 12.2, CDT 11.6.

Python library:

  - Added a `requirements.txt` file that holds the list of dependencies.
    They can be installed with the following command:
    `python3 -m pip install -r python/requirements.txt`

  - Added `omnetpp.ned`, an experimental package that allows loading and
    browsing NED types from Python. The rest of the changes affect the
    `omnetpp.scave` package, see below.

Scave Python library (`omnetpp.scave` package):

  - Refined result query and plotting functions to ensure accuracy based on the
    measurement units of simulation results. The `unit` attribute is now
    consistently included in the returned dataframe, with options to convert
    quantities to the base unit using the `convert_to_base_unit` parameter in
    `results.get_[scalars|vectors|statistics|histograms]()` (default is `True`).
    Additionally, plotting functions now mandate uniform units across results,
    reporting errors for discrepancies. A utility function, `convert_to_base_unit(df)`,
    has also been added to `utils.py` for easier conversions.

  - When `results.get_[scalars|vectors|statistics|histograms]()` is used outside
    the IDE, e.g. as part of `opp_charttool`, results are now loaded directly
    through the C++ scave library instead of via converting to CSV with
    `opp_scavetool`. This results in improved reliability and performance gains.

  - In result queries, enhanced the syntax of filter expressions to allow
    comment lines starting with `#`. This addition enables users to comment out
    lines from multi-line expressions, improving clarity and manageability.

  - Since empty output vectors are now also included in the output vector files
    by default, the `omit_empty_vectors` option was added to `result.get_vectors()`
    so that the user can filter them out when that's desirable.

  - Enhanced `plot_vectors_separate()` in `utils.py` to plot enums as colored
    strips.

  - Added `add_legend_labels()`, `sort_rows_by_legend()` to `utils.py`.

  - Added a `createdWith` attribute to charts in ANF files. This attribute
    records the OMNeT++ version used to create the chart, aiming to simplify the
    process of porting user modifications in chart scripts to newer versions of
    the chart template.

  - Raise minimum required Matplotlib version to 3.5.3.

  - Many more fixes and improvements. For the full list, see `python/ChangeLog`.

`opp_featuretool`:

  - Added support for the "recommended" project features in addition to
    mandatory dependencies. The `-f`, `--with-dependencies` command-line option
    now includes both mandatory and recommended features; the new `-r`,
    `--with-required` option only refers to required features.
  - More helpful message if user specifies a nonexistent feature on the command
    line ("Did you mean X?")

`opp_charttool`:

  - Result loading now takes place via calling into the native scave library,
    instead of invoking `opp_scavetool`. This significantly improves performance.
  - Added a positional argument to select charts either by index or name.
  - Only print the "exporting..." message in verbose mode (`-v`).
  - Use logging instead of print statements where appropriate. The log level can
    be controlled with the new `-l`,`--log-level` option.
  - Several further refinements.

`opp_makemake`:

  - Ported from Perl to Python. Surprisingly, this also brought significant
    performance improvement.

Build:

  - If a Python virtual environment named `.venv` is present in the OMNeT++ root
    directory (created by executing `python3 -m venv .venv`), sourcing the `setenv`
    script automatically activates that environment. This allows OMNeT++ to use its
    own private Python virtual environment, avoiding possible version conflicts with
    system packages. Python dependencies are defined now formally in the
    `python/requirements.txt` file so you can easily install all the dependencies with
    `python3 -m pip install -r python/requirements.txt`.

Documentation:

  - Install Guide updated.
  - Users Guide: Result Analysis chapter significantly revised and expanded.
    Added many more details on how charts operate and how to work with them
    effectively.
  - Reduced backlog for the Simulation Manual: object parameters, transmission
    updates, clarified volatile, list of supported NED properties, etc.
  - Fresh style for the OMNeT++ API documentation.
  - More proofreading and refinements.


OMNeT++ 6.0.3 (January 2024)
----------------------------

This is a maintenance release, with several performance improvements
in the simulation kernel and further adjustments.

IDE:

  - Updated to Eclipse 4.30, CDT 11.4, Pydev 11.0
  - Model installation dialogs: If the IDE was installed from opp_env,
    tell the user to install the model with opp_env, too.


OMNeT++ 6.0.2 (October 2023)
----------------------------

This is a maintenance release with dozens of bug fixes.

Configure and setenv:

  - `configure` now mandates having `setenv` sourced before it runs.
  - `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
    frameworks from accidentally making OMNeT++ bundled icons unaccessible
    from Qtenv by simply appending to `OMNETPP_IMAGE_PATH`.
  - `configure` now creates `configure.user` from `configure.user.dist`
    if it does not already exist.

Simulation kernel:

  - Improved overflow checking in SimTime `operator=()` methods
  - Better commenting of `cGate::connectTo()` and `disconnect()`, and some other methods
  - Fix in `ConstantFilter` to return the constant value from `getInitialDoubleValue()`
  - Fix for missing initialization of some result filters
  - Fix in `cDatarateChannel` to clear finished transmissions from txList
  - Fix for the issue where `intuniform()` had a broken bounds check
  - Disallowing changes to arrival time and scheduling priority in `cEvent` when scheduled
  - Fix for startup error complaining about arg count of `hypot()`
  - Addition of `cEventHeap::checkHeap()` for potential FES debugging
  - Introduction of a `panic()` function and `ASSERT_DTOR()` for throwing from destructors
  - Fix for warnings in the code

IDE:

  - IDE update to Tycho 2.7.5, Eclipse 4.29, CDT 11.3, and removal of ansiconsole plugin
  - Error message display improvement in Simulation Model Installer
  - Checking for dangling project references in IDE
  - Workaround for "Unhandled event loop" exceptions in Inifile Editor
  - Fix for "String index out of range: 0" in Inifile Editor
  - Detection of duplicate submodule names in NED
  - Compatibility with Matplotlib 3.8 in Analysis Tool
  - Improved error/warning message display in Analysis Tool
  - Adjustments to zoom and legend behavior in native plots in Analysis Tool
  - Preselection of all temp charts in "Save On Close" dialog in Analysis Tool
  - Support for key events in Matplotlib for x/y navigation modes in Analysis Tool
  - Fix leaking cursor resources in Analysis Tool
  - Workaround for display glitch in Analysis Tool's "Choose table columns" dialog
  - Periodic reload of user's chart templates in Analysis Tool
  - Improved zoom behavior in native plots in Analysis Tool
  - Trimming from left instead of right for right-aligned table columns in Analysis Tool
  - Parameter type display improvement in Analysis Tool
  - Support for displaying vectors with bool data type in Analysis Tool
  - Workaround for crash in SWT table and tree widgets in Analysis Tool
  - Ensure reading all NED files in NED Documentation Generator
  - Resolution of unresolved module interface in NED Documentation Generator
  - Fix for unreadable text in dark theme in generated NED source listing
  - Tooltip content improvements for submodules in generated NED documentation
  - Default navigation target changes for submodule types in generated NED documentation
  - Fix for bug #939 in Sequence Chart

Qtenv:

  - Error handling improvement in the Object Inspector
  - Focus adjustment in the Run Selection dialog
  - Purge message animations when their messages are rotated out of the LogBuffer
  - Fix simulations stopping spontaneously when running in FAST mode
  - Message animations made more tolerant of degeneration connections
  - Clamp non-zero computed animation speed to at least 1e-9
  - Proper rendering of degenerate connections
  - Fix for missing logs with disabled event/initialization banners
  - Fix for non-working parameter replacements in channel display strings
  - Fix assertion failure on stopping eventlog recording
  - Fix for includes in osg.msg
  - Refactoring for code clarity, warning elimination

Python (omnetpp.scave.* packages):

  - Use of pandas.concat instead of deprecated DataFrame.append
  - Fix to hide legend in Matplotlib plots if it was empty
  - Fixed division by zero warnings in timeavg filter
  - Fix to prevent turning scalars of value 0 into NaNs
  - Added missing docstring in analysis.py of omnetpp/scave
  - Fixed a subtle bug related to mutable objects as default values
  - Look for projects by real name instead of directory name
  - Improved treatment of workspace_dir and error handling in get_project_location()
  - Fixed error if a renamed project is not directly under the Eclipse workspace directory
  - Improved error message for project not found errors
  - Reload mpl and plt before exporting an image for reproducibility in opp_charttool
  - Added "python" folder of referenced projects to python path in opp_charttool

Other:

  - Fix wrong bounding box for Graphviz-generated images in the manual
  - Fix stray LaTeX comments showing up in the HTML version of the manual
  - Allow running tests in other than "debug" mode
  - Fix for bug #1055 during Windows builds


OMNeT++ 6.0.1 (September 2022)
------------------------------

This is a maintenance release containing several bugfixes.

Simulation kernel:

  - Fix: Proper namespace resolution for C++ types in `@signal` declarations.
  - Added `cProperty::getValueOriginFile()`, `getValueOriginType()`.
  - Fix: Build errors on newer versions of clang.
  - Fix #933: eventlog recording intervals were ignored.
  - Fix: Check for context component in all built-in NED functions that need it
    and throw an exception if there is none, instead of just crashing.

IDE:

  - CDT component upgraded to 10.6.2
  - Fix #938: Filtering the eventlog in the sequence chart makes it empty.
  - Fix #896: NED visual editor cannot show warning symbol.
  - Fix #634: NED files in search results now open properly in NED editor.
  - Fix #932: Exception when the filter button was clicked on the Sequence
    Chart window.
  - Fix #936: Illegible text on build output console in dark mode.

Analysis Tool:

  - Various fixes in the Python analysis package.
  - Added the `python/` folders of referenced project(s) to Python path of chart scripts.
  - On Windows, the bundled Python was upgraded to 3.10, fixing several issues
    in NumPy and Matplotlib.

NED Documentation Generator:

  - Refined support for msg types with namespace.
  - Added support for "@debug links" page directive.
  - Made namespaces appear in the navigation tree.

Qtenv:

  - Fix: Non-working parameter replacements in channel display strings.
  - Fix: Missing logs with disabled event/initialization banners.
  - Fix #934: Eliminate assertion failure on stopping eventlog recording from Qtenv.
  - Fix: Clamp nonzero computed animation speed to at least 1e-9. (Very low
    animation speed could result in simulation time not progressing at all,
    due to finite precision in the computations.)
  - Various other minor bug fixes.

Documentation:

  - Minor corrections in the Installation Guide.
  - Updated documentation for the `omnetpp.scave` Python classes.
  - Document fingerprint ingredients added in 6.0.

See ChangeLogs in individual folders for details.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A6.0.1


OMNeT++ 6.0 (April 2022)
------------------------

OMNeT++ 6.0 is the result of more than three years of work, and includes many
essential new features that we would already have a hard time without. The
present changelog summarizes all changes made during the 15+ pre-releases.

We briefly summarize the changes below in each part of OMNeT++ before going into
the details.

The most prominent new feature is the new Python-based Analysis Tool in the IDE.
The use of Python under the hood allows for arbitrarily complex computations to
be performed on the data, visualizing the result in the most appropriate form
chosen from a multitude of plot types, and producing publication quality output,
all while using an intuitive user interface that makes straightforward tasks
easy and convenient. Custom computations and custom plots are also easily
accessible. The tool is able to handle large quantities of data. The Python APIs
are also available outside the IDE (e.g. for standalone scripts), and a
command-line tool for viewing and exporting charts created in the IDE also
exists (`opp_charttool`).

The NED language now supports parameters that carry C++ objects as values (type
`object`), which can be used to parameterize modules with structured data
(e.g. nontrivial configuration), packet prototypes, function objects, etc.
Structured data may come from NED functions like `readCSV()` or `readJSON()`
which parse data files, or may be specified directly in NED or ini files using
JSON syntax. The syntax of ini files has even been adjusted to make it more
convenient to write multi-line JSON values in it. Further new functionality
includes the string match operator `=~`, the "spaceship" operator `<=>`, and
support for Universal Function Call Syntax (UFCS). Incompatible changes include
the change in the interpretation of parameter names that are not qualified with
the `this` or `parent` keywords, and the necessity to mark module parameters
with `@mutable` that are allowed to be set at runtime. Embedding NED files into
simulation binaries for easier dissemination has also become possible.

Message descriptions (msg files) have undergone even bigger changes. An import
system has been added to make the content of a msg file available in others. The
generated code and class descriptors can now be widely customized via
properties. Targeted C++ blocks have been introduced for injecting C++ code into
various places in the generated source files. Altogether, these (and further,
smaller) features facilitate writing significantly cleaner msg files, especially
in large projects like INET.

The format of ini files have been made more flexible: the `Config` word in
section headers is now optional, and long lines can be broken up to multiple
lines without using trailing backslashes (just indent the continuation lines).

In the simulation kernel, the most important change is the introduction of the
Transmission Updates API, which allows in-progress packet (frame) transmissions
to be modified, i.e. aborted, shortened, or extended. This API is necessary for
implementing L2 features like frame preemption or on-the-fly frame aggregation.
Other changes include the addition of utility APIs like `scheduleAfter()`,
`rescheduleAt()` and `rescheduleAfter()`, refinements around module deletion and
the introduction of the `preDelete()` virtual member function, refinements in
the signals and listeners APIs, improved expression evaluation support, and the
addition of string-handling utility functions, just to name a few.

Regarding statistics recording, perhaps the most significant addition is the
`demux` filter, which allows splitting a single stream (of emitted values) to
multiple streams. The filter makes it possible to record statistics subdivided
by some criteria, e.g. to record statistics per TCP connection, per remote IP
address, per traffic class, etc., in INET. Further improvements include the
addition of the `warmup` filter and the `autoWarmupFilter` statistic attribute
that allow computed statistics to be calculated correctly also in the presence
of a nonzero warm-up period. Result files now hold more metadata: parameter
values and configuration entries are now also (optionally) recorded. This
change, together with other, smaller adjustments, cause new result files not be
understood by previous versions of OMNeT++.

A significant amount of work has been put into improving the looks and
functionality of Qtenv: material-style icons, HIDPI support, dark theme support,
and countless small features that improve usability, especially around the
Object Inspector and the Log View. For example, it is now possible to set the
Messages view of the Log to display all timestamps as a delta to a
user-specified reference timestamp; or, the Object Inspector now allows
selecting a different display mode for a tree node. One important change is
that method call animations are now disabled by default.

The Sequence Chart tool in the IDE has been significantly improved, both
visually and functionally, with the goal of allowing the user to configure the
chart in a way that facilitates understanding of the sequence of events in the
simulation. The tools provided for that purpose are the possibility to hide
unnecessary elements (unrelated arrows, events, etc), support for user-defined
coloring, interactively collapsible compound module axes, horizontally expanded
events so that the arrows of nested method calls do not overlap, and more. The
eventlog file format has also changed in a non-backward-compatible way, due to
the addition of extra elements that allow the Sequence Chart tool to be faster
and more scalable.

Generating documentation from NED and MSG files has been made possible from the
command line as well, using the new `opp_neddoc` tool. Functionality has been
extended with the possibility of incorporating external information into the
generated pages.

The C++ debugging experience has been made more pleasant in several ways. For
example, the "Debug on Error" and "Debug Next Event" functionality in Qtenv may
now invoke the integrated debugger of the Simulation IDE, which is especially
useful when the simulation was launched from the IDE. The User Guide also
contains a hint on how to configure simulations to invoke VS Code as debugger.

Due to improvements in the toolchain and the build process, Windows users
may see the linking time of large models like INET Framework to drop
dramatically (1-2 orders magnitude, e.g. from several minutes to several
seconds). On macOS, Apple Silicon, is currently supported with x86-64 emulation.

Now, the details:

NED:

  - Semantics change: Within a submodule's or connection's curly brace block,
    the interpretation of parameter, gate, and submodule references that don't
    use an explicit `parent` or `this` qualifier has changed. They no longer
    refer to the enclosing compound module's item, but to the item of (or
    within) the local submodule or channel object. The interpretation of item
    references outside subcomponent blocks has remained unchanged. An example:

        network Network {
          parameters:
              int foo;
          submodules:
            node1: { ... }
            node2: Node {
              foo = foo; // ERROR: self-reference! Change to 'foo=parent.foo'.
              bar = this.foo;  // OK
              baz = node1.foo;  // ERROR: refers to yet-uncreated submodule node2.node1!
              bax = parent.node1.foo;  // OK
            }
        }

    The set of forms accepted by `exists()` and `sizeof()` has been expanded.
    Notably, `exists()` can now be used to check the existence of an item in a
    submodule vector (as submodule vectors may now contain holes, due to
    `@omittedTypename`). Also, the `index()` syntax has been removed, `index`
    is only accepted without parens.

    Note that referencing a submodule's submodule from NED (i.e. the `a.b` or
    `this.a.b` syntax within a submodule block) is generally wrong, because the
    network is built top-down, and submodules are only created after parameters
    have already been set up. (An exception is using it in the values of
    `volatile` parameters, because they are evaluated later.)

    Error messages have been revised and expanded to be more helpful and
    facilitate porting NED code from 5.x.

    To write NED which is compatible with both OMNeT++ version 5.x and 6.0,
    qualify all references within subcomponent blocks with explicit `parent`
    or `this`, and require OMNeT++ 5.7 which is the first and only 5.x version
    that understands the `parent` keyword.

  - NED grammar: Added `object` as parameter type. Parameters of this type may
    hold objects subclassed from `cOwnedObject` that they can take ownership of.

  - NED parameters of type `object` may have a `@class()` property, with
    the syntax `@class(classname)` or `@class(classname?)`. The property
    specifies that the parameter should only accept objects of the given type
    and its subclasses. The referenced class must be registered via
    `Register_Class()` or `Register_Abstract_Class()`. Parameters declared
    without the question mark in `@class()` don't accept `nullptr` as value,
    while the ones with question mark do.

  - The NED expression syntax has been extended to accept JSON-style constructs,
    i.e. arrays and dictionaries (termed "object" in JSON), and the `nullptr`
    keyword. The array syntax is a list of values enclosed in square brackets,
    for example `[1, 2, 3]`, and the array is accessible as a `cValueArray`
    object in C++. The dictionary syntax uses curly braces:
    `{"foo" : 1, "bar" : 2 }`, and dictionaries are presented as `cValueMap`
    objects. If a dictionary is prefixed by a name, then the name is
    interpreted as a class name, and values are interpreted as fields of the object:
    `cMessage { name: "hello", kind: 1}`.

  - Object parameters allow simple values (int, double, bool, string) as well.
    C++-wise, they are stored in the parameter in `cValueHolder`-wrapped
    `cValue` objects.

  - New NED functions have been introduced: `get()` (return an element of an
    array or dictionary), `size()` (returns the size of an array or dictionary),
    `eval()` (evaluates a NED expression given as string), `dup()` (clones an
    object). `dup()` simply calls an object's C++ `dup()` method; it is useful
    because module parameters of type `object` only accept objects they fully
    own, which can be worked around using `dup()`. An example:

      object a = [1,2,3];
      object b = dup(a);  // without dup() it is an error: value is owned by parameter 'a'

    As `dup()` invoked on containers only duplicates the objects owned by the
    container, you may need extra `dup()` call when referring to objects owned
    by other parameters. Example:

        object a = {};
        object b = dup([a,a]);  // ERROR (a's inside the array are not cloned)
        object c = [dup(a), dup(a)]  // OK

  - NED grammar: Made operator precedence more similar to C/C++.
    Relational operators (`==, !=, <, <=, >, >=`) used to be on the same
    precedence level; now `==` and `!=` have lower precedence than the rest.

  - NED grammar: Added the `=~` (match) and `<=>` (comparison, a.k.a.
    "spaceship") operators, the `undefined` keyword, and `bool()` as
    conversion function.

  - String constants are now accepted with apostrophes too. This makes it
    possible to write quotation marks in strings without the need to escape
    them.

  - For quantities (numbers with measurement units), the rules have changed
    slightly. When specifying quantities with multiple terms, a minus sign is
    only accepted at the front, to reduce the chance of confusion in arithmetic
    expressions. I.e. `-5s100ms` is valid as a quantity string (and means
    -5.1s), but `5s-100ms` is no longer.

    Also, the plain `0` constant is no longer accepted at places where a
    quantity with a measurement unit is expected. Reason: it was confusing when
    the expected unit was dB or dBm. E.g. in `omnetpp.ini`, should `**.power = 0`
    be interpreted as 0W or 0dBm?

  - Added support for Universal Function Call Syntax (UFCS) to NED, which means
    that now any function call may also be written as if the function was a
    method of its first argument (provided it has one). That is, `f(x,..)` can now
    be also written in the alternative form `x.f(...)`. This results in improved
    readability in certain cases, and allows chaining function calls.

  - `xmldoc()` (and other file-reading functions) now interpret the file name as
    relative to the directory the expression comes from. When `xmldoc()` occurs
    in an included ini file, the file name is now interpreted as relative to the
    directory containing the included ini file (as opposed to being relative to
    the main ini file or the working directory.) If `xmldoc()` occurs in a NED
    file, the file name is relative to the directory where that NED file was
    loaded from.

  - NED functions for reading (from file) and parsing (from string) of CSV,
    JSON and XML files: `parseCSV()`, `readCSV()`, `parseExtendedCSV()`,
    `readExtendedCSV()`, `parseJSON()`, `readJSON()`, `parseExtendedJSON()`,
    `readExtendedJSON()`, `parseXML()`, `readXML()`. The "extended" variants support
    expressions in the file, which will be evaluated during parsing. The XML
    functions are aliases to the existing `xml()`/`xmldoc()` functions. Bonus
    file-related functions: `readFile()`, `workingDir()`, `baseDir()`, `resolveFile()`,
    `absFilePath()`.

  - The body of a parametric submodule now allows assigning apparently
    nonexistent parameters. For example, in the example below, the `sleepTime`
    assignment does not cause an error if `FooApp` has a `sleepTime` parameter
    but `IApp` does not:

        app: <default("FooApp")> like IApp {
            parameters:
                address = parent.address;
                sleepTime = 1s;  // ignored if app has no 'sleepTime' parameter
        }

  - Implemented `@omittedTypename` property. `@omittedTypename` allows one to
    specify a NED type to use when `typename=""` is specified for a parametric
    submodule or channel. `@omittedTypename` can be specified on a module
    interface or channel interface, or on a submodule or connection. It should
    contain a single (optional) value. The value names the type to use. If it is
    absent, the submodule or channel will not be created. (The connection will
    be created without a channel object.)

  - The `@mutable` property was introduced for parameters. If the module (or
    channel) is prepared for a parameter to change its value at runtime (i.e.
    the new value takes effect), then it should be marked with `@mutable`. If
    `@mutable` is missing, then trying to change the parameter will now result
    in a runtime error ("Cannot change non-mutable parameter".)

    The motivation is that in a complex model framework, there is usually a
    large number of (module or channel) parameters, and so far it has not been
    obvious for users which parameters can be meaningfully changed at runtime.
    For example, if a simple module did not implement `handleParameterChange()`
    or the `handleParameterChange()` method did not handle a particular
    parameter, then the user could technically change that NED parameter at
    runtime, but the change did not take effect. This has often caused
    confusion.

    Parameters of `ned.DelayChannel` and `ned.DatarateChannel` are now marked
    `@mutable`.

    To allow running existing simulation models that have not yet been updated
    with `@mutable` annotations, we have added the `parameter-mutability-check`
    configuration option. Setting `parameter-mutability-check=false` will give
    back the old behavior (not raising an error).

  - Added the `expr()` operator to NED, with the purpose of allowing models to
    accept formulas or expressions which it could use e.g. to determine the
    processing time of a packet based on packet length or other properties,
    decide whether a packet should be allowed to pass or should be filtered out
    based on its contents (filter condition), or to derive x and y coordinates
    of a mobile node in the function of time.

    The argument to `expr()` is a NED expression which will typically contain free
    variables, (like x and y in `expr(x+y)`). The `expr()` operator creates an
    object that encapsulates the expression, so it can be assigned to parameters
    of the type `object`. On the C++ side, the module implementation should
    query the parameter to get at the expression object (of type
    `cOwnedDynamicExpression`), and then it may bind the free variables and
    evaluate the expression as often as it wishes.

  - In `xml()` and `xmldoc()` element selectors, `$MODULE_INDEX`,
    `$PARENTMODULE_NAME` and similar variables now evaluate to the empty string
    if they are not applicable, instead of raising an error that was often
    problematic.

  - Improved error reporting during network building: when an error occurs
    during assigning a parameter from a NED file, the error message now includes
    the location of the assignment (file:line in NED file).


MSG:

  - Made the hitherto experimental operation mode and feature set of the message
    compiler official. This feature set was originally added around OMNeT++ 5.3,
    and could be enabled by passing the `--msg6` option to `opp_msgtool`. There
    was also a no-op `--msg4` option that selected the old (OMNeT++ 4.x
    compatible) operation. Most of the OMNeT++ 6.0 pre-releases shipped with the
    new operation mode being the default (i.e. `--msg6` became a no-op), with
    features continually being added and refined. Then, finally, the old code
    and the `--msg4` option were removed altogether.

    The new operation mode represents a complete overhaul of the message
    compiler, with significant non-backward-compatible changes to the MSG
    language. These features were largely motivated by the needs of INET 4.

  - The message compiler is now aware of the OMNeT++ library classes, as if
    they were imported by default. Declarations come from `sim_std.msg`.

  - Added import support. A message file can now reference definitions in
    other message files using the `import` keyword. Type announcements are
    no longer needed (in fact, they are ignored with a warning), and there
    is now much less need for `cplusplus` blocks as well.

  - Classes without an `extends` clause no longer have `cObject` as their
    default base class. If `cObject` should be the base class, `extends cObject`
    needs to be added explicitly.

  - Field getters now return `const` reference. Separate `get..ForUpdate()`
    getters that return non-`const` are generated to cover uses cases when the
    contained value (typically an object) needs to be modified in-place.

  - Added targeted `cplusplus` blocks, with the syntax of `cplusplus(<target>) {{..}}`.
    The target can be `h` (the generated header file -- the default),
    `cc` (the generated C++ file), `<classname>` (content is inserted into the
    declaration of the type, just before the closing curly bracket), or
    `<classname>::<methodname>` (content is inserted into the body of the
    specified method). For the last one, supported methods include the
    constructor, copy constructor (use `Foo&` as name), destructor, `operator=`,
    `copy()`, `parsimPack()`, `parsimUnpack()`, etc., and the per-field
    generated methods (setter, getter, etc.).

  - Enum names can now be used as field type name, i.e. `int foo @enum(Foo)`
    can now be also written as `Foo foo`.

  - Support for `const` fields (no setter generated/expected).

  - Support for pointer and owned pointer fields. Owned pointers are denoted
    with the `@owned` field property. Non-owned pointers are simply stored and
    returned; owned pointers imply delete-in-destructor and clone-on-dup
    behavior. Owned pointer fields have a remover method generated for them
    (`removeFoo()` for a `foo` field, or `removeFoo(index)` if the `foo` field
    is an array). The remover method removes and returns the object in the field
    or array element, and replaces it with a `nullptr`. Additionally, if the
    object is a `cOwnedObject`, `take()` and `drop()` calls are also generated
    into the bodies of the appropriate methods. There is also an `@allowReplace`
    property that controls whether the setter method of an owned pointer field
    is allowed to delete the previously set object; the default is
    `@allowReplace(false)`.

  - More convenient dynamic arrays: inserter, appender and eraser methods are
    now generated into the class. For example `insertFoo(index,value)`,
    `appendFoo(value)`, and `eraseFoo(index)` are generated for a `foo[]` field.

  - Support for pass-by-value for fields. Annotate the field with `@byValue`
    for that. `@byValue` and many other properties can also be specified on the
    class, and they are inherited by fields that instantiate that type.

  - Additional C++ base classes may be specified with the `@implements` property.

  - The `@str` property causes an `str()` method to be generated; the expression
    to be returned from the method should be given in the value of the property.

  - The `@clone` property specifies code to duplicate (one array element of) the
    field value.

  - The `@beforeChange` class property specifies the name of a method to be
    called from all generated methods that mutate the object, e.g. from setters.
    It allows implementing objects that become immutable ("frozen") after an
    initial setup phase.

  - The `@custom` field property causes the field to only appear in descriptors,
    but no code is generated for it at all. One can inject the code that
    implements the field (data member, getter, setter, etc.) via targeted
    `cplusplus` blocks.

  - The `@customImpl` field property, suppresses generating implementations for
    the field's accessor methods, allowing custom implementations to be supplied
    by the user.

  - Added the `@abstract` field and class property. For a field, it is
    equivalent to the `abstract` keyword; for classes, it marks the whole
    class as abstract.

  - Abstract fields no longer require the class to be marked with `@customize`.

  - Message files now allow more than one `namespace <namespace>;` directive.
    The `namespace;` syntax should be used to return to the toplevel C++
    namespace.

  - The names of generated method can be overridden with the following field
    properties: `@setter`, `@getter`, `@getterForUpdate`, `@sizeSetter`,
    `@sizeGetter`, `@inserter`, `@eraser`, `@appender`, `@remover`, etc.

  - Data types can be overridden with the following properties: `@cppType`,
    `@datamemberType`, `@argType`, `@returnType`, `@sizeType`.

  - Changed C++ type for array sizes and indices, i.e. the default of
    `@sizeType`, from `int` to `size_t`.

  - Added support for setting pointer members and array sizes via class
    descriptors. (`cClassDescriptor` had no facility for that.)
    This involves adding two methods to `cClassDescriptor` (`setFieldArraySize()`
    and `setFieldStructValuePointer()`), and support for the `@resizable()` and
    `@replaceable` field attributes that tell the message compiler to generate
    the respective code in the class.

  - The `@toString` and `@fromString` properties specify a method name or code
    fragment to convert the field's value to/from string form in class
    descriptors (`getFieldValueAsString()` and `setFieldValueFromString()`
    methods of `cClassDescriptor`). In the absence of `@toString`, previous
    versions converted the value to string by writing it to a stream using
    `operator<<`; now the `str()` method of the object is used if it has one. If
    neither `@toString` nor `str()` exist, an empty string is returned.

  - Likewise, the `@toValue` and `@fromValue` properties specify a method name
    or code fragment to convert the field's value to/from `cValue` form in class
    descriptors ((`getFieldValue()` and `setFieldValue()` methods of
    `cClassDescriptor`)).

  - Better code for generated classes, e.g. inline field initializers, and use
    of the `=delete` syntax of C++11 in the generated code.

  - Better code generated for descriptors, e.g. symbolic constants for
    field indices.

  - The list of reserved words (words that cannot be used as identifiers in MSG
    files; it is the union of the words reserved by C++ and by the MSG language)
    has been updated.

  - A complete list of supported properties (not all of them are explicitly
    listed above) can be found in an Appendix of the Simulation Manual.

Ini files:

  - It is now possible to break long lines without using a trailing backslash.
    Continuation lines are marked as such by indenting them, i.e. an indented
    line is now interpreted as a continuation of the previous line. (It is not
    possible to break a line inside a string constant that way.) Breaking lines
    using a trailing backslash way still works (and it can also be used to break
    string constants, too). Indentation-based line continuation has the
    advantage over backslashes that it allows placing comments on intermediate
    lines (whereas with backslashes, the first `#` makes the rest of the lines
    also part of the comment).

  - The `Config` prefix in section headers is now optional, that is, the heading
    `[Config PureAloha]` may now be also written as `[PureAloha]`, with the two
    being equivalent.

Simulation kernel / Modules, channels, programming model:

  - Added the `scheduleAfter()`, `rescheduleAt()`, `rescheduleAfter()` methods
    to `cSimpleModule`. They are mainly for convenience, but using
    `rescheduleAt()` instead of `cancelEvent()` + `scheduleAt()` will eventually
    allow for a more efficient implementation.

  - Change in the parameter change notification mechanism: Calls to the
    `handleParameterChange()` method during initialization are no longer
    suppressed. Because now every change results in a notification, the
    umbrella `handleParameter(nullptr)` call at the end of the initialization
    is no longer needed and has been removed. The consequence is that
    `handleParameterChange()` methods need to be implemented more carefully,
    because they may be called at a time when the module may not have
    completed all initialization stages. Also, if an existing model relied on
    `handleParameter(nullptr)` being called, it needs to be updated.

  - Improvements in the multi-stage initialization protocol with regard to
    dynamic module creation. Modules now keep track of the last init stage
    they completed (`lastCompletedInitStage`). During an init stage, initialization
    of modules is restarted if creation/deletion is detected during iteration;
    modules already initialized in the previous round are recognized and skipped
    with the help of `lastCompletedInitStage`.

  - `cModule` now keeps track of submodule vectors as entities, and not just as
    a collection of submodules with vector indices, meaning that we can now
    distinguish between nonexistent and zero-size submodule vectors. Random
    access of vector elements has also became more efficient (constant-time
    operation). Several new methods have been added as part of this change:
    `hasSubmoduleVector()`, `getSubmoduleVectorSize()`, `addSubmoduleVector()`,
    `deleteSubmoduleVector()`, `setSubmoduleVectorSize()`, `getSubmoduleVectorNames()`.

  - As part of the above change, several `cModule` and `cModuleType` methods have been
    added or updated. A partial list:

    - In `cModule`, the `hasSubmodule()`, `getSubmoduleNames()`, `hasGateVector()`,
      `hasGates()` methods have been added for consistency between submodule and
      gate APIs.
    - The return type of the `getGateNames()` method has been changed from
      `std::string<const char*>` to `std::vector<std::string>`, for consistency
      with `getSubmoduleNames()`.
    - `cModule`: added `setIndex()` and `setNameAndIndex()`.
    - `cModule`, `cGate`: `getIndex()`, `getVectorSize()`, `gateSize()` and
      similar methods now throw exception for non-vector submodules/gates.
    - `cModule`: add separate `addGateVector()` method instead misusing `addGate()`
      for creating gate vectors, also for consistency with `addSubmoduleVector()`.
    - In `cModuleType::create()`, the `vectorSize` parameter of `create()` has
      been removed.
    - In `cModuleType::createScheduleInit()`, an index argument was added to
      allow creating submodule vector elements.
    - `cModule`: added `addSubmodule()` method which flows more naturally than
      `cModuleType::create()`.

  - There have been changes in submodule and channel iterators. `SubmoduleIterator`
    has been rewritten due to the change in how submodule vectors are represented,
    which may affect the iteration order in some cases. Iterators now throw an
    exception if a change occurs in the list of submodules/channels during
    iteration. Their APIs have also changed a little: `operator--` was removed,
    and `init(m)` was renamed to `reset()`.

  - Optimized `cModule::ChannelIterator` by letting `cModule` maintain a linked
    list of channels (`cChannel`), so that `ChannelIterator` doesn't have to search
    through the whole compound module to find them.

  - Module name and full name (i.e. "name[index]") are now stringpooled, which
    reduces memory usage in exchange for a small build-time extra cost.
    In addition, string pools now use `std::unordered_map` instead of `std::map`,
    which results in improved performance.

  - `cComponent`: added `getNedTypeAndFullName()` and `getNedTypeAndFullPath()`.
    They are especially useful in constructing error messages in NED functions.

Simulation kernel / Signals and notifications:

  - `intpar_t` was renamed to `intval_t`, and `uintval_t` was added. Both are
    guaranteed to be at least 64 bits wide.

  - Signal listeners changed to use `intval_t` and `uintval_t` instead of `long`
    and `unsigned long`. This change was necessary because `long` is only 32 bits
    wide on Windows. This affects methods of `cListener` and subclasses like
    `cResultFilter` and `cResultRecorder`.

  - Emitting `nullptr` as a string (`const char*` overload of `emit()`) is
    disallowed. The reason is that `nullptr` cannot be represented in
    `std::string`, which causes problems e.g. in result filters and recorders.

  - Added two new model change notifications: `cPostModuleBuildNotification`,
    `cPostComponentInitializeNotification`.

  - Minor changes in some model change notification classes. In
    `cPreModuleAddNotification`, the `vectorSize` field was removed (as it was
    redundant), and in `cPostModuleDeleteNotification`, the interpretation of
    the index field has changed a little: if the deleted module was not part
    of a module vector, index is now set to -1 instead of 0.

Simulation kernel / Utilities:

  - Two utility functions were added to `cObject`: `getClassAndFullPath()` and
    `getClassAndFullName()`. They are mostly useful in logging and error messages.

  - `cMatchExpression`: The `field =~ pattern` syntax replaces `field(pattern)`.
    Also removed the implicit OR syntax. The old syntaxes looked confusing, and
    made it difficult to tell apart the concise notation from expression-style notation.

  - Added `opp_component_ptr<T>`. It implements a smart pointer that points to
    a `cComponent` (i.e. a module or channel), and automatically becomes `nullptr`
    when the referenced object is deleted. It is a non-owning ("weak") pointer,
    i.e. the pointer going out of scope has no effect on the referenced object.
    `opp_component_ptr<T>` can be useful in implementing modules that hold
    pointers to other modules and want to be prepared for those modules
    getting deleted. It can also be useful for simplifying safe destruction
    of compound modules containing such modules.

  - New classes: `opp_pooledstring`, `opp_staticpooledstring`. They provide pool-
    backed string storage (reference-counted and non-reference-counted,
    respectively). In turn, the `cStringPool` class was removed; use either
    of the pooled string classes or or `opp_staticpooledstring::get(const char *)`
    instead.

  - Several string functions have been made available for models. A
    representative partial list: `opp_isempty()`, `opp_isblank()`,
    `opp_nulltoempty()`, `opp_trim()`, `opp_split()`, `opp_splitandtrim()`,
    `opp_join()`, `opp_stringendswith()`, `opp_substringbefore()`, etc.

  - The `cStringTokenizer` class has been rewritten. It now supports features
    like optional skipping of empty tokens, optional trimming of tokens,
    optional honoring of quotes, optional honoring of parens/braces/brackets
    (i.e. the input string is not broken into tokens in the middle of a
    parenthesized expression).

Simulation kernel / Visualization support:

  - Added display name support to modules. Display name is a string that
    optionally appears in Qtenv next to (or instead of) the normal module
    name, in order to help the user distinguish between similarly-named
    submodules. For example, application-layer modules `app[0]`, `app[1]`, etc.
    in INET may be given descriptive names like "file transfer", "video",
    "voice", or "CBR", and have them displayed in Qtenv. Display names may be
    set using the `display-name` per-module configuration option, or
    programmatically by calling `setDisplayName()`.

  - Added the `g=<group>` (layout group) display string tag, which makes it
    possible to apply predefined arrangements like row, column, matrix or ring
    to a group of unrelated submodules. This layouting feature was previously
    only available for submodule vectors. When "g" tags are used, submodules
    in the same group are now regarded for layouting purposes as if they
    were part of the same submodule vector.

  - Made it possible to specify display strings in the configuration. The
    value given in the `display-string` per-component configuration option
    is merged into the component's display string in the same way inheritance
    or submodule display strings work: it may add, overwrite or remove items
    from it.

  - Added text alignment support to text and label figures: `cFigure::Alignment`
    enum, `getAlignment()`/`setAlignment()` in `cAbstractTextFigure`.

  - Added the `toAlpha()` method and a constructor taking `Color` to
    `cFigure::RGBA`.

Simulation kernel / Transmission updates:

  - The initial `send()` is interpreted as: "packet transmission begins now,
    packet content and duration are, as things are now, going to be this".

    Following that, an "update" (or any number of updates) can be sent.
    An update is a packet with the updated ("actual") content, and with a
    "remaining transmission duration" attached. Updates may only
    be sent while transmission is still ongoing.

    As an example, aborting a transmission is done by sending a
    packet with a truncated content and a remaining duration of zero.

    Transmission updates are paired with the original packet they modify
    using a transmissionId. The transmissionId is normally chosen to be the
    packet ID of the original transmission. Channels should understand
    updates and handle them accordingly.

    Receivers that receive the packet at the end of the reception, which is the
    default operating mode, will only receive the final update. The original
    packet and intermediate updates are absorbed by the simulation kernel.

    Receivers that receive the packet at the start of the reception (see
    `cGate::setDeliverImmediately()`, previously called
    `setDeliverOnReceptionStart()`) should be prepared to receive all of the
    original packet and the updates, and handle them appropriately. Tx updates
    can be recognized from `cPacket::isUpdate()` returning `true`.
    `cPacket::getRemainingDuration()` returns the remaining transmission
    duration, and `cPacket::getDuration()` the total transmission duration.

    As a safeguard against unprepared modules accidentally processing
    tx updates as normal full-blown packets, the module is only given
    tx updates if it explicitly declares that it is able to handle them
    properly. The latter is done by the module calling
    `setTxUpdateSupport(true)` before receiving packets, e.g. in `initialize()`.

    Non-transmission channels treat tx updates in the same way as they treat any
    other messages and packets (they ignore the `cPacket::isUpdate()` flag).

    Details and related changes follow.

  - `send()` and `sendDirect()` now accept a `SendOptions` struct where optional
    parameters such as delay can be passed in. `sendDelayed()` and other
    `send()`/`sendDirect()` variants now convert their extra args to a `SendOptions`,
    and delegate to the "standard" `send()`/`sendDirect()` versions. `SendOptions` was
    introduced as a means to handle combinatorial explosion of `send()` variants.

  - For methods that participate in the send protocol (`cGate::deliver()`,
    `cModule::arrived()`, `cChannel::processMessage()`), `SendOptions` was added.

  - `cDatarateChannel` now allows the sender to explicitly specify the packet
    duration in `SendOptions`, overriding the duration that the channel would
    compute from the packet length and the channel datarate.

  - `cDatarateChannel`'s datarate is now optional: set it to 0 or `nan`
    to leave it unspecified. This change was necessary to support
    transmitting frames with per-frame data rate selection. If the datarate
    is unspecified, the packet duration must be supplied in the send call,
    otherwise a runtime error will be raised.

  - `cDatarateChannel`: non-packet messages now pass through without
    interfering with packets.

  - `cDatarateChannel`: disabled channels now let transmission updates through,
    so that it is possible for the transmitter module to abort the ongoing
    packet transmission.

  - Tx updates (without duration/remainingDuration) are allowed on paths without
    transmission channels.

  - In `cChannel::processMessage()`, `result_t` was renamed `cChannel::Result`,
    and it is now a proper return value (not an output parameter).

  - `remainingDuration` was added to `cChannel::Result`.

  - `cDatarateChannel` now optionally allows multiple concurrent transmissions,
    with or without any bookkeeping and associated checks. This is useful for
    modeling a channel with multiple subchannels or carriers. The operating mode
    has to be selected programmatically, with the channel's `setMode()` method.
    Possible modes are `SINGLE`, `MULTI` and `UNCHECKED`.

  - The `forceTransmissionFinishTime()` method of channels has been deprecated.
    It was always meant as a temporary device to allow implementing aborting
    frame transmissions, and now with the arrival of the new transmission
    update API there is no reason to use it any more. Simulations using it
    should be migrated to the new API.

  - Renamed `setDeliverOnReceptionStart()` to `setDeliverImmediately()`.

  - Added `cSimpleModule::supportsTxUpdates()` flag.

  - `cPacket` now carries a `remainingDuration` field.

  - `cPacket`: eliminated `FL_ISRECEPTIONSTART`; `isReceptionStart()` now uses
    `remainingDuration` as input; added a similar `isReceptionEnd()` method.

  - `cPacket::str()` overhaul to reflect new fields and uses.

  - In the APIs, send delay and propagation delay, which were sort of
    combined into a single value, are now distinct values, handled
    separately.

Simulation kernel / Module deletion:

  - Added the `preDelete()` method to `cComponent`. This is an initially empty
    virtual function that the user can override to add tasks to be done
    before the module (or channel) is deleted. When `deleteModule()` is called
    on a compound module, it first invokes `preDelete()` for each module in the
    submodule tree, and only starts deleting modules after that. `preDelete()`
    can help simplify network or module deletion in a complex simulation that
    involves model change listeners.

  - `cIListener`'s destructor now unsubscribes from all places it was subscribed
    to. This change was necessitated by the following `deleteModule()` change.

  - `deleteModule()`: Module destruction sequence was changed so that when
    deleting a compound module, the compound module's local listeners are
    notified about the deletion of the submodules.

  - `deleteModule()` internals refactored. The motivation was to avoid doing
    things like firing pre-model-change notifications from a halfway-deleted
    module. Now we do every potentially risky thing (such as deleting submodules
    and disconnecting gates) from `doDeleteModule()`, and only delete the module
    object when it is already barebones (no submodules, gates, listeners, etc).
    With this change, the deletion sequence is now pretty much the reverse of
    the setup sequence.

  - Now it is allowed for modules to be deleted (including self-deletion) and
    created at will during initialization.

Simulation kernel / Expressions, object parameters, JSON values:

  - Under the hood, all expression parsing and evaluation tasks now use the same
    new generic extensible expression evaluator framework. It is used for NED
    expressions, object matching, scenario constraint, statistics recording,
    result selection in the Simulation IDE and in `opp_scavetool`, in
    `cDynamicExpression` and `cMatchExpression`, etc. In the new framework,
    expression parsing and the translation of the AST to an evaluator tree are
    done in separate steps, which makes the library very versatile. Evaluators
    include support for shortcutting logical and conditional operators, constant
    folding, and an `undefined` value (anything involving `undefined` will evaluate
    to `undefined`).

  - `cNedValue` was renamed to `cValue` (compatibility typedef added), as it is
    now a generic value container used throughout the simulation kernel (`cPar`,
    `cExpression`, `cClassDescriptor`, etc.), i.e. it is no longer specific to NED.

  - `cValue`'s `doubleValue()` and `intValue()` methods now throw an exception when
    called on a value that has a measurement unit, in order to reduce usage
    mistakes. If the value has a unit, call either `doubleValueInUnit()` /
    `intValueInUnit()`, or `doubleValueRaw()`/`intValueRaw()` plus `getUnit()`.

  - `cValue` changed to hold `any_ptr` (see later) instead of `cObject*`. This
    change involves several changes, e.g. type `OBJECT` renamed to `POINTER`,
    and `pointerValue()` added.

  - `cPar`: Added support for object parameters. New type constant: `OBJECT`.
    New methods: `setObjectValue()`, `objectValue()`, `operator=(cObject*)`,
    `operator cObject*`.

  - `cPar`: Added `cValue`-based generic access: `getValue()`, `setValue()`.

  - Store origin (file:line) info in `cPar` parameters (more precisely, in
    `cDynamicExpression`), so we can report it on evaluation errors.
    Most visible change: `cPar::parse()` gained an extra `FileLine` argument.
    Also, `cDynamicExpression` now has `get/setSourceLocation()`.

  - Added `cValueArray` and `cValueMap` classes for representing JSON data in
    NED expressions. A third class is `cValueHolder`, a wrapper around `cValue`,
    which is only used when a non-object value (double, string, etc) is assigned
    to a NED parameter of the type `object`. All three classes subclass from
    `cValueContainer`. Note that behavior of the `dup()` methods of
    `cValueArray` and `cValueMap` is consistent with that of `cArray` and
    `cQueue`, i.e. only those objects that are owned by the cloned container are
    duplicated.

  - NED functions that take or return values of type `object` are now allowed.

  - NED functions can now be defined with the alternative signature:

      cValue f(cExpression::Context *context, cValue argv[], int argc)

    in addition to the existing signature

      cValue f(cComponent *contextComponent, cValue argv[], int argc)

    The `cExpression::Context` argument allows one to access the context
    component, and also the directory where the ini file entry or the
    NED file containing the expression occurred (the "base directory").
    `Define_NED_Function()` accepts both signatures. The base directory
    is useful for functions like `xmldoc()` that want to access files
    relative to the location of the NED expression.

  - `cNedFunction` now allows to search for NED functions by name AND accepted
    number of args.

  - `cDynamicExpression` has been reimplemented using the new internal
    `Expression` class, and support for user-defined variables, members,
    methods, and functions was added. As a consequence, the public interface of
    the class has significantly changed as well.

  - Added the `cOwnedDynamicExpression` class which holds the result of a NED
    `expr()` operator. `cOwnedDynamicExpression` is both `cOwnedObject` and
    `cDynamicExpression` (multiple inheritance). To make this possible, the
    `cObject` base class was removed from `cExpression`.

  - `cClassDescriptor`: Use exception instead of returning `false` for
    indicating error. The return types of the following methods changed
    from `bool` to `void`: `setFieldValueAsString()`, `setFieldArraySize()`,
    `setFieldStructValuePointer()`.

  - `cClassDescriptor`: Added support for setting pointer members and array
    sizes via class descriptors. New methods: `setFieldArraySize()`,
    `setFieldStructValuePointer()`.

  - `cClassDescriptor`: Added `getFieldValue()`/`setFieldValue()` methods to
    allow accessing fields in a typed way, using `cValue`. Previously existing
    methods `getFieldValueAsString()`/`setFieldValueAsString()` only allowed
    string-based access. In MSG files, the `@toValue()` and `@fromValue()`
    properties can be used to provide code to convert objects or fields to
    `cValue`.

  - `cClassDescriptor`: Methods changed to use `any_ptr` instead of `void*`
    for passing the object. (`any_ptr` is a smart pointer class that provides
    type safety for `void*` pointers.) Pointers need to be put into and extracted
    from `any_ptr` using the new `toAnyPtr()` / `fromAnyPtr()` functions. They have
    specialized versions for each type (via templates and overloading).
    For new types, the message compiler generates `toAnyPtr()`/`fromAnyPtr()`
    in the header file. For the simulation library classes, these methods
    come from `sim_std_m.h` (generated from `sim_std.msg`); `sim_std_m.h`
    is now part of `<omnetpp.h>`.

Simulation kernel / Fingerprints:

  - Due a bugfix in `cHasher::add(const char *)`, fingerprints that involve
    hashing strings changed their values.

  - The implementation of `cHasher::add(const std::string&)` was changed to be
    consistent with the `add(const char *)` overload. This may cause fingerprint
    changes in models that use it.

  - Changed the way fingerprints are computed from figures. Most importantly,
    fingerprints are now affected by all visual properties, not just geometry
    information. This change only affects fingerprints that contain the 'f'
    (=figures) ingredient.

  - The introduction of the new expression evaluation framework also somewhat
    affects fingerprints. The fact that logical operators and inline-if are now
    shortcutting may change the fingerprint of some simulations, due to
    consuming fewer random numbers during expression evaluation.

Simulation kernel / Miscellaneous:

  - The `getModuleByPath()` method was changed to never return `nullptr`, even if
    an empty string is given as path. Instead, it will throw an exception if the
    module was not found. This change makes this method consistent with other
    getter methods in the simulation library, and allows `nullptr` checks to be
    removed from model code that uses it. A new method, `findModuleByPath()` was
    added for cases when an optionally existing module needs to be found. These
    methods, initially defined on `cModule`, have been moved to `cComponent` so
    that they can be called on channels too.

  - Signature change of the `cVisitor::visit(cObject *obj)` virtual method: it
    can now request end of iteration via returning `false` (hence, return type
    changed from `void` to `bool`) instead of throwing `EndTraversalException`.
    Existing `cVisitor` subclasses in model code will need to be adjusted.

  - `cPar`: Implemented `isMutable()` and the mechanism behind the new `@mutable`
    property.

  - `cProperty`: Added `getNumKeys()` method; `updateWith()` made public.

  - `cConfiguration`: Removed `getParameterKeyValuePairs()`. Instead,
    `getKeyValuePairs()` made smarter with an extra `flags` parameter
    to be able to handle the various use cases.

  - `cMessage`: Allowed `isMessage()` to be called from subclasses.

  - `cEnvir`: New result recording related methods: `recordComponentType()`,
    `recordParameter()`.

  - `cEnvir`: Added `getConnectionLine()`, which returns the coordinates of a
     connection arrow. This is for certain custom animations.

  - `cEnvir`: Added `pausePoint()`, an animation-related experimental API.

  - Result filters: Two new methods in the `cResultListener` interface:
    `emitInitialValue()` and `callEmitInitialValue()`.

  - `cResultFilter`, `cResultRecorder`: Grouped `init()` args into a `Context` struct,
    The old `init()` methods have been preserved as deprecated (and invoked from
    the new `init()`) in case an existing filter/recorder overrides them. Note
    that potential external calls to the old `init()` method won't work any more
    (they will have no effect), and need to be changed to the new version.

  - Added `MergeFilter`, a result filter that allows multiple inputs,
    and multiplexes them onto its singe output. It is available (as the
    `merge()` function) in the `source=` part of `@statistic`.

  - Fixed histogram loading issue in the output scalar file (.sca). Bin edges
    that are very close could become equal in the file if insufficient printing
    precision was set, rendering the file unreadable. The issue is now handled
    both during result file writing (if such condition is detected, bin edges
    are written with full [16-digit] precision) and reading (zero-width bins are
    merged into adjacent nonzero-width bin).

Simulation kernel / Cleanup:

  - Removed obsolete/deprecated classes and methods. A partial list:
    `cVarHistogram`, `cLegacyHistogram`, `cLongHistogram`, `cDoubleHistogram`,
    `cWeightedStdDev`, `cDensityEstBase`;  `detailedInfo()` method; `timeval_*()`
    functions; `cHistogram` methods `setRangeAuto()`, `setRangeAutoLower()`,
    `setRangeAutoUpper()`, `setNumCells()`, `setCellSize()`; `operator()` of iterator
    classes in `cArray`/`cModule`/`cQueue`; `cFigure`/`cCanvas` deprecated methods
    `addFigureAbove()` and `addFigureBelow()`; many `cAbstractHistogram`
    (ex-`cDensityEstBase`) methods, etc. Instead of the removed `timeval_*()`
    methods, use `opp_get_monotonic_clock_usecs()` or
    `opp_get_monotonic_clock_nsecs()`, and perform the arithmetic in `int64_t`.

  - Refactoring: Some classes, methods and variables related to ownership
    management were renamed: `cDefaultOwner` -> `cSoftOwner`; `defaultOwner` ->
    `owningContext`, etc.

  - The `setPerformFinalGC()` method was removed. It was meant for internal
    use, and pretty much unused by model code.

  - Removed the support for OMNeT++ 4.x fingerprints (`USE_OMNETPP4x_FINGERPRINTS`).

  - `WITH_OMNETPP4x_LISTENER_SUPPORT` was removed.

  - Source code modernization: use in-class member initializers wherever possible.
    The source code now requires a C++14 compiler.

  - Internal classes, global variables, etc moved into the `omnetpp::internal`
    namespace.

Runtime:

  - Accept expressions as value for (most) config options. For options that
    accept values both with and without quotes (types STRING, FILENAME,
    FILENAMES, PATH), a heuristic decides whether a string is to be taken
    literally or to be evaluated as an expression. Expressions may also use NED
    operators, module parameters and other NED expression features. For example,
    it is possible to use the module vector index in the value of the
    `display-name` option: `**.app[0..3].display-name = "cbr-" + string(index)`

  - Allow parameter values to be specified on the command line. For example,
    `--**.mss=512` is equivalent to inserting the `**.mss=512` line near the
    top of the configuration in `omnetpp.ini`.

  - Do not complain about missing `omnetpp.ini` if a `--network` command-line
    option is present.

  - There were several improvements related to the NED path. The `NEDPATH`
    environment variable has been renamed to `OMNETPP_NED_PATH`, but the old one
    is still recognized for backward compatibility. Multiple `-n` options are now
    accepted. Also, the `NEDPATH` environment variable used to be ignored when a
    `-n` option was present, no longer so. Excluding packages from NED file
    loading has also been implemented. NED exclusions can be specified in
    multiple ways: the `-x <packagelist>` command-line option, the
    `OMNETPP_NED_PACKAGE_EXCLUSIONS` environment variable, and the
    `ned-package-exclusions` configuration option (they accept semicolon-
    separated lists).

  - Change in NED loading: Now, if a package has files in several distinct
    source trees, only one of them may contain a `package.ned` file.

  - There were also several improvements related the image path. The `-i <imgpath>`
    option command-line option has been added. It may occur multiple
    times. The image path is now defined jointly by the `OMNETPP_IMAGE_PATH`
    environment variable, `-i` command-line options, and the `image-path`
    configuration option. `./bitmaps` was removed from default image path, as it
    was virtually unused. The default value `./images` now comes from the
    default value of the image-path configuration option.

  - Added support for embedding NED files into a binary (an executable or
    library). The `cpp` subcommand of `opp_nedtool` generates C++ code that
    contains the content of NED files as string constants, which can then be
    compiled into the simulation binary. When the simulation program is started,
    these embedded NED files will be loaded automatically, and the original NED
    files will no longer be needed for running simulations. Optional garbling,
    which prevents NED source code from being directly readable inside the
    compiled binaries, is also available. A `makefrag` file can be used to
    integrate the NED-to-C++ translation into the build process. To see an
    example `makefrag` file, view the `makefrag` help topic in `opp_nedtool`.

  - Several global config options were changed to be per-run: `scheduler-class`,
    `debug-on-errors`, `print-undisposed`, `fingerprintcalculator-class`.

  - Added the `parsim-num-partitions` config option, which makes it possible
    to explicitly declare the number of partitions to be used with parallel
    simulation. (Before, it was explicitly taken by OMNeT++ from MPI or the
    respective communication layer.)

  - Added the `config-recording` configuration option, which controls
    the amount of configuration options to save into result files.

  - Allow recording actual module/channel parameter values into the output
    scalar file, via the new `param-recording` per-object boolean configuration
    option. Note that parameters will be recorded as "parameter" result items,
    not as scalars. For volatile parameters, the expression itself will be
    recorded (e.g. `exponential(0.5)`), not any particular value drawn from it.

  - Added support for result directory subdivision: the `resultdir-subdivision`
    boolean configuration option and the `${iterationvarsd}` variable. The
    motivation is that the performance of various file managers (including
    Eclipse's Project Explorer) tends to degrade severely if there are more than
    a few thousand files in a single directory. In OMNeT++, this problem occurs
    when a parameter study produces a large number of result files. The common
    workaround is to "hash" the files into a subdirectory tree. (For example,
    git also uses this technique to store the contents of `.git/objects` dir).

    In OMNeT++, such feature can be turned on by setting `resultdir-subdivision=`
    `true` in the configuration. Since it is natural to use the iteration
    variables to define the directory hierarchy, directory subdivision makes
    use of the new `${iterationvarsd}` configuration variable. This variable is
    similar to `${iterationvarsf}` but it contains slashes instead of commas,
    which makes it suitable for creating a directory tree. Enabling directory
    subdivision will cause `/${configname}/${iterationvarsd}` to be appended to
    the name of the results directory (see `result-dir` config option), causing
    result files to be created in a subdirectory tree under the results
    directory.

    If you want to set up the directory hierarchy in a different way, you can do
    so by setting the `result-dir` config option and appending various variables
    to the value. E.g.: `result-dir = "results/${repetition}"`

  - Added the `${datetimef}` inifile variable, which contains the current
    timestamp in a form usable as part of a file name.

  - Eventlog recording: Implemented snapshot and incremental index support to
    increase performance and scalability. This introduces significant (breaking)
    changes in the `elog` file format, and adds a set of associated configuration
    options; see the Sequence Chart section below for details.

  - Added `%<` (trim preceding whitespace) to the list of accepted directives
    for log prefixes.

  - The obsolete command-line options `-x`, `-X`, `-g`, and `-G` were removed.

  - Made the `-q` option more permissive: if `-c` is missing, assume `General`

  - Added the `-e` option, which prints the value of the given configuration option.

  - `opp_run -v` output now includes the system architecture OMNeT++ was built for
    (e.g. `ARCH_X86_64` or `ARCH_AARCH64`).

  - `-h resultfilters` and `-h resultrecorders` now include the descriptions in
    the list of result filters and recorders.

  - Added two new `-h` topics: `latexconfigvars`, `sqliteschema`. They
    are mainly used for producing info for the Appendices in the manual.

  - In order to reduce OMNeT++'s external dependencies, we now use an embedded
    copy of Yxml as the default XML parser. Yxml (https://dev.yorhel.nl/yxml)
    is a small and fast SAX-style XML parser with a liberal license.

    Support for using LibXML2 remained in place, but very few users will
    actually need it. Expat support has been removed. Note that no significant
    functionality is lost when Yxml is used instead of LibXML2. LibXML2 is only
    needed for Schema or DTD-based validation (and possibly, default value
    completion), which virtually no simulation models require. Also note that
    Yxml-based parsing scales much better than LibXML2, both performance-wise and
    regarding memory usage.

  - The bundled SQLite sources were upgraded to version 3.30.1.

Statistics recording:

  - Added the `warmup` filter. This filter discards values during the warm-up
    period, and is automatically inserted at the front of the result filter
    chains when the `warmup-period` config option is present.

  - Added the `autoWarmupFilter` statistic attribute that allows one to disable
    auto-adding the `warmup` filter to a statistic. Example:

        @statistic[foo](record=vector;autoWarmupFilter=false);

    This will cause all values from the `foo` signal to be recorded, even
    values emitted during the warm-up period (if one is set).

    However, the real motivation behind this feature is to allow the user to add
    the warm-up filter at a non-default location in the filter chain, because the
    default location is not always correct.

    For example, `@statistic[foo](source=min;record=vector)` is equivalent to
    `@statistic[foo](source=min(warmup(foo));autoWarmupFilter=false;record=vector)`,
    and records (as vector) the minimum of the values which were emitted after
    the warmup period is over. In contrast, if we replace `min(warmup(foo))`
    with `warmup(min(foo))`, it will compute the minimum of ALL values, but only
    starts recording the result after the warmup period has expired.

    This is a crucial difference sometimes. For example, a statistic might
    record queue length computed as the difference of the number of messages
    that entered the queue and those that left it:

        @statistic[queueLen](source=count(pkIn)-count(pkOut);record=vector); //INCORRECT

    In this case, if a warmup period is set, the result may even go negative
    because the `pkIn` and `pkOut` signals that arrive during the warmup period
    are ignored, and if `pkOut` arrives after that, we are at -1. The correct
    solution is to add the `warmup` filter after the difference between arrivals
    and departures have been computed:

        @statistic[queueLen](source=warmup(count(pkIn)-count(pkOut));autoWarmupFilter=false;record=vector); //OK

    The `autoWarmupFilter` option exists because either location for the warmup
    filter (beginning or end of the chain, or even mid-chain) may make sense for
    certain statistics. The model author needs to decide per-statistic which one
    is correct.

  - Added the `demux` filter, which allows splitting the stream of values arriving
    from a signal to multiple streams. For example, if values from a `foo`
    signal are tagged with the labels `first`, `second` and `third`, then the
    following statistics:

        @statistic[foo](record=count(demux(foo)));

    will produce three scalars: `first:foo:count(demux)`, `second:foo:count(demux)`,
    and `third:foo:count(demux)`. The labels are taken from the (full) name of
    the details object specified in the `emit()` call.

    This filter is especially useful if you intend to save multiple instances
    of the same statistics from the same module (e.g. per-connection TCP
    statistics).

  - In result files, the `,vector` suffix is now suppressed in the titles of
    vector results (similarly also `,histogram` and `,stats`), as they simply echo
    the result item's type. (They were there in the first place because
    recording mode is automatically appended to result titles `@statistic(title=...)`
    after a comma; it is now suppressed for the mentioned recording modes.)

  - Added the `merge` filter which multiplexes several inputs onto a single output.
    It is available in the `source=` part of `@statistic`.

  - Result filters: The `count` and `sum` filters now record the initial zero
    value as well.

  - Result files now include two new result attributes for each item: `recordingmode`,
    which is the item in the `@statistic(record=...)` list that produced the given
    result item, and `moduledisplaypath`, the module/channel path that contains
    display names instead of the normal names where available.

  - `sumPerDuration` filter: fix computation when invoked during warmup period.

Cmdenv:

  - Added the `cmdenv-fake-gui` boolean configuration option, which enables
    "fake GUI" functionality during simulation. "Fake GUI" means that
    `refreshDisplay()` is periodically invoked during simulation, in order to
    mimic the behavior of a graphical user interface like Qtenv. It is useful
    for batch testing of simulation models that contain visualization. Several
    further configuration options exist for controlling the details:
    `cmdenv-fake-gui-after-event-probability`, `cmdenv-fake-gui-before-event-probability`,
    `cmdenv-fake-gui-on-hold-numsteps`, `cmdenv-fake-gui-on-hold-probability`,
    `cmdenv-fake-gui-on-simtime-numsteps`, `cmdenv-fake-gui-on-simtime-probability`,
    `cmdenv-fake-gui-seed`.

Qtenv:

  - Modernized look: Material-style SVG-based icons, HIDPI support, dark theme
    support.

  - New actions on the main toolbar: "Debug next event", "Debug on errors",
    "Show animation parameters". ("Load NED file" was removed from the toolbar
    as it was rarely needed, but it's still available from the menu.)

  - The default digit separator used in the main simulation time and event
    number displays was changed to space.

  - A lot of effort was made to refine packet animation, also with regard to
    the new "transmission updates" API. For example, the animation filter now
    affects deliveries as well, and transmissions on ideal channels are now
    shown as a full-length line. Transmission updates are drawn as "notches"
    on the message line.

  - Turned off animating method calls by default. The rationale is that method
    call animations usually expose low-level (C++ implementation level)
    details on the GUI, which are rarely of interest to a casual user.

  - Added the possibility to disable method call visualization locally (for that
    module type) via the context menu, even when the global switch in the
    Preferences dialog is turned on.

  - Added support for showing a submodule's display name under the icon
    instead of, or in addition to, the normal name. The format can be
    selected in the context menu.

  - The view mode (grouped/flat/inheritance/children) in the Object Inspector
    used to be tied to the type (class) of the object displayed in the
    inspector. Since that resulted in too much mode switching while the user
    navigated the object tree, and the switching logic was not easily
    discoverable by the user, we removed the feature of per-type remembering of
    view modes. The view mode now only changes when the user explicitly
    switches in on the UI.

  - In the Object Inspector, added the possibility to select display mode
    (Children/Grouped/Flat/Inheritance) per-node. If display mode override is
    specified for a node, it will affect the whole subtree. The display mode for
    the selected tree can be changed via hotkey (Ctrl+B) or via the context
    menu.

  - The "Set Sending Time as Reference" option was added to the messages view
    context menu. This option makes it possible to set a reference time, and
    display all other times as a delta compared to it.

  - In the messages view, there was a slight change in the notation used for the
    source and destination modules of the message, in order to make it
    unambiguous. Use explicit "." and "^" to indicate the location of the
    module. Also, it now uses arrows of uniform lengths everywhere.

  - The "Allow Backward Arrows for Hops" option was added to the messages view
    context menu. When this option is enabled, it allows the use of backward
    arrows to ensure a consistent relative ordering of modules in the log. For
    example, if two modules A and B exchange messages, this option will cause
    the window to display them as "A-->B" and "A<--B", as opposed to the default
    "A-->B" and "B-->A". This sometimes makes the log easier to follow.

  - Added support for new columns in the packet log view: TxUpdate, Durations,
    Length, Info. This was implemented in the `cDefaultMessagePrinter` class
    which is part of the simulation library. This change makes it possible
    to see if a packet is a transmission update (except if the simulation
    installs its own packet printer like INET does).

  - In the messages view, simulation time is now formatted with digit grouping
    on.

  - In the log, the banners of component init stages that do not print anything
    are now suppressed.

  - Added the "Fira Code" font as embedded resource, and set it to be used by
    default for the log window. The reason is that it provides nice "-->" arrow
    ligatures in the Messages view.

  - Prevent manually enabled "Debug on Errors" setting from being turned
    off by (lack of) configuration option.

  - Fix osgEarth viewpoints ignoring SRS (PR #851).

  - Countless small improvements and bug fixes.

  - Raised the minimum required Qt version to 5.9.

Tkenv:

  - Tkenv has been removed. Use Qtenv for all simulations.

Tools:

  - The names of all command-line tools now begin with `opp_`.

  - Added `opp_ide`, an alternative to the `omnetpp` command that launches the
    Simulation IDE.

  - Added `opp_neddoc`, a tool that makes it possible to generate HTML
    documentation from NED files from the command line. `opp_neddoc` works by
    launching the IDE in headless mode.

  - `opp_nedtool` was rewritten for convenience and features. The main points
    are the following:
      - Removed msgc-related functionality, now it is really just about NED.
      - Command-line interface redesigned for better usability (subcommands,
        better help, etc.)
      - Added support for generating C++ source (`cpp` subcommand), for
        embedding NED files into an executable or library.
      - The tool now accepts directories as command-line arguments, too, and
        processes all NED files in them.
      - More convenient output (no `*_n.ned`, `*_n.xml`).
      - Removed obsolete `@listfile` and `@@listfile` support.
      - Fixed bugs in splitting NED files to one NED type per file.

  - `opp_msgtool` was rewritten in the style of `opp_nedtool`. Details:
      - Made `--msg6` the default mode, `--msg4` was removed.
      - The command-line interface now uses subcommands; if no subcommand is
        specified, the default action is C++ code generation. The rarely useful
        `-T` (type of next file) and `-h` (here) options have been removed.
      - Better help. A `builtindefs` help page has also been added, which
        prints the built-in declarations.

  - `opp_scavetool` had several improvements. A brief summary:
      - The tool now accepts directories as command-line arguments, too, and
        recursively loads scalar and vector files from them.
      - Command-line arguments may now contain double asterisk wildcards (`**`)
        in addition to normal wildcards (`*`,`?`). Double asterisks can match any
        number of directory levels. Example: `results/**.vec` matches all `.vec`
        files anywhere under the `results/` directory. (When specifying wildcard
        arguments on the command line, be sure to quote them so that the shell
        does not expand them before `opp_scavetool` gets invoked.)
      - Added options to limit vector data by simulation time:
        `--start-time <time>`, `--end-time <time>`.
      - Accepts exporter options in the `--<key>=<value>` notation as well, not
        only with `-x`.
      - In filter expressions, run attributes now have to be referred to with
        the `runattr:` prefix. The `attr:` prefix now only matches result attributes.
      - In exporters, the way of representing histograms in the output has slightly
        changed: underflow/overflow values are now saved separately instead of as
        underflow/overflow "bins". Thus, there are now one more bin edges than bin
        values.
      - The possibility to apply vector operations to vector results was removed
        from `opp_scavetool` and exporters. The recommended way of processing output
        vectors is with Python and NumPy.
      - The `help` subcommand was refined.

  - Added `opp_charttool`, a tool for "running" ANF files on the command line,
    e.g. for image or data export. Filtering options (`-i`, `-n`) allow multiple
    charts to be selected for exporting. The `templates` subcommand lists the
    available chart templates.

  - `opp_featuretool`: Extensive refactoring as well as refinement of its
    operation and command-line options. In general, the tool has become more
    forgiving: a missing `.featurestates` file or extra/missing entries in it
    no longer causes an error or warning. Missing file and file entries are
    initialized with the default enablement state; extra entries are preserved
    (in the hope they'll be useful, e.g. after switching to a different topic
    branch). The content of the `.nedexclusions` file is now also automatically
    adjusted without stopping with an error. User-visible changes are the following:
      - Removed the `validate` and `repair` subcommands (all commands implicitly
        validate and repair now).
      - Better validation of the `.oppfeatures` file: Detect and report duplicate
        feature IDs and unresolved feature dependencies.
      - Check for the existence of the C++ source root and NED folders.
      - Preserve unknown features in the `.featurestate` file instead of fail/warn
        about them. Reason: they often occur when switching between branches.
      - `isenabled -v` prints list of disabled features to stdout, not to stderr
      - Complain less about fixable problems.
      - Non-verbose operation by default.
      - Improved error/warning messages.

  - `opp_makemake`: The `makefrag` file is now included instead of copied
    into the generated makefile. Also, a `help` target has been added to the
    generated makefiles: typing `make help` now prints the list of accepted
    targets (`all`, `clean`, etc.) and user-settable makefile variables (`MODE`,
    `V`, `CFLAGS`, etc.), complete with helpful descriptions and usage examples.

  - `opp_test`: The set of possible test outcomes has been refined. Possible
    outcomes are now `PASS`, `FAIL`, `ERROR`, `SKIP` and `EXPECTEDFAIL`. `SKIP`
    means that the test was not performed, because e.g. it would test an
    optional feature which is currently disabled; `ERROR` means that the test
    was not performed because of an unrecoverable error (i.e. crash or
    exception). The diagnostic output for failed tests is now a colorized diff
    instead of a simple printout of the expected and the actual contents. It is
    possible to designate a test case as an "expected failure", by specifying
    `%expected-failure: <reason>` in the test file. This is useful if we know
    that a certain test case is correct, but it is not possible to fix it for
    some reason.

IDE / Analysis Tool:

  - The Analysis Tool in the IDE has been rewritten, using a new approach.
    The number one goals were to allow ARBITRARY computations to be performed
    on the data, VISUALIZE the result in the most appropriate form chosen from
    a multitude of chart/plot types, and produce PUBLICATION QUALITY output.
    Additional goals were an intuitive user interface that makes straightforward
    tasks easy and convenient, scalability that allows the processing of
    a large amount of simulation results inside the IDE, and facilities
    to make (most of) the UI's functionality available on the command line
    as well.

    The key technologies the new Analysis Tool employs are Python, NumPy,
    Pandas, and Matplotlib. The use of Python and Pandas gives access to
    advanced data manipulation and analysis capabilities, while Matplotlib
    offers dozens of plot types out of the box, endless customizations, and
    proven high-quality output. A large effort was made to properly integrate
    these technologies into the IDE, so that using the Analysis Tool provides
    a superior user experience, and brings the power of Python close to users
    without necessarily requiring them to write code in Python.

    Common charting tasks are accessible with a few mouse clicks, via
    predefined Chart Templates that can be configured using dialogs.
    The heart of all Chart Templates is Python code that performs all of the
    computation and plotting. The Python code behind a chart can be viewed
    and freely edited using an integrated language-aware editor. Matplotlib
    plots appear inside the Analysis Tool's UI, and they are live (i.e. not just
    images): zooming, panning, and interacting with possible embedded controls
    in the plot all work seamlessly. IDE-native plots can also be chosen
    as an alternative to Matplotlib, as they provide increased scalability at
    the cost of being less flexible and featureful.

    Special attention was paid to scalability. The UI is now easily capable of
    dealing with at least several tens of thousands of result files, and several
    million result items.

    The OMNeT++ Python API for querying, transforming and plotting simulation
    results is available in the form of Python packages for use in Python
    scripts outside the IDE as well. There is also a command-line tool
    (`opp_charttool`) that can "run" Analysis (anf) files e.g. for image export.
    `opp_charttool` can be integrated e.g. into the build process of a paper
    written in LaTeX.

    The documentation of the Analysis Tool's user interface can be found in the
    User Guide. A reference of the Python API has a dedicated Appendix in the
    Simulation Manual, while the "Result Recording and Analysis" chapter in the
    same Manual provides some practical guidance on how to use the API.

IDE / Sequence Chart Tool:

  - The visualization of events have been horizontally expanded on the chart, now
    it uses long rounded rectangles instead of small circles. This change allows
    the visualization of method calls without their arrows overlapping. If method
    calls are hidden then the chart falls back to the old behavior where events
    are small circles.

  - The set of displayed axes can be explicitly configured independently from the
    set of displayed events. The chart now also supports displaying events on a
    compound module axis if the corresponding submodule axis is hidden. Collapsing
    and expanding module axes can be done using the mouse.

  - The chart now also displays a set of axis headers on the left side that shows
    the module hierarchy in a compact and interactive way.

  - All parts of the sequence chart can be independently shown/hidden.

  - The selection has been extended with support for axes and method call arrows.

  - A new time measurement feature has been added that allows measuring the time
    difference between multiple selected events and other interesting points in time.

  - Several automatic configuration presets have been added (e.g. network level,
    full detail, default) to make the configuration for the most common use
    cases easier.

  - A new pattern-matching-based user-defined colorization (e.g. message sends,
    method calls, axes, events) feature has been added. This feature allows
    creating more easily understandable sequence charts by encoding certain model
    properties as colors.

  - The chart can display diagnostics information such as eventlog file
    statistics and operation statistics.

  - Added support for SVG export.

IDE / NED Documentation Generator:

  - The documentation generator now allows you to inject your own documentation
    fragments (possibly generated by 3rd-party tools) at various points in the
    documentation.

  - Modern look for the generated pages (Material design).

  - It is now possible to specify excluded directories during documentation
    generation, in order to be able to exclude samples, showcases etc. that
    are not integral part of the documentation.

  - Now generates `nedtags.xml` and `msgtags.xml` files which are compatible
    with the `doxytags.xml` file that Doxygen generates. This allows
    crosslinking to the NED documentation from a Sphinx-based documentation,
    using the doxy-link plugin.

  - Generate usage and inheritance diagrams only if they are meaningful.

  - Use SVG files as the usage/inheritance diagram image. Removed map
    files as SVG already contains proper links.

  - Generate "Implementors" table for interfaces. The Implementors table
    includes indirect implementors as well.

  - Fix: Inheritance diagrams were missing in module interface pages,
    even if they had implementors. This bug made INET documentation much less
    useful.

  - The functionality of the documentation generator is now also available
    from the command line (`opp_neddoc`).

IDE / Inifile Editor:

  - Added support for ignorable options. User can mark custom config options
    as ones to be ignored by the editor.

  - Config option values that contain expressions are no longer falsely
    flagged as errors.

  - The confusing "Unused entry (does not match any parameters)" warning that
    often appeared falsely for INET simulations due to deficiencies in
    analyzing the network structure has been re-worded to include the
    possibility that it is false, and has also been demoted from being
    "warning" to "info".

  - Eliminated the asynchronous/delayed form page reread in Form mode, which
    often caused editing glitches.

IDE Miscellaneous:

  - Install Simulation Models dialog: Added support for changing the project
    name and location on download.

  - Turned on showing the line numbers ruler in source editors by default.

  - Added an XML editor, Terminal (a view that contains a shell prompt), PyDev
    (Python editor), and the Eclipse Marketplace client to the IDE.

  - The simulation launcher now passes NED exclusions to the simulation program
    as `-x` options. NED exclusions typically come from disabled project features,
    e.g. in INET.

  - Added SVG export capability to the NED Editor.

  - Updated to Eclipse 4.22, CDT 10.5 and PyDev 9.2.

  - The JRE is now provided by the Eclipse JustJ project, so the IDE no longer
    requires Java to be installed on the host OS.

  - Added support for ARM-based Linux distros.


C++ Debugging:

  - Better debugging experience by fine-tuning the compiler options, especially
    with the clang compiler.

  - Added LLDB pretty printers for various OMNeT++ types. They can be useful
    if you use LLDB-based external debuggers like XCode or VS Code. You should
    manually import them from the LLDB debugger console with the following
    command:

        command script import <OMNETPP_ROOT>/python/omnetpp/lldb/formatters/omnetpp.py

  - Allow Qtenv's "Debug on Error" and "Debug Next Event" functionality to use
    the integrated debugger of the Simulation IDE. This required multiple
    changes. First, the IDE was extended to accept an URL which, when opened in
    the IDE, causes the integrated debugger to start a debug session and attach
    to a process given with its PID. This URL is:

    `omnetpp://cdt/debugger/attach?pid=<pid>`

    The URL can also be opened from the command line, by running the `omnetpp`
    command with the URL as argument. The command opens the URL in the existing
    IDE instance if it is already running, or starts a new one if it does not.

    The second change was to update the default value of the OMNeT++
    `debugger-attach-command` configuration option to use the above command.
    (Previously it has used various other debuggers which were likely to be
    found on the host OS: GDB, Nemiver, VS Code, etc.)

    Hint: You can force Qtenv to use Visual Studio Code as your external
    debugger by setting the following environment variable:

        export OMNETPP_DEBUGGER_COMMAND="code --open-url \"vscode://vadimcn.vscode-lldb/launch/config?{request:'attach', pid:'%u'}\""

    Alternatively, you can specify the same value (`code --open-url ...`)
    to the "debugger-attach-command" configuration option in `omnetpp.ini`.

Result file format changes:

  - There were a number of changes in the format of result files (.sca and
    .vec files), so the file format version has been bumped from 2 to 3. (The
    file format is recorded at the top of each result file in a `version` line.)
    The set of changes is relatively small; see details below.

  - Iteration variables are no longer saved as run attributes but in separate
    `itervar` lines, in order to be able to tell them apart with certainty.

  - `param` lines (which recorded parameter assignment entries in the
    configuration) have been replaced with the more general `config` lines.
    `config` lines record all configuration entries, not just parameter assignments.

  - In version 2, concrete parameter values (if requested) were recorded as
    scalars, whereas in version 3 they are recorded as a separate result item
    type, in `par` lines. This allows the recording of volatile parameters (as
    expressions) and non-numeric values as well.

  - Component NED type names are now recorded, as `typename` pseudo parameters.

  - The `sum` and `sqrsum` fields of weighted statistics are no longer recorded,
    as they are irrelevant.

SQLite result file format changes:

  - SQLite result files underwent a similar change in the database schema. Files
    in the old format are no longer understood. A brief summary of changes follows.

  - The `runParam` table was renamed to `runConfig`, and its columns were
    also similarly renamed: `paramKey`, `paramValue` and `paramOrder`
    became `configKey`, `configValue` and `configOrder`, respectively.

  - Iteration variables are in a new table called `runItervar`, with columns
    `runId`, `itervarName` and `itervarValue`.

  - Two new tables were added for representing the new result item types:
    `parameter` (with the columns `paramId`, `runId`, `moduleName`, `paramName`
    and `paramValue`), and `paramAttr` (with the columns `paramId`, `attrName`
    and `attrValue`). Note that `paramValue` contains values in the syntax it
    would have to be written in a NED file or in `omnetpp.ini`, i.e. string
    constants include the quotation marks.

Eventlog file format and configuration:

  - The eventlog file format has been changed substantially. You won't be able
    to open older eventlog files with this version of the IDE, nor new files in
    previous IDE versions.

  - The most prominent change is the introduction of snapshot and index chunks in
    addition to the already recorded event chunks. The former contains a complete
    snapshot of the relevant simulation state without referring to any other line
    in the file. The latter provides the set of changes in the relevant simulation
    state since the last index or snapshot chunk. The contents of the index chunk
    is expressed as references to other lines of the eventlog file. This change in
    the eventlog format provides the foundation for a better user experience in
    the IDE especially for large eventlog files.

  - Several new eventlog-recording-related configuration options have been added:

    - The `eventlog-snapshot-frequency` configuration option specifies how often
      snapshots are recorded in the eventlog file. Snapshots help various tools to
      handle large eventlog files more efficiently. Specifying greater value means
      less help, while smaller value means bigger eventlog files.

    - The `eventlog-index-frequency` configuration option specifies how often
      indices are recorder in the eventlog file. An index is much smaller than a
      full snapshot, but it only contains the differences since the last index.
      Specifying greater value means less help, while smaller value means bigger
      eventlog files.

    - The `eventlog-max-size` configuration option specifies the maximum size of
      the eventlog file in bytes. The eventlog file is automatically truncated
      when this limit is reached.

    - The `eventlog-min-truncated-size` configuration options specifies the minimum
      size of the eventlog file in bytes after the file is truncated. Truncation
      means older events are discarded while newer ones are kept.

    - The `eventlog-options` configuration option allows for recording only
      certain categories (e.g. text, message, module, method call, display
      string), to reduce the eventlog file size.

Build environment:

  - If a file called `setenv_local` is present in the installation root, it is
    automatically sourced from the `setenv` script. `setenv_local` can be used
    to set up user specific environment variables.

  - The content of the `$PLATFORM` makefile variable has changed. Until now,
    it contained a `<platform>.<architecture>` pair (e.g. `win32.x86_64`) which was
    misleading. From now on, `Makefile.inc` provides `$PLATFORM` (e.g. `win32`)
    and `$ARCH` (e.g. `x86_64`) as separate variables. `makefrag` files must be
    updated if `$PLATFORM` variable was used in them.

  - To build OMNeT++ with Akaroa support, `WITH_AKAROA=yes` must be specified
    in the `configure.user` file. The configuration script no longer tries to
    autodetect Akaroa without the user explicitly requesting it.

  - osgEarth support is now disabled by default. You must explicitly set
    `WITH_OSGEARTH=yes` in the `configure.user` file. Some Linux distibutions
    no longer include osgEarth so you may need to build it manually
    from sources.

  - Debug builds are now compiled with -O0 for maximum debuggability. Release builds
    are compiled with `-O2 -ffp-contract=off` to disable fused multiply-add operations.
    This ensures that floating point operations will generate exactly the same results
    in DEBUG and RELEASE mode and their accuracy would not depend on unpredictable
    optimizations.

Windows Support:

  - On Windows, the toolchain (compiler, libraries, etc.) can now be found in
    `tools/win32.x86_64`.

  - Setting of environment variables has been moved into the `setenv` script.
    It is automatically sourced when `mingwenv.cmd` or `vcenv.cmd` is started.

  - On Windows, libraries and other dependencies used by simulations, such as
    Qt, OSG, osgEarth, etc., are now separated from the rest of the toolchain
    binaries (compiler, etc.). You can find them in the `opt` subdirectory of
    the `tools/win32.x86_64` folder.

  - On Windows, when creating a shared library, the build system now generates a
    proper import library and also a module definition file
    (`<targetname>.dll.a`, `<targetname>.def`). External projects must link
    against that library instead of linking directly with the `<targetname>.dll`
    file. (Note that the linker automatically uses the `.dll.a` files if they
    are present in the library path and falls back to use the pure `.dll` file
    only if the import library is not present. It means that you don't have to
    do anything extra to use these files apart from making them available along
    your `.dll` files.)

  - The Windows toolchain now contains the (much faster) LLD linker. LLD is also
    used on other platforms instead of the GNU linker, if available. For large
    projects, LLD is dramatically faster than the standard GNU linker. In one
    instance, the use of LLD, together with other changes that were partly done
    in INET, reduced INET linking time on Windows from 380s to 2s in DEBUG mode,
    and from 90s to 5s in RELEASE mode on one of our dev boxes.


macOS support:

  - On macOS, the toolchain (compiler, libraries, etc.) can now be found in
    `tools/macos.x86_64`.

  - Rudimentary support for Apple silicon: `source setenv` now automatically
    launches a (nested) x86_64 emulation shell on ARM-based Macs.

Sample simulations:

  - Added "openstreetmap", an example simulation that displays a city map, with
    some animated car traffic on it. Map data come from an OSM file exported
    from openstreetmap.org.

  - Added "petrinets", an example simulation demonstrating how to implement
    stochastic Petri nets, a well-known formalism for the description of
    distributed systems.

  - Added the "wiredphy" sample simulation to demonstrate transmission updates.

  - The "fifo" example simulation was extended with a TandemQueues network and
    related configurations in omnetpp.ini.

  - The "resultfiles" folder project now contains scalar and vector result files
    from the Fifo1, Fifo2, and the new TandemQueues simulations.

Documentation:

  - Simulation Manual has been updated. The largest changes were in the "Message
    Descriptions" and "Result Analysis" sections (which were practically
    rewritten). There are also two new appendices: "Python API for Chart Scripts",
    and "Message Class/Field Properties".

  - User Guide: The chapter about the Analysis Tool ("Result Analysis") has been
    rewritten.

  - Install Guide has been updated. New sections in the macOS chapter: "Enabling
    Development Mode in the Terminal", "Running OMNeT++ on Apple Silicon". We
    also added instructions on installing OMNeT++ on WSL (Windows Subsystem for
    Linux) on Windows 10, which is important because we found that running
    OMNeT++ on WSL is considerably faster than using it with the MinGW
    toolchain.

  - Converted all of our structured documents (User Guide, Install Guide, etc.)
    from their original source format (DocBook or AsciiDoc) to reStructuredText,
    except for the Simulation Manual which remains in LaTeX.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A6.0


OMNeT++ 5.7.1 (August 2023)
---------------------------

This is a modernized maintenance release of omnetpp-5.7.

Changes:

 - Source code updated to eliminate warnings issued by modern C++ compilers.
   Backported several smaller fixes.

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.


OMNeT++ 5.7 (Sept 2021)
-----------------------

This version is intended to be the last release of the 5.x series. The main
purpose of this release is to make it possible to write model code, primarily
NED, which is also compatible with the upcoming OMNeT++ 6.0. It also contains
several bug fixes backported from the 6.0 branch.

For compatibility with 6.0:

  - NED: support for "parent.foo" syntax of parameter references (and other
    references too, see below) in expressions. Using the "parent" qualifier is
    only allowed inside submodule and connection blocks (within the curly
    braces), but not on compound module level. This change is necessary for
    being able to write NED files which are compatible with both OMNeT++ 5.7+
    and 6.x. The interpretation of parameter references in expressions have
    changed in 6.0. In 6.0, a "foo" parameter reference in a submodule means the
    parameter of the same submodule ("this.foo"), while in 5.x it means the
    parameter of the enclosing compound module (which would be expressed in 6.0
    as "parent.foo").

    The way to write NED files compatible with both is to always explicitly
    qualify the parameter with "this" or "parent". In a NED file written for
    5.x, prefix plain parameter references inside subcomponent curly brace blocks
    with "parent." to ensure that their meaning doesn't change in version 6.0.

    The same change applies to submodule and gate references as well, e.g. in
    the argument of exists() and sizeof().

    The change also affects the interpretation of references in expressions that
    occur in ini files.

    Example:

        network Network {
          node: Node {
            foo = foo; // CAVEAT: means "parent.foo" in 5.x, "this.foo" in 6.x!
            bar = this.foo;  // OK: unambiguous, recognized by all versions
            faa = parent.foo;  // OK: unambiguous, recognized in 5.7 and 6.x
            baz = node2.foo;  // CAVEAT: means "parent.node2.foo" in 5.x, "this.node2.foo" in 6.x!
            bax = parent.node2.foo;  // OK: unambiguous, recognized in 5.7 and 6.x
          }
        }

  - MatchExpression: added `field =~ pattern` syntax as alternative to `field(pattern)`

  - Added Ws, Wh, kWh, MWh to the list of recognized measurement units

Core:

  - Statistic recording: Added a way to disable auto-adding of the warmup filter.
    One needs to specify "autoWarmupFilter=false" in the @statistic for that.
    This opens the possibility for the user to add warmup filters to a custom
    place, not just before all other filters. E.g. consider "min(warmup(foo))"
    (which is produced by auto-adding of the warmup filter), and "warmup(min(foo))"
    which might be what the user wants instead.

  - Fingerprints: Fixed a bug in cHasher::add(const char *) which caused some bits
    in the input not affect the fingerprint. This bugfix does not affect the
    fingerprint of most simulations, because very few fingerprints use strings
    as input.

  - Canvas: In cPathFigure, path parsing was refined: If an op char is missing,
    assume previous one, in accordance with the SVG specification.

  - cHistogram and histogram strategy classes: work around several problems
    caused by the finite precision of 'double'.

  - Miscellaneous other bug fixes.

Cmdenv:

  - Added Fake GUI mode. Fake GUI means that refreshDisplay() is called
    periodically during simulation, in order to mimic the behavior of a
    graphical user interface like Qtenv. It is useful for testing simulation
    models with visualization using Cmdenv, e.g. in smoke or fingerprint tests.
    Fake GUI can be enabled with the cmdenv-fake-gui=true configuration option.

    The set of currently supported options for Fake GUI:
    cmdenv-fake-gui-before-event-probability,
    cmdenv-fake-gui-after-event-probability,
    cmdenv-fake-gui-on-hold-probability,
    cmdenv-fake-gui-on-hold-numsteps,
    cmdenv-fake-gui-on-simtime-probability,
    cmdenv-fake-gui-on-simtime-numsteps,
    cmdenv-fake-gui-seed.

Qtenv:

  - Added Ctrl+W as shortcut for closing toplevel inspector windows.

  - Made the "Run" and "Fast" buttons "pop out" (and stop simulation) if
    triggered again. This adds a radiobutton-like behavior to these actions,
    which is handy for running the simulation for a very short time, with a
    single button.

  - Log Inspector: Perform "find" on plain text comment, i.e. ignore formatting
    escape sequences.

  - Log Inspector: After a search, scroll the found text into the middle of the
    viewport instead of just to the edge.

  - Find dialog: Select the entire searched text when opening the find dialog,
    to allow easier and quicker typing of a different query.

  - Further bug fixes.

Tools:

  - JsonExporter: Fix: In the exported file, result items appeared under
    multiple runs due to missing filtering. Applies to both the IDE and opp_scavetool.

  - In the Simulation Manual, the section about opp_scavetool was out of date.

Simulation IDE:

  - Welcome screen theme changed (circle -> solstice) due to problems
    with dark theme with the old 'circle' theme.

  - The Download Simulation Models dialog now supports specifying a custom
    project name and location. Also, the default project name is now coming from
    the downloaded descriptor file (not from the .project file in the archive.)

  - Fixed the download issues in the Download Simulation Models dialog.

  - NED, Ini Editor: Syntax highlight for the NED "parent" keyword

  - Ini Editor: group fingerprint and fakegui-related fields on a separate new
    form page.

  - NED Editor: Fix rendering issue with the mouse cursor for the creation tool
    in the palette.

  - Launcher: Print working directory and command line to console when
    debugging simulations, similar to the simple "run" mode launch.

  - Launcher: On macOS, use lldbmi2 for debug launches by default.

  - Makefile generation brought in sync with omnetpp-6pre11.

Build:

  - Use Eclipse 4.21, and JustJ as the private JRE.

  - Use python3 as the interpreter for Python scripts.

  - opp_makemake: Makefile generation brought in sync with omnetpp-6pre11.

  - Got rid of a few bison warnings and properly test for Bison 3.

  - Fixes for the Windows build.

  - Updated bundled sqlite3 amalgamation sources.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.7


OMNeT++ 5.6.3 (August 2023)
---------------------------

This is a modernized maintenance release of omnetpp-5.6.2.

Changes:

 - Source code updated to eliminate warnings issued by modern C++ compilers.
   Backported several smaller fixes.

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.


OMNeT++ 5.6.2 (May 2020)
------------------------

This release contains minor stability fixes.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.6.2


OMNeT++ 5.6.1 (February 2020)
-----------------------------

Minor bug fixes.

 - Fix makefile template glitch in the IDE which caused unnecessary rebuilds
   of the projects in IDE.

 - Added missing .cproject file for the canvas example.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.6.1


OMNeT++ 5.6 (January 2020)
--------------------------

This is primarily a bugfix release, with a small but practically quite useful
NED feature (`@reconnect`) and minor additions to the simulation library.
Regarding bugfixes, Qtenv has received quite a lot of attention.

NED:

  - The new @reconnect property allows reconnecting already connected gates
    by specifying it in the new connection's body. This is especially
    useful in INET Framework, e.g. because it eliminates the need for "hook"
    modules in compound modules that represent network interfaces.

Simulation library:

  - SimTime: Added the fromRaw() method.

  - cGate: Added the isGateHalf() and getOtherHalf() methods.

  - The deleteModule() method was moved from cSimpleModule to cModule, revised,
    and its functionality extended: it is now allowed for a running module
    to delete itself, also as part of a module tree. (Note: direct deletion
    of a module or channel object, i.e. via the "delete" operator, is still
    not allowed.)

  - cLabelFigure: Added rotation support (setAngle() method).

  - cAbstractHistogram: Fixed the getCDF() method which was broken.

  - cFigure: Fixed a bug that caused adding figures to be O(n) or slower,
    causing a huge performance penalty with 10000 or more figures.

  - Several further smaller improvements.

Qtenv:

  - Added a "Debug Now" option to the Simulate menu.

  - Allow dragging submodules around by holding Shift. (This works by changing
    the coordinates in the display string.)

  - Added option to export module graphics to an image, and a dialog to
    select the exported area.

  - Start the animation at t=0, not right before the first event.

  - Add some sideways offset to methodcall animation lines and text, to
    reduce overlap with connection arrows.

  - Connection arrows: Make the "m=[nesw]" display string tag work better
    with border-to-border connections.

  - Align the lines of info text ("t" display string tag) of submodules and
    connections appropriately in the left/top/right positions.

  - Performance: cache min(animSpeed) in cCanvas, so that Qtenv doesn't
    have to compute it every time.

  - Set a busy indicator (spinny cursor) during more potentially long
    operations.

  - When the layouting process takes a long time (more than five seconds),
    ask the user what to do (wait or finish).

  - Added an option to disable logging from refreshDisplay().

  - Default log prefix format changed to use %K instead of %C. (%K only prints
    the context component if it is different from the event's module; %C always
    prints it.)

  - Log prefix format: Added the %< (trim preceding whitespace) directive.

  - Keep caret (cursor) near the same event when switching Log Inspector modes.

  - Removed the unused scrollback limit option. Updated the Preferences
    dialog accordingly.

  - Dozens of further bug fixes and improvements.

IDE:

  - Updated to use Eclipse 4.14, CDT 9.10.

  - IDE: Made the "Project Features" menu item more creative in finding
    the project. (It said "Select a project first" much too often.)

  - Fix weighted histogram PDF/CDF computation (use sum of
    weights instead of observation count).

  - NED editor: added manual connection routing using the "m" tag.

  - SequenceChart: Avoid out of bounds error when selecting manual axis
    ordering, or switching between "Manual" and "Minimize crossings" modes.

  - EventlogTable, SequenceChart: Strip ANSI escape sequences from log
    message lines.

  - Let the user permanently turn off the "OMNeT++ libraries not yet
    compiled" dialog.

  - Some further fixes and improvements.

Other:

  - scavetool: Fixed vector file indexing bug that caused certain blocks
    to be left out from the index. This also affects the Analysis Tool in
    the IDE, which uses the same code for result file access.

  - opp_test, opp_featuretool: Various small fixes.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.6


OMNeT++ 5.5.2 (August 2023)
---------------------------

This is a modernized maintenance release of omnetpp-5.5.1.

Changes:

 - Source code updated to eliminate warnings issued by modern C++ compilers.
   Backported several smaller fixes.

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.


OMNeT++ 5.5.1 (June 2019)
-------------------------

This release contains minor stability fixes.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.5.1


OMNeT++ 5.5 (June 2019)
-----------------------

This release contains minor fixes. See the individual ChangeLogs for details.

Core:

  - Added the intuniformexcl() NED and C++ function. It returns a random integer
    with uniform distribution over [a,b). that is, including a and excluding b.

  - The contents of resultfilters.h and resultrecorders.h are now public API,
    i.e. part of <omnetpp.h>. This change allows one to derive new result filters/
    recorders by subclassing the built-in ones.

  - A description string can now be specified when registering new result filters
    and recorders. New registration macros have been added for this purpose.

  - cXMLElement was optimized to use less memory when large files are loaded.

  - cHistogram: Now collects the number of positive and negative infinity values
    separately.

  - SimTime: Added preciseDiv() for a precise division of an integer and a
    simulation time.

  - cQueue, cPacketQueue: added a constructor that accepts a comparator
    object (not just a function).

  - cModule: Added containsModule() method.

Other:

  - Several Qtenv-related fixes and improvements

  - Added support for osgEarth 2.10 and above

  - Updated IDE to Eclipse 4.11. This resolves compatibility issues with JDK 11.

  - Workaround for long C++ indexer runtimes in the IDE.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.5


OMNeT++ 5.4.2 (August 2023)
---------------------------

This is a modernized maintenance release of omnetpp-5.4.1.

Changes:

 - Source code updated to eliminate warnings issued by modern C++ compilers.
   Backported several smaller fixes.

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.


OMNeT++ 5.4.1 (June 2018)
-------------------------

This release contains minor fixes. See the individual ChangeLogs for details.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.4.1


OMNeT++ 5.4 (June 2018)
-----------------------

This release contains last-minute features and improvements, mostly motivated
by the upcoming INET 4 release. Highlights are the typename and exists()
operators in NED; nan and inf keywords in NED; support for logarithmic units
like dB, dBW, dBm, dBV, dBmV. Qtenv has also gained much more powerful
just-in-time debugging capabilities than before.

Details follow.

NED:

  - Allow 'typename' in expressions. Motivation: we want to be able to write:
    `foo: <> like IFoo if typename!=""`

  - Added the `exists` operator. Syntax: `exists(<submodulename>)`

  - Introduced `nan` and `inf` as keywords for numeric constants

Core:

  - SimTime now has a new method named ustr() that prints the time in an
    appropriate unit. It is now used in cClassDescriptor for displaying
    simtime_t fields and at a few other places.

  - SimTime: more precise computation for integer-SimTime division.

  - Measurement units are now available as NED functions as well. They
    accept dimensionless numbers and quantities with a compatible unit.
    E.g. the expressions mW(2*100) and mW(0.2W) both result in 200mW.

  - New measurement units: K, Ohm, mOhm, kOhm, MOhm, uW, nW, pW, fW,
    THz, kmps, binary and decimal multiples of bit (Kib, Mib, Gib,
    Tib, kb, Mb); deg, rad; pct (percent), ratio.

  - Support for logarithmic units: dBW, dBm, dBV, dBmV, dBA, dBmA, dB.
    Conversion between logarithmic and corresponding linear units (W vs. dBW)
    is supported. Conversion between bit and byte (and multiples) is
    also supported.

  - cPacket: refined str() method

  - Several smaller improvements.

Qtenv:

  - Debug-on-error functionality can now be turned on interactively, via the
    Simulate -> Debug on Errors menu item.

  - When an error occurs and debug-on-error is enabled, Qtenv now offers to
    launch and attach an external debugger unless the simulation program is
    already being debugged. The same applies to the Debug Next Event
    functionality. External debuggers can be configured with the
    debugger-attach-command configuration key, and can also be overridden
    with the OMNETPP_DEBUGGER_COMMAND environment variable.

  - Workaround for a crash with Qt 5.11.

IDE:

  - Launcher: fix: The IDE sometimes started build in the UI thread,
    locking up the UI for the time of the build, potentially for several
    minutes.

  - Fixed "unknown" image as module background in the graphical NED editor.

  - Fix: do not report "undefined parameter" for typename assignments in NED

  - Fix: inifile analysis: typename deep assignments in NED were ignored

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.4


OMNeT++ 5.3.1 (August 2023)
---------------------------

This is a modernized maintenance release of omnetpp-5.3.

Changes:

 - Source code updated to eliminate warnings issued by modern C++ compilers.
   Backported several smaller fixes.

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.


OMNeT++ 5.3 (April 2018)
------------------------

Highlights of this release are an improved message compiler (required for INET 4),
much improved histogram support, a more powerful cMessagePrinter API and its
implementation in Qtenv, and better support for smooth animation in Qtenv.
Details follow.

Core:

  - Revised message compiler, added import support and more.
    See src/nedxml/ChangeLog for details. Use --msg6 to turn on
    the new features.

  - cPar: integer representation changed from long to int64_t.
    See src/include/omnetpp/ChangeLog for details.

  - Evaluation of NED expressions now uses integer arithmetic when operands
    are integers. Conversions that incur precision loss should now be explicit:
    converting a double to integer requires the int() cast; also, converting
    an int to double will raise an error if there is actual precision loss,
    and you can suppress that error by explicit double() cast. There are many
    smaller-scale changes related to this change -- see include/omnetpp/ChangeLog
    for details.

  - cHistogram has been replaced with new implementation. (The old one is still
    available under the name cLegacyHistogram.) The new cHistogram is more
    general (supports arbitrary bins), more configurable, and produces much
    higher quality histograms even in the default setup due to newly introduced
    techniques such as adaptive precollection, bin size rounding and bin edge
    snapping, auto-extension with new bins, bin merging to keep the number of
    bins optimal, etc. Histogram operation can be customized via histogram
    strategy classes (cIHistogramStrategy); several histogram strategies are
    provided.

  - In recording histograms with @statistic, the number of bins can be specified
    by adding numBins=nn: `@statistic[x](record=histogram;numBins=100)`. The actual
    number of bins produced might slightly differ, due to auto-extension and
    bin merging during result collection.

  - cMessagePrinter: API was extended with tags and column names

  - cCanvas: added getAnimationSpeed(); better documentation for
    setAnimationSpeed() and holdSimulationFor().

  - cHistogram, cNedValue: minor changes in the public API

  - Result recorders: Added "timeWeightedHistogram" recorder

Qtenv:

  - Implemented support for the improved cMessagePrinter Options API, and added
    a respective configuration dialog.

  - The log viewer now supports ANSI control sequences for text styling:
    foreground/background color, and bold/italic/underline text. Styling is
    supported in both message history view and log view, i.e. it can be used
    in text returned from cMessagePrinter and logged by the EV and other logging
    macros.

  - Rewritten the controller for smooth animation. The new algorithm is now able
    to scale linearly, as long as the simulation can keep up.

  - Usability improvements in the Animation Parameters dialog, e.g. it now
    also displays the current simulation speed, and it is colored red if the
    simulation cannot keep up with the requested animation speed.

  - Histogram inspector: visualize outlier bins (drawn with a different
    color than normal bins), and show info about them. Setting up the bins while
    in the precollection phase can be forced from the GUI.
    Fix numeric overflow when large numbers (>= 2^64) are shown.

  - Histogram result objects are easier to locate on the GUI, and are now
    also found by the Find/Inspect dialog.

  - Made statistic recorders "findable" from Qtenv

Tkenv:

  - The Tcl/Tk based runtime interface is now deprecated and turned off by
    default. It will be removed in future OMNeT++ versions. You can still enable it
    in configure.user by setting WITH_TKENV = yes, but we strongly suggest using
    Qtenv instead.

Toolchain and dependencies:

  - Updated toolchain on Windows (clang 5.0.1, gcc 7.3, gdb 8, osgEarth 2.7)

  - Updated toolchain on macOS (OpenSceneGraph 3.2.3, osgEarth 2.7)

  - OMNeT++ now requires osgEarth 2.7 or later (check the Install Guide for
    further instructions on how to upgrade osgEarth.)

Tools:

  - opp_runall, opp_fingerprinttest, opp_test: portability fixes for Windows
    and macOS

  - opp_fingerprinttest: fix: error messages from the simulation did
    not appear

  - 'march=native' and 'mtune=native' compiler options are no longer used
    by default for a release build, because on certain CPUs this caused rounding
    differences so simulations ran on a different trajectory depending on the
    actual CPU used.

Samples:

  - canvas: Added smooth animation with CarAnimator::refreshDisplay().

  - osg-earth, osg-satellites: Switch from ObjectLocator to GeoTransform.
    The former was removed after osgEarth 2.8, and the latter has been
    around for quite some time now.

Documentation:

  - Documented smooth animation in detail, and updated the section on statistical
    data collection classes (cStdDev, cHistogram) in the Simulation Manual.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.3


OMNeT++ 5.2.2 (August 2023)
---------------------------

This is a modernized maintenance release of omnetpp-5.2.1.

Changes:

 - Source code updated to eliminate warnings issued by modern C++ compilers.
   Backported several smaller fixes.

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.


OMNeT++ 5.2.1 (December 2017)
-----------------------------

This is a maintenance release.

 - Minor bug fixes and improvements in Qtenv.

 - Other minor bug fixes.

 - Updates to the C++ Development and Launcher chapters of the User Guide.

See ChangeLogs in individual folders for details.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.2.1


OMNeT++ 5.2 (September 2017)
----------------------------

Highlights of this release are the fine-tuning of result recording and processing
(especially around weighted statistics); many bug fixes and improvements in Qtenv;
makefile changes to allow DEBUG and RELEASE builds of models to co-exist, and
several related changes in the IDE.

Core:

 - Figure zIndex is now additive: the effective zIndex is now the sum of the
   zIndex values of the figure and all its ancestors up to the root figure.
   This provides more flexibility, as the stacking order of figures
   is no longer constrained by their position in the figure tree.

 - Added cPanelFigure, a new figure type that turns off zoom for its children.
   cPanelFigure is handy when one needs to do relative positioning unaffected
   by zoom, for example when adding decorations next to submodule icons.

 - Statistics recording: added support for collecting time-weighted
   statistics. This is useful for variables like queue length. Add
   timeWeighted=1 or timeWeighted=true to a @statistic to make it collect
   time-weighted statistics. timeWeighted=1 affects the operation of the
   following filters/recorders: "mean", "stats" and "histogram". The "timeavg"
   recorder always computes time average; an additional recorder "avg" has
   also been added that always computes unweighted mean. "mean" emulates
   either "avg" or "timeavg", depending on the presence of the timeWeighted=1
   option.

 - Changed result filters/recorders so that they interpret NaN as "missing data"
   and ignore it. The change affects "count" (which now also ignores nullptrs),
   "sum", "min", "max", "avg", "timeavg", "sumPerDuration", "stats", "histogram".
   For the time weighted case, NaNs mark intervals to be ignored.

 - cResultFilter: added an init() method to allow filters access the content
   of the @statistic property they occur in.

 - The packetBytes/packetBits filter now throw error if object is not a cPacket.
   They still accept (and ignore) nullptr.

 - Added support for weighted statistics to histogram classes and to cStddev
   (the cWeightedStddev class is no longer needed.) The primary motivation
   for weighted statistics is the existence of variables where time average
   makes much more sense than arithmetic mean, e.g. queue length.

 - The OSG viewer was factored out from Qtenv into a separate support library
   which is only loaded at runtime on demand. Core OMNeT++ no longer has
   any dependence on OSG or OSGEarth. This change improves simulation
   startup times and also debugging experience, as it eliminates the loading
   of the huge number of shared libraries that OSG and OSGEarth depends on.
   (The library loading overhead was also present in simulations that did not
   even contain 3D visualization.) The only API change is that one must use
   cOsgCanvas::EarthViewpoint instead of osgEarth::Viewpoint when using
   the cOsgCanvas::setEarthViewpoint().

Build:

 - opp_makemake: IMPORTANT CHANGES regarding debug/release-mode builds:

   - Models are built in RELEASE mode by default. Until now, DEBUG was the
     default, which resulted in casual users always running their simulations
     in DEBUG mode, i.e. much slower than possible.

   - RELEASE and DEBUG mode binaries can now co-exist, so users do not have
     to recompile when they switch modes (e.g. when they want to debug a
     simulation.) This is a significant gain especially for large models
     like INET. The goal was achieved by adding the "_dbg" suffix to the
     names of DEBUG-mode binaries so they don't collide with RELEASE-mode
     ones.

   - Binaries are now hard-linked from the build directory (out/) to the
     target directory instead of being soft-linked. This leaves the copy
     in the target directory intact when the one in the build directory
     is deleted.

 - Refinements on the build process of OMNeT++ itself: generated files are
   created only once, even when using parallel build; do not copy build
   artifacts if not actually changed; Makefiles in the samples/ folder
   are re-created only if they are missing; etc.

 - The samples/ folder is now optional: deleting it in an OMNeT++ installation
   will no longer break the build.

IDE/Base:

 - The Eclipse platform was updated to the latest Oxygen build (4.7.1)

IDE/Build:

 - Makefile generator: follow changes in the command-line opp_makemake tool

IDE/Launching:

 - The Run, Profile and Debug launch types now automatically trigger build.

 - The launcher now switches the project (and optionally all projects it
   depends on) to the appropriate build configuration if necessary: Run
   and Profile will perform RELEASE build, Debug will perform DEBUG build.

 - For Debug launch, the "_dbg" suffix is added implicitly to the target
   name (only for the exe files). The simulations will also load the "_dbg"
   versions of libraries when compiled to DEBUG.

 - A new "Build before launch" section was added to the Launch config
   dialog, which allows you to control whether to switch build configuration
   automatically or to ask before the build. The scope of build can also
   configured; options are: "None", "This project", or "Project + dependencies".

 - If the launch is configured as "Ask before build" (default), a
   confirmation dialog is shown on launch (and before the build), asking
   whether to switch the active build configuration to the appropriate one.
   The dialog allows you to set the decision as permanent and reconfigures
   the launch config accordingly.

 - Better progress reporting and cancellation for batch runs. Internally,
   this is the result of switching to the new JobGroups API of Eclipse.

IDE/Analysis Tool:

 - Added support for "statistic" items and for weighted statistics.

   Background: Specifying "record=stats" in a @statistic NED property, or
   or calling the record() method on a cStddDev object saves a "statistic"
   object in the output scalar file. A "statistic" object includes fields
   like the count of observations, mean, standard deviation, minimum
   and maximum value. "statistic" objects so far have been blown up to
   several unrelated scalars upon loading into the Analysis Tool. Now they
   are loaded as objects, and appear in the Histograms and All tabs of the
   Browse Data page. Statistics fields continue to be available as scalars
   as well, so they can be used as chart input.

   Also, weighted statistics (like those saved from cWeightedStddev) were
   not properly displayed in the Analysis Tool. This has been rectified
   as well.

   Details:
   - The Histograms tab on the Browse Data page now includes "statistics"-
     type results as well.

   - The Histograms tab now has new table columns: "Kind", "SumWeights",
     "#Bins", "Hist.Range", all displayed by default. "Kind" indicates
     whether a result item is a "statistic" or a "histogram", and whether
     it is weighted or unweighted.

   - Individual fields of "statistic" and "histogram" result items are
     no longer displayed on the Scalars page.

   - Properties for "statistic" and "histogram" result items now includes
     "Kind" ("weighted"/"unweighted") and "Sum of weights".

   - On the All tab of Browse Data page, result item fields are now in
     natural order (i.e. no longer alphabetically sorted).

 - Export dialog refined: Added "Open exported file afterwards" checkbox,
   better filename generation and validation, etc.

 - Changed the syntax for accessing iteration variables in filter
   expressions. Before, they could be accessed like run attributes,
   with the "attr:" prefix. Now, one needs to use the "itervar:" prefix,
   which is more explicit.

 - Properties view: properly display attributes, iteration variables and
   parameter assignments for runs

 - Better display of histogram bins in the Properties view.
   The display format of bins was changed from "a..b" to "[a,b)", in order
   to clearly indicate that "a" is inclusive and "b" is exclusive.

 - The "Variance" line has been removed from Properties view because
   it's not too useful and can be easily computed as the square of the
   standard deviation.

 - Missing or unavailable data in table cells is now displayed with a
   hyphen instead of "n.a."

 - New icon for scalar items on the All tab of Browse Data page

Qtenv:

  - OSG Viewer is now a separate library, loaded only on demand.

  - Improved the appearance and usability of the Animation Parameters dialog.

  - Show (debug) or (release) in the window title based on NDEBUG.

  - Completely overhauled figure rendering. This should improve performance
    and fix many issues.

  - The size of arrowheads on connections and line figures is now more
    reasonable with many combinations of zoom level, line width, and the
    "zoomLineWidth" property.

  - Implemented cPanelFigure support and cumulative zIndex.

  - Figures without a tooltip inherit it from their nearest ancestor
    that has one. Empty strings break this inheritance, but are not shown.
    The own tooltip of the figure now overrides that of its associated object.

  - The special value "kind" is now also accepted as a color in the "i" tag
    of display strings of messages as well.

  - A large number of miscellaneous improvements, fixes and cleanups;
    see src/qtenv/ChangeLog.

Envir:

 - EnvirBase: changed the lifecycle of several plugin objects to per-run,
   i.e. those objects are now deleted and re-created between runs. Affected
   plugins: event log recorder, output vector file recorder, output scalar
   file recorder, snapshot recorder, future event set (FES).

   The corresponding configuration options have also become per-run options:
   eventlogmanager-class=, outputvectormanager-class=, outputscalarmanager-class=,
   snapshotmanager-class=, futureeventset-class=.

   Side effect: in output vector files, vector IDs now start again from 0
   at the start of each run when running multiple runs. Before, they continued
   from where the previous run left off.

Scavetool:

 - Added --list-itervars option

 - Removed obsolete export commands 'vector' and 'scalar'

 - Do not export itervars as scalars by default; added -y,
   --add-itervars-as-scalars option to turn exporting itervars back on

 - Sort runs by runId, for consistent query output

 - JSON and CSV exporter changes (applies to IDE Analysis Tool as well):
   - Python export changed to JSON export with optional Python flavour
   - CSV export renamed to "CSV for spreadsheet" and significantly improved
     (also saves iteration variables for better run identification, etc.)
   - another CSV export added ("CSV Records"), provenly suitable as input
     for the read_csv() of Python Pandas

Result recording and processing:

 - Switch to new scalar/vector file recorders: The default values for
   the outputscalarmanager-class= and outputvectormanager-class= config
   options are now the new classes OmnetppOutputScalarManager and
   OmnetppOutputVectorManager. These classes rely on the new result
   file writer classes in common/.

   The old classes cIndexedFileOutVectorManager and cFileOutScalarManager
   still exist. They will be removed in a later version of OMNeT++.

 - Save iteration variables separately from run attributes ("itervar"
   lines). NOTE: This changes result file format!

 - omnetpp recorders: ensure order of "param" lines in the file mirror the
   order in the ini file

 - Output scalar file recorders: sum and sqrSum are no longer saved for
   weighted statistics. This is to follow recent cStddev/cWeightedStddev
   change.

 - Output scalar files: do not save numeric iteration variables (of
   parameter studies) as scalars. Instead we'll add them (as scalars)
   at load time, if needed.

 - Proper loading and exporting of weighted statistics

 - Added StatisticsResult (HistogramResult sans histogram). Until now,
   statistic results in scalar files were loaded as several unrelated
   scalars, and were not available as a single object.

 - OmnetppResultFileLoader: fix: 'run' line is mandatory since version 4.0

 - SqliteResultFileLoader: updated: histBin table was renamed to histogramBin

 - SqliteResultFileLoader: loading of vector attrs and run params was missing

 - SQLite recorder: slight changes in file format

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.2


OMNeT++ 5.1.2 (August 2023)
---------------------------

This is a modernized maintenance release of omnetpp-5.1.1.

Changes:

 - Source code updated to eliminate warnings issued by modern C++ compilers.
   Backported several smaller fixes.

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.


OMNeT++ 5.1.1 (May 2017)
------------------------

This release contains only minor bugfixes.

 - The IDE on Windows now can be started directly from a shortcut. Previously
   the IDE was not able to load its native library if it was started from a
   shortcut instead of the MinGW shell.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.1.1


OMNeT++ 5.1 (March 2017)
------------------------

This is the final version of OMNeT++ 5.1. This release significantly improves
and builds upon functionality introduced in version 5.0, and also raises the
bar in other areas.

Highlights:

 - A matured Qtenv that now replaces Tkenv as the default runtime GUI.
 - Support for smooth custom animation and video recording.
 - Better support for simulation campaigns in managing and performing a large
   number of runs and in result recording/processing.
 - Experimental support for SQLite-based result files.
 - Updated toolchain and libraries on Windows including 64-bit Windows support.
 - Updated Eclipse base for the IDE (Eclipse v4.6.3).

For further details see the preview version announcements below.

Changes since OMNeT++ 5.1 Preview 3:

Result Analysis:

 - Data export functionality in scavetool and the IDE has been reimplemented.
   Currently the following export formats are available: CSV, Python source,
   OMNeT++ scalar/vector files, SQLite scalar/vector files. The new exporter
   interface allows for defining new exporters (including UI) without any
   change to scavetool or the Analysis Tool in the IDE, and for more flexible
   output formats.

   In scavetool, the new 'export' command replaces the now-deprecated 'vector'
   and 'scalar' commands.

Qtenv:

 - Added graphical inspectors for output vectors and histograms. They are
   enabled for cOutVector and cStatistic-based histogram objects, i.e. they
   are currently not available for @statistic-based output vectors and
   histograms.

Build:

 - The configure script no longer detects and tests for the presence of the
   'pcap' library. If models need it, they have to implement their own method
   to detect and configure it.

Samples:

 - The routing example was modified to demonstrate the new, built-in packet
   animation in Qtenv. Packets are displayed as arrows along the connection
   showing the actual packet length on the wire. Added parameter to be able
   to switch between cut-through switching and store-and-forward. Also added
   a parameter study into its omnetpp.ini.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+milestone%3A5.1


OMNeT++ 5.1 Preview 3 (February 2017)
-------------------------------------

This version is expected to be last preview version before releasing
OMNeT++ 5.1 final.

Core:

 - Ordering of 'platdep/sockets.h' and 'omnetpp.h' is no longer important.
   It is recommended to include 'omnetpp.h' first.

 - Changes in the Canvas API: the insertBelow()/insertAbove() methods
   have been added to cFigure; cFigure's parse(cProperty*) and
   getAllowedPropertyKeys() methods are now public API; the first argument
   of the recently introduced Register_Figure() macro must now be quoted.

 - Added opp_get_monotonic_clock_usecs(). This function should be used for
   measuring wall-clock time intervals in schedulers and other places instead
   of gettimeofday() which is not monitonic.

 - opp_run (and all executable simulations) now support the '-h configvars'
   to print the list of dollar variables that can be used in configuration values.

 - Other minor improvements.

Documentation:

 - Install Guide has been updated and covers the installation procedure on
   the most recent operating system versions.

 - The Simulation Manual has been heavily updated. Look for the (new) symbol
   to see the updated content.

 - User Guide now has a new chapter about the Qtenv runtime environment.

Build:

 - omnetpp.h is now treated as a 'system header', so it will no longer generate
   warnings in case a model specifies more stringent checking for the compiler.

 - The `configure` script now accepts WITH_XXX=yes/no options on the command line.
   Look into the configure.user file to see the supported variables.

 - Cross compilation of OMNeT++ for Windows on a Linux host is now supported.

IDE:

 - Fixed an issue when generating NED documentation on Windows

 - IDE can now generate a makefile that is exactly the same as the one
   generated by the opp_makemake tool.

Qtenv:

 - Numerous bug fixes and enhancements.

Tools:

 - scavetool: the -f option for the 'query' command adds statistics fields (min,
   max, sum, etc.) as scalars.

 - msgc: Message compiler now correctly generates 'override' keywords for certain
   functions.

 - The message compiler now generates code which produces no warnings even
   when -Wall -Wextra -Wpedantic is used as a compiler command line option.

Samples:

 - osg-earth now has a configuration where off-line map tiles are used instead
   of an on-line map tile provider. (Note that this may not work on current
   Debian/Ubuntu distros, because they contain an osgEarth library version
   that is too old.)

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+milestone%3A5.1pre3


OMNeT++ 5.1 Preview 2 (December 2016)
-------------------------------------

This release significantly improves and builds upon functionality introduced
in version 5.0, and also raises the bar in other areas. Highlights include:
a much-improved Qtenv that now replaces Tkenv as the default runtime GUI;
support for smooth custom animation and video recording; better support for
simulation campaigns in managing and performing a large number of runs and in
result recording/processing; experimental support for SQLite-based result
files; updated toolchain and libraries on Windows; 64-bit Windows support;
updated Eclipse base for the IDE.

Note:

1. There are now separate downloads for Linux, Windows and macOS. A fourth
   package labelled "core" comes without the IDE, and can be used on all other
   systems.
2. 32-bit Windows and Linux are no longer supported.

Known Issues:

1. The manuals have not yet been updated.
2. Documentation generation on Windows is currently not working.

Changes since version 5.0:

Core:

 - Support for smooth custom animations. The goal was to allow models to visualize
   their operation using sophisticated animations while the simulation is running
   under a GUI (Qtenv). The key idea is that refreshDisplay() is called repeatedly
   at a reasonable rate to render frames. refreshDisplay() knows the animation
   position from the simulation time (which is now interpolated between events)
   and the "animation time", a variable also made accessible to the model.
   Animation-to-simulation speed ratio (sec/simsec) is determined by an "animation speed"
   variable (or rather, several such variables) that can be set explicitly from code.
   Different animation speeds can be chosen dynamically to adapt the animation
   to the current time scale of the interesting events or processes in the simulation.
   Animations that need to take place in zero simulation time are supported
   with "holds", i.e. holding up the simulation for a certain animation time period.
   The actual playing speed of the resulting animation can be controlled in Qtenv
   using the "Speed" slider on the toolbar. Note that this feature is only available
   in Qtenv (and not in Tkenv which is more-or-less in maintenance mode from now on).

 - If a model implements such full-blown animations for a compound module that
   OMNeT++'s default animations (message sending/method call animations) become
   a liability, they can now be programmatically turned off for that module with
   a setBuiltinAnimationsAllowed(false) call.

 - Support for self-refreshing figures. cFigure now has a refreshDisplay()
   method which is called on every display refresh as long as the containing
   canvas is open in the GUI. Overriding that method in custom figure classes
   allows the figure to update itself according to the state of the simulation.
   The self-refreshing feature is especially useful for figures that implement
   various meters, gauges, plots or charts, because this way they don't require
   an additional helper module to update them. Note this feature is only available
   in Qtenv.

 - Custom figure classes are now registered using Register_Figure(), not with
   Register_Class(). This allows for more flexibility and a cleaner handling of
   C++ namespaces.

 - The stacking order of figures is now jointly determined by the child order and
   a new runtime-settable cFigure member called zIndex, with the latter taking
   priority. In @figure properties in NED files, "zIndex" replaces "childZ" which
   was only used at parse time but not stored in figures afterwards.

 - A figure can now have a tooltip text set.

 - A figure can now be associated with a simulation object, for example a module
   or a packet. Association means that the figure more-or-less "stands for" (or
   visually represents) the other object in the GUI. For example, when the user
   clicks or double-clicks the figure under Qtenv, the associated object is
   focused or opened in the GUI. The figure also "inherits" the tooltip of its
   associated object, provided it does not have its own tooltip.

 - For text figures, one can now optionally turn on a "halo", which makes the
   text more readable on all backgrounds.

 - Support has been added to load custom images for use by cImageFigure/cIconFigure
   and in display strings (cEnvir::loadImage()). It is also possible now to
   programmatically add new directories to the image path (cEnvir::appendToImagePath()).

 - The dimensions of images used by cImageFigure/cIconFigure can now be determined
   programmatically (getImageNaturalWidth() / getImageNaturalHeight() methods).

 - Support for measuring the text has been added to cTextFigure and cLabelFigure
   (getBounds() method). Note that getBounds() assumes zoom level 1.0.

 - Support for accessing the coordinates of auto-layouted submodules (cEnvir::
   getSubmoduleBounds()). This functionality is needed by some visualizers in
   INET, where nodes of a wired network are typically not explicitly placed.

 - In cObject and subclasses, info() has been renamed to str(). The old method
   still exists and delegates to the new one. The detailedInfo() method has been
   deprecated due to lack of usefulness.

 - In NED statistic declarations (@statistic), signal names in the "source"
   attribute can now be qualified with the name of a submodule. This will cause
   the signal listener to be added to the given submodule instead of the module
   containing the @statistic. Example: `@statistic[foo](source=a.b.foo)`. Note that
   there is no syntax to specify modules above or outside the one containing the
   @statistic, as that would limit the module's reusability (encapsulation
   violation).

 - In cModule, the arrived() method has been made public API. arrived() is
   invoked as part of the send() protocol, and its default version inserts the
   message into the FES after some bookkeeping. Overriding arrived() allows one
   to perform custom processing at the destination module of a send() call
   immediately, still within the send() call.

 - cPacket's default getDisplayString() method now falls back to returning the
   encapsulated packet's display string, instead of just returning an empty string.

 - When formatting error messages, location/time information is now placed *after*
   the exception text, not before. The goal was to improve readability, as user
   now needs to go through less blabla before getting to the actual error message.
   Individual error messages have also been revised for brevity and consistency
   (capitalization, use of quotation marks, etc.)

 - Several things have been made inspectable at runtime (e.g. from Qtenv):
   simulation results being collected (i.e. result filter/recorder objects
   added via @statistic); per-signal listeners lists; values of XML module
   parameters and cXMLElement trees.

 - Unit conversion now knows about C (coulomb) and related units As, mAs, Ah, mAh.

 - Many other smaller-scale changes, fixes and refactoring, in part related to
   the ones above; see ChangeLogs under src/ for details.

Qtenv:

 - Qtenv has reached maturity and it is now the default GUI for simulations.

 - Qtenv now requires Qt5.4, and optionally OpenSceneGraph 3.2-3.5 and
   osgEarth 2.5-2.7.

 - You can use your own Qt bundle by setting the QT_PATH variable in
   configure.user before running ./configure. (This may be needed if your Linux
   distribution comes with an older version.)

 - Added support for smooth custom animations. This means that simulation time is
   interpolated between events, and the animation can be stopped between events.
   In the event number display, the event number is prefixed with either 'last:'
   or 'next:' to make it unambiguous. A new 'Animation Parameters' window has
   been added where one can view the current animation speed, framerate, and
   other metrics.

 - One-stepping now stops right before events so pressing F4 will execute the
   next event instantly. For consistency, after initialization, the simulation
   time will be set to that of the first event (not always to 0s).

 - The simulation time display now has digit grouping and units turned on by
   default for better readability. Settings can be changed in the context menu.

 - Added built-in support to record animation into a high quality video.
   (Press the record button on the toolbar for instructions.)

 - Improvements on built-in animations: Messages sent with a nonzero propagation
   delay are now animated properly (not instantaneously). Packets of nonzero
   length are now displayed as "strips" when transmitted on a link with a
   transmission rate and propagation delay. The animation of method call
   hierarchies now represents the call graph better.

 - Two-way connections are now drawn as two half-length lines so that they
   don't cover one another.

 - Module layouts are now shared among all graphical inspectors of the same
   module. Layout seeds are now persistent between runs.

 - The Configuration/Run selection dialog now accepts run filters from the
   command line and will display the matching runs on the top of the list.

 - Inspector windows made persistent between runs and will be re-opened on
   simulation restart.

 - Other improvements, including tweaks to the Preferences dialog, context
   menu usability in inspectors, updated application and toolbar icons, a new
   'Rebuild network' button on the toolbar, and many bug fixes.

 - Experimental support to run as a native Wayland client (start the simulation
   with the QT_QPA_PLATFORM=wayland environment variable set).

Tkenv:

 - While Qtenv is now the default runtime GUI, simulations can still be
   launched under Tkenv by adding "-u Tkenv" to the command line. Tkenv is
   being maintained, but it is not actively developed any more. This means
   that most new features, including the ones added in this OMNeT++ release
   (smooth custom animations, self-refreshing figures, etc) will not be
   available under Tkenv.

Envir:

 - Run filtering: The -r option of simulations now also accepts a run filter
   expression as an alternative to a list of run numbers and run number ranges.
   This makes it possible to use the values of iteration variables for filtering,
   instead of the artificial and more-or-less meaningless run number. The new
   -q option (see below) can be used to query the list of matching runs.
   Example: ./aloha -c PureAlohaExperiment -r '$numHosts>5 and $numHosts<10' -q runs

 - To query the list of matching runs, the new -q <what> option can be used
   together with -c <config> and -r <runfilter>. The argument to -q can be
   any of: "numruns", "runnumbers", "runs", "rundetails", "runconfig",
   "sectioninheritance".

 - The nesting order of iterations has been made configurable, and the default
   has changed: the repeat counter has been switched from being the innermost
   loop to being the outermost one. This is more practical, as it allows one
   to get early results for all data points, then refine the picture as more
   runs are being completed. The nesting order (also among iteration variables)
   can be specified using the new iteration-nesting-order configuration
   option.

 - Bugfix: the constraint option did not take effect when specified on the
   command line (--constraint=...)

 - The -s (silent, i.e. non-verbose) option has been added, partly to facilitate
   machine processing of -q output.

 - By default, error messages are written to stderr. A -m (merge output) option
   has been added that redirects errors to stdout; a practical benefit is that
   it preserves the relative order of the output.

 - Default result file naming scheme been changed to be more practical. The
   traditional naming scheme contained the run number, which has now been
   replaced by the values of the iteration variables and the repetition
   counter. Example: old: Aloha-16.sca, new: Aloha-numHosts=10,mean=0.9-#3.sca.
   Illegal and inconvenient characters are encoded in an urlencode-like manner.
   This naming scheme applies to cmdenv output files, eventlog files and
   snapshot files as well.

 - Experimental support for SQLite as result file format. SQLite result files
   can be browsed using existing GUI tools (SQLite Browser, SQLite Studio),
   can be queried using SQL, and can be accessed and manipulated from all
   major programming languages including Python and R. These benefits are
   in exchange for slight performance penalty. SQLite result files contain
   the same information as OMNeT++ native result files, they can co-exist,
   and OMNeT++ tools and the IDE understand both. To switch to SQLite as
   default result file format, compile OMNeT++ with PREFER_SQLITE_RESULT_FILES=yes
   set in configure.user. To use SQLite only for specific simulations,
   add the following lines to their omnetpp.ini files:

   outputvectormanager-class="omnetpp::envir::SqliteOutputVectorManager"
   outputscalarmanager-class="omnetpp::envir::SqliteOutputScalarManager"

 - In output scalar files, when run attributes (iteration variables, etc.)
   are saved as scalars, the module name they are saved with has been changed
   from `.` to `_runattrs_`.

 - The cpu-time-limit option has been fixed to work as expected; a new
   real-time-limit option also has been added.

Cmdenv:

 - All changes described in the Envir section above apply, plus:

 - When performing multiple runs, Cmdenv now stops after the first run that
   stops with an error. This behavior can be controlled with the new
   cmdenv-stop-batch-on-error=<bool> option.

 - When performing multiple runs, Cmdenv now prints run statistics at the end.
   Example output: "Run statistics: total 42, successful 30, errors 1, skipped 11"

 - The cmdenv-output-file option can now be specified per run, and now has a
   default file name that follows the naming scheme of result files (but with
   the .out extension). Since saving the output can no longer be
   disabled by omitting the cmdenv-output-file setting, new option
   cmdenv-redirect-output=<bool> has been added for that purpose.

 - The cmdenv-interactive option can now be specified on per-run basis.

Tools:

 - scavetool: The command-line interface (options and help) has been redesigned
   for usability.

 - scavetool: The tool now supports querying the contents of result files
   in a user-friendly way, via the new "query" subcommand. This subcommand
   has also been made the default operation mode. For example, a simple
   "scavetool *.vec *.sca" command reports the number of runs, vectors and
   scalars found in the specified files.

 - scavetool: CSV and other tabular export has been improved: run attributes
   (iteration variables, etc) are now added to the output as columns.
   Note that scavetool currently cannot export to SQLite.

 - opp_makemake: OMNeT++ and generated model makefiles now use compiler-
   generated dependencies (gcc/clang -MMD option) that are saved in the
   out/ directory in *.d files. "make depend" is no longer needed.

 - opp_makemake: Support for deep includes (automatically adding each
   subfolder to the include path) has been dropped, due to being error-prone
   and having limited usefulness. In projects that used this feature,
   #include directives need to be updated to include the directory as well.

 - opp_makemake: Removed support for generating nmake-compatible makefiles,
   as we now use GNU Make on all platforms.

 - opp_featuretool: Symbols for enabled features (e.g. WITH_IPv4 for the IPv4
   feature in INET) are now placed into a generated header file, instead of being
   passed to the compiler via -D options. The name of header file can be
   specified in the feature definition file (.oppfeatures).

 - opp_runall has been reimplemented in Python, and its command-line interface
   has been redesigned to not only allow using multiple CPUs but also several
   runs per Cmdenv instance. (This change allows one to execute a large number
   of short simulation efficiently by reducing process startup overhead.)
   Internally, opp_runall now uses the new "-q" option of simulations to
   expand a run filter expression to a list of runs.

 - opp_test: export "run" script and "retest" scripts under work/ for each test,
   to facilitate running tests manually. Minor bug fixes.

 - Removed unused utilities like opp_makedep, lcg32_seedtool, abspath, and
   some others.

IDE:

 - The IDE is now based on Eclipse 4.6 Neon and CDT 9.0 (Java 1.8 required).

IDE/C++ Build:

 - The "Collecting includes..." phase of project build (that could take quite
   a significant amount of time, and was often seen as a pain point) has been
   eliminated. The key was to switch from IDE-generated dependencies to
   compiler-generated dependencies (see corresponding opp_makemake change)
   that made include analysis in the IDE redundant.

 - Change in "Project Features" feature: Preprocessor symbols for enabled
   features (e.g. the WITH_IPv4 macro for the IPv4 feature) are now placed
   into a generated header file, and not added to the build configuration
   (Paths & Symbols page) as macros to be passed to the compiler via -D
   options. The name of header file is part of the feature definition file
   (.oppfeatures). Using a generated header file reduces the number of things
   that can go wrong during indexing and project build, and also has the
   advantage of being seen from derived projects.

 - The "Export build tester makefile" button has been removed from the
   Project Features property page. The INET Framework project now has an
   opp_featuretool-based shell script for the same purpose (tests/featuretest)
   that can be easily adapted to other projects as well.

 - Support for deep includes (automatically adding each subfolder to the
   include path) has been dropped from the IDE as well, due to being error-
   prone and having limited usefulness.

 - Improvements in the Makemake Options dialog that opens from the Makemake
   project property page. For example, an "Include directories" listbox has
   been added to the Compile tab page, and less frequently used options
   on the same page have been moved under the More>>> link.

 - CDT integration: renamed the default C/C++ project configuration names
   from "gcc-debug"/"gcc-release" to simply "debug" and "release".

IDE/Simulation Launching:

 - In the Run/Debug Configurations dialog, the "OMNeT++ Simulation" form page
   has been revised for usability and to better support simulation campaigns.
   The launcher (code that schedules and actually runs the simulations and
   arranges feedback in the Progress view and the Console) has also been
   improved. Details follow.

 - The "Runs" form field now accepts a run filter expression that can refer
   to iteration variables. (This form field corresponds to the -r option of
   simulations.)

 - Batch execution of simulations is now controlled with two new spinner
   widgets: "Number of CPUs to use", "Runs per process".

 - The form page now allows specifying time limits for the simulation
   ("Simulation time limit", "CPU time limit" fields)

 - Radio buttons have been replaced by tri-state checkboxes (with on/off/grayed
   states, where the grayed state means "no setting specified, let the inifile
   setting take effect".) The consequent space saving allowed other options
   to be added to the form page: "Verbose", "Stop batch on error", "Express
   mode", "Save stdout", "Record scalar results", "Record vector results",
   "Record eventlog".

 - User interface selection now also uses an editable combo instead of
   radio buttons.

 - Added content assist for Additional Arguments field.

 - Improvements in the launcher: Simulation batches are now easier to cancel;
   in case of a simulation error, the error dialog now correctly displays
   the error message, not just a "Finished with error" text.

IDE/Inifile Editor:

 - Updated to know about option changes and new options.

IDE/Analysis Tool:

 - In the Browse Data page, display Experiment/Measurement/Replication columns
   instead of Folder/Filename/RunId/Config/RunNumber. Note that this is just
   the default value for a preference, so the change will only take effect
   in new installations or new workspaces. The default columns widths have
   also been increased.

 - Initial support for SQLite result files. From the end user perspective,
   they should work exactly as OMNeT++ result files.

 - CSV and other tabular export has been improved in the same way as in
   scavetool (as they use the same export engine).

Build:

 - The Windows version now targets 64-bit Windows, using MinGW-w64. Both the
   GCC and Clang compilers are included, as well as all necessary libraries
   (Qt5, OSG, etc.) Support for 32-bit Windows has been dropped.

 - Compiling OMNeT++ now requires a C++11 compliant compiler.

Samples:

 - aloha: The updated example highlights the use of the new smooth animation
   API. The server and all hosts now have fixed positions (still random but
   deterministic, and not using auto-layouting), so that their individual
   propagation delays can be computed. Each transmitted packet is visualized
   with a ring and many concentric circles, illustrating the propagation of the
   electromagnetic wave. The visualization is faithful, e.g. multiple signals
   visually overlapping at a receiver actually means a collision. Animation
   speed is controlled by each node, taking three parameters for different
   states (idle, transmission edge, midtransmission) into account.

 - osg-earth: Made the mobile nodes in the 'Boston streets' configuration more
   easily noticeable by increasing their sizes, and adding a color parameter.
   Movement trails have been raised off the ground a bit to avoid Z-fighting
   glitches.

 - osg-satellites: Refactored mobility logic and replaced the discontinued
   MapQuest public tile source with a single offline image to avoid dependency
   on external sources.

 - queueinglib: Fixed a race condition by requiring the PassiveQueue to allocate
   the Server before sending a job to it.

 - resultfiles: Result files were renamed to reflect the new default naming
   scheme (i.e. configname-itervars-#repetition).

 - Note: The systemc-embedding, sockets and cqn/parsim examples are NOT working
   currently in the Qtenv runtime environment. If you want to run those examples
   please run them in either Tkenv or Cmdenv ('-u Tkenv' or '-u Cmdenv').

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.1pre2


OMNeT++ 5.1 Preview 1 (September 2016)
--------------------------------------

This Preview release was only circulated at the OMNeT++ Summit in Brno,
Czech Republic on September 15-16, to get immediate feedback from the
participants.


OMNeT++ 5.0.1 (August 2023)
---------------------------

This is a modernized maintenance release of omnetpp-5.0.

Changes:

 - Source code updated to eliminate warnings issued by modern C++ compilers.
   Backported several smaller fixes.

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.
    4. `configure` now accepts WITH_FOO=no macros as command line options

 - Allow running tests in other than "debug" mode.


OMNeT++ 5.0 (April 2016)
------------------------

Release 5.0 is a result of development effort of nearly two years. This is a
major release that introduces significant new features compared to the last
4.x version, for example the Canvas API (2D graphics), OpenSceneGraph-based 3D
graphics support, improved logging, a new Qt-based runtime environment that
will eventually replace Tkenv, and much more.

We have also taken the opportunity of the major release to improve several
corners of the OMNeT++ API, and also to get rid of deprecated functionality.
For porting models from OMNeT++ 4.x, see doc/API-changes.txt which lists all
changes, with hints on how to update the model code.

Changes since the release candidate:

Documentation:

 - Several organizational and rendering improvements on the API documentation
   generated using Doxygen.

 - The User Manual has been renamed to Simulation Manual to reduce the chance of
   confusing it with the User Guide (which describes the IDE and Tkenv/Qtenv).

 - Improved the contents and the HTML rendering of the Simulation Manual.

Core:

 - cFigure: skew() methods now take a coefficient instead of an angle.

 - cOsgCanvas: initialize zNear/zFar to NaN (to indicate they are unset by
   default), and added the methods setZLimits(), clearZLimits(), hasZLimits().
   Missing zNear/zFar will turn on automatic zNear/zFar computation in the viewer.

 - Removed a Register_Enum()-related workaround introduced around OMNeT++ 5.0b2
   so that it could compile INET-3.0.

 - cFingerprint renamed to cFingerprintCalculator.

 - cKSplit: method rangeExtension(bool) has been renamed to setRangeExtension().

 - cModule::size() has been deprecated (use getVectorSize() instead).

 - Several methods in cComponent, cModule, cSimpleModule, cChannel, cArray,
   cQueue, cTopology, cStatistic and other classes have been made virtual to
   allow the user override them.

 - Fix: simtime_t was not part of the omnetpp namespace.

 - Fixed #943 which prevented using cDatarateChannel's forceTransmissionFinishTime()
   method when the channel's busy state signal was recorded into an output vector.

Tkenv:

 - Fixed tooltip-related problems: large tooltips could not appear (e.g. the
   help for log prefix in the Preferences dialog); submodule tooltips were
   missing from the canvas

Qtenv:

 - Added help to the Log Prefix field in the Preferences dialog and filter
   fields in the Find Objects dialog

 - Added tooltips to timeline messages

 - Fix: on Windows, Earth scenes appeared distorted in the osg-earth and
   osg-satellites sample simulations

 - Fix: Earth scenes did not display in the osg-intro sample simulation (blank
   viewer window)

Utils:

 - opp_featuretool refinements

Additionally, several bug were fixed in the IDE, Tkenv and Qtenv.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.0


OMNeT++ 5.0 rc1 (March 2016)
----------------------------

This release brings numerous refinements to APIs introduced in previous beta
versions, several bug fixes, and relatively few new features. The manual has
been improved and brought up to date for the most part, although there are
still gaps that will be filled in for the final 5.0 release. The most
conspicuous change will be the new simulation time display in Tkenv and Qtenv.

Build:

 - Introduced WITH_TKENV, WITH_QTENV, WITH_OSG and WITH_SYSTEMC in configure.user;
   these options allow one to disable certain features if they are not needed.
   In particular, WITH_TKENV=no replaces NO_TCL=1

Core:

 - The logging API was refined. In addition to several renames (e.g.
   GLOBAL_COMPILETIME_LOGLEVEL was changed to COMPILETIME_LOGLEVEL), the
   numeric order of log level constants (LOGLEVEL_*) was reversed to make
   the values consistent with semantics, and LOGLEVEL_OFF was added to allow
   one completely disable logging. Support for log filtering was added in the
   form of compile-time and runtime predicates. The default compile-time
   log levels were also changed (to TRACE in debug builds, and DETAIL in
   release builds).

 - In the OpenSceneGraph API, OmnetppObjectNode was renamed to cObjectOsgNode.
   Static cOsgCanvas methods that existed for creating and manipulating such
   nodes (createOmnetppObjectNode(), isOmnetppObjectNode(), setOmnetppObject(),
   getOmnetppObject()) were removed, and one should directly use cObjectOsgNode
   instead. Also, the osgutil.h header is now part of omnetpp.h, so it doesn't
   need to be included separately.

 - In cOsgCanvas, the redundant setPerspective() method was removed (it was a
   union of two other setters).

 - There were several refinements in the cFigure API:
   - replaced getClassNameForRenderer() with getRendererClassName()
   - spelling: ArrowHead changed to Arrowhead in method names, enum name, etc.
   - move() made recursive, nonrecursive version is called moveLocal()
   - Pixmap: renamed resize() to setSize()
   - Transform: skew() methods to use coefficient instead of angle
   - Transform: the matrix was transposed so points can be column vectors
     instead of row vectors
   - added NUM_GOOD_DARK_COLORS and NUM_GOOD_LIGHT_COLORS
   - some cPixmapFigure methods were also renamed

 - Refined @figure syntax, the way figures can be defined in NED files:
   - transform= now accepts "((a b) (c d) (t1 t2))" and matrix(a,b,c,d,t1,t2)
     syntaxes as well
   - in transform=, scale() now accepts center as well
   - in transform=, skew() now expects coefficient instead of angle
   - arc, rectangle, image, etc to support bounds=x,t,width,height
     as an alternative to the pos=,size=,anchor= triplet

 - Revised the configuration options that control recording of @statistics,
   individual scalars, and histograms. As a result, the **.statistic-recording
   and **.bin-recording options were introduced that take over some of the
   responsibility of the formally overloaded **.scalar-recording option.
   Rationale and details in src/envir/ChangeLog.

 - Added cIEventlogManager, a new Envir plugin class that allows one
   to replace the built-in eventlog manager. Eventlog managers are
   responsible for recording simulation history into an eventlog file.
   A new eventlog manager can be activated with the eventlogmanager-class
   configuration option.

 - The cFingerprint class was renamed to cFingerprintCalculator, and the
   fingerprint-class config option to fingerpringcalculator-class.

 - In cEnvir, renamed getRNGMappingFor(component) to preconfigure(component).

 - In cSimulation, moved both updating the event number and simulation time
   into executeEvent(). They were updated inconsistently, plus executeEvent()
   is a better place for that than getNextEvent(), considering cScheduler's
   putBackEvent().

 - Removed cEnvir::isDisabled() and added cEnvir::isLoggingEnabled(). Renamed
   cEnvir::disableTracing member variable to cEnvir::loggingEnabled.

 - cModule: size() deprecated, use getVectorSize() instead

 - More methods made virtual in cSimpleModule and other classes

 - Improved the descriptions of several per-object config options.

Tkenv:

 - Added a large simulation time and event number display to the toolbar that
   display the *current* simulation time (or the time of the last event), as
   opposed to the time of the next event displayed previously. The status bar
   below the toolbar displays information about the *next* event, together with
   the time delta from the current simulation time. Information on the currently
   set up simulation, along with message statistics, have been moved to the
   bottom status bar of the main window.

Qtenv:

 - Added a large simulation time and event number display to the toolbar. (This
   is the same GUI change as in Tkenv, see above.)

 - Changed the name of the preferences file to .qtenvrc.

 - Numerous bug fixes and improvements.

Cmdenv:

 - Configuration options controlling logging in non-express mode
   (cmdenv-express-mode=false) have been simplified. The following options
   have been removed: cmdenv-module-messages, cmdenv-global-log-level,
   **.cmdenv-ev-output. The equivalent functionality (enabling/disabling
   logging and setting the log level globally and per-module) is now
   controlled with **.cmdenv-log-level. Logging can be disabled with
   **.cmdenv-log-level = OFF. Note that the cmdenv-event-banners and
   cmdenv-log-prefix options still exist.

 - The cmdenv-message-trace configuration option has been removed. It had very
   limited usefulness, and it was somewhat overlapping with eventlog.

 - The cmdenv-log-format option has been renamed to cmdenv-log-prefix.

Examples:

 - Updated visualization in samples to use the new refreshDisplay() callback.

 - Removed google-earth demo. (We already have a similar demo, osg-earth.)

Utils:

 - opp_featuretool revised and improved

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.0rc1


OMNeT++ 5.0 beta 3 (December 2015)
----------------------------------

The highlights of this release are the experimental version of Qtenv (which is
a Qt-based runtime environment that will eventually replace Tkenv), support
for 3D graphics using OpenSceneGraph, and the addition of the much-requested
opp_featuretool utility. The full list follows:

Core:

 - Use of the omnetpp namespace was made permanent. The recommended way of
   porting models is to add the "using namespace omnetpp;" like to header
   files. To compile models without change, add -DAUTOIMPORT_OMNETPP_NAMESPACE
   to the compiler command line.

 - Added 3D graphics support via OpenSceneGraph. The 3D scene graph is to
   be built using the OpenSceneGraph API (e.g. loaded from file via osgDB::
   readNodeFile()), and then set onto a cOsgCanvas instance. 3D scene(s)
   can be visualized in the new Qtenv runtime GUI. (There is no 3D support
   in Tkenv, due to technical limitations.)

 - Added resolveResourcePath() to cEnvir and cComponent. The method searches
   a number of folders for a resource given with its file name or relative
   path, and returns the path for the first match. It can useful for locating
   various data files for models, OSG model files, and other files.

 - Visualization: added a method to the base class of modules and channels to
   serve as a container of visualization-related code. refreshDisplay() is
   invoked by graphical user interfaces (Qtenv, Tkenv) whenever GUI contents
   need to be refreshed. Display string updates, canvas figures maintenance,
   and OSG scene graph updates are probably best done inside refreshDisplay()
   methods, as it can result in significant performance gain and, in some
   cases, also in more consistent information being displayed.

 - An isExpressMode() method has been added to cEnvir to query whether the
   simulation runs in Express mode under a GUI. This information can be useful
   additional input for refreshDisplay() code.

 - The simtime-scale config option has been replaced by simtime-resolution,
   a more user-friendly incarnation. simtime-precision accepts time units
   (s, ms, us, ns, ps, fs, as), power-of-ten multiples of such a unit (e.g.
   100ms), and also base-10 scale exponents in the -18..0 range (mostly for
   backward compatibility with simtime-scale).

 - RNG-to-module mapping made more flexible: the **.rng-<N> config option now
   allows expressions, including those containing index, parentIndex, and
   ancestorIndex(level). This change allows things like assigning a separate
   RNG to each element of a module vector.

 - Fingerprint computation has changed. It was also made more flexible,
   e.g. it is now possible to control which ingredients are added into the
   fingerprint computation. Define USE_OMNETPP4x_FINGERPRINT at compile-time
   to get back OMNeT++ 4.x fingerprints.

 - Signal listener interface change: A cObject *details argument has been
   added to emit() methods to allow simulation models provide extra information
   with primitive data types (double, long, etc) they emit, without the need
   to switch over to emitting cObject altogether. This is a non-backward-
   compatible change: signal listeners will need to be updated with the extra
   argument. To facilitate transition, you can compile OMNeT++ and models with
   WITH_OMNETPP4x_LISTENER_SUPPORT defined; this will set up cIListener
   to delegate to the old methods, so existing listeners will work.

 - Result filter/recorder interfaces, being based on signal listeners, have
   undergone a similar change. Models containing custom result filters or
   recorders will need to be updated.

 - SimTime overhaul to increase accuracy (i.e. use integer arithmetic
   where possible), improve the API, and add missing operations/functions.

   Details:
   - Added missing * and / operators for integral types. This causes
     integer multiplications/divisions to be computed with integer arithmetic
     instead of floating point, resulting in better accuracy.
   - Added overflow checking for integer multiplication and negation
   - Use int64 arithmetic and overflow checking when assigning from integer types
   - Added isZero()
   - Replaced "int exponent" with "SimTimeUnit unit" in the two-argument
     constructor (value + unit) and several other methods, with the aim
     of making the API more user-friendly (and model code easier to read).
     Models that call affected methods with integers will need to be
     updated.
   - Moved math functions like fabs(SimTime) into the omnetpp namespace
   - Added div(SimTime, SimTime); also refined related fmod()'s documentation

 - Removed SIMTIME_RAW(), STR_SIMTIME(), and SIMTIME_TTOA(). These macros are
   no longer needed, as were introduced in OMNeT++ 4.0 to assist porting models
   from version 3.x where simulation time was stored in double. Also, MAXTIME
   was renamed to SIMTIME_MAX.

 - The tkenv-image-path config option has been replaced with image-path.

 - NOTE: The above descriptions have been edited for brevity. More information
   is available in the ChangeLog files (e.g. include/omnetpp/ChangeLog and
   src/sim/ChangeLog), and in doc/API-changes.txt.

Qtenv:

 - Qtenv, the Qt-based runtime environment is currently in preview status,
   Tkenv is still the default runtime. Qtenv can be activated by adding the
   '-u Qtenv' switch to simulation command lines. Alternatively, specify
   'PREFER_QTENV=yes' in configure.user to make Qtenv the default GUI.
   Note that 3D visualization is only available in Qtenv.

SystemC:

 - Updated the bundled SystemC reference implementation to version 2.3.1

Examples:

 - Several new examples (osg-intro, osg-earth, osg-indoor, osg-satellites) have
   been added to the 'samples' folder to demonstrate the new 3D visualization
   capabilities in Qtenv.

 - Aloha has been updated to use the new refreshDisplay() method.

Tools:

 - Added 'opp_featuretool', which is basically the command-line equivalent of
   the Project Features dialog in the IDE: it allows one to enable/disable
   project features defined for an OMNeT++ project, e.g. the INET Framework.

Build:

 - Because of the new Qtenv runtime environment, OMNeT++ now has several additional
   dependencies: Qt4 is mandatory, while OpenSceneGraph and osgEarth is optional.
   Qtenv can be fully disabled by commenting out the QT_VERSION= variable in the
   configure.user file and then re-running ./configure. Building Qtenv without
   the OpenSceneGraph libraries will disable 3D visualization support in Qtenv.

 - A separate Mac OS X specific installation bundle was created that contains
   the binary files for all the external dependencies (Tcl/Tk, Qt, OSG, osgEarth etc.)
   As a consequence you no longer have to manually install Quartz before installing
   OMNeT++. We have now separate installation bundles for Linux/Mac/Windows.

 - The bundled toolchain on Windows has been updated to use MinGW-w64 (32-bit)
   containing both the 'gcc' and 'clang' compilers. The bundle contains all the
   dependencies needed to build OMNeT++ (including Tcl/Tk, Qt4, OpenSceneGraph,
   osgEarth etc.)

 - Because of the large size of the extra dependencies (Qt, OpenSceneGraph), the
   tools directory is now distributed as a highly compressed archive file inside
   the Windows installation bundle. It is extracted to its final location when
   mingwenv.cmd is first started.

 - On systems that have both the 'gcc' and 'clang' compilers installed, OMNeT++
   will pick 'clang' by default. This behavior can be changed by the PREFER_CLANG
   variable in the configure.user file.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.0b3


OMNeT++ 5.0 beta 2 (July 2015)
------------------------------

Core:

 - OMNeT++ classes are now in the "omnetpp" namespace. Models need to be
   modified to compile, e.g. by adding "using namespace omnetpp" or
   USING_NAMESPACE lines at appropriate places. (The latter conditionally
   expands to the former when needed, and to the empty string when not,
   making your code compatible with OMNeT++ 4.6 too.) If you don't want to
   change the model source code, you can define the AUTOIMPORT_NAMESPACE
   macro on the compiler command line (add -DAUTOIMPORT_NAMESPACE to
   CFLAGS), which will basically add the using namespace directive at the
   end of <omnetpp.h>.

   Turning off the omnetpp namespace (by setting USE_NAMESPACE=no in
   configure.user) is no longer fully supported, and will be removed
   in the next beta.

 - Introduced cRandom, which encapsulates a random number stream into an
   object. Random numbers can be extracted with the draw() method.
   Added the following cRandom subclasses: cUniform, cExponential,
   cNormal, cTruncNormal, cGamma, cBeta, cErlang, cChiSquare, cStudentT,
   cCauchy, cTriang, cWeibull, cParetoShifted, cIntUniform, cBernoulli,
   cBinomial, cGeometric, cNegBinomial, cPoisson. They encapsulate the
   similarly named functions (e.g. exponential()) with their parameters
   and the random number generator (cRNG). cStatistic also now subclasses
   from cRandom: it can generate a random variate from the distribution
   observed in the collected sample.

 - Random variate generation functions (normal(), etc.) signature change:
   the trailing "int rng" (RNG index of context module) argument was changed
   to cRNG*, and moved to the front. Motivation: break the functions'
   dependence on the context module.

   However, to lessen the impact of the signature change, random variate
   generation functions with their original signatures have been added
   to cComponent as methods. Thus, models that only use those functions from
   module methods will not notice any change.

 - The interface of iterators (GateIterator, SubmoduleIterator and
   ChannelIterator in cModule, cQueue::Iterator, etc.) have been changed
   to make them more consistent with STL iterators. Therefore, operator()
   as a means of dereferencing has been deprecated, and operator* and
   operator-> have been added instead. The increment/decrement operators
   have also been revised.

 - FES made replaceable. To this end, an abstract cFutureEventSet base class
   has been introduced, and cMessageHeap (now renamed cEventHeap) was made
   to extend cFutureEventSet. An accessor method (cFutureEventSet* getFES())
   has been added to cSimulation, and the older members msgQueue and
   getMessageQueue() have been removed. Simulations now accept a

        futureeventset-class=<classname>

   configuration option.

 - Further refinement of the new canvas API (cCanvas and cFigure + subclasses)

 - The "simulation" and "ev" macros have been removed, to reduce pollution
   of the global namespace. You can use the newly introduced getSimulation()
   and getEnvir() functions instead.

 - cObject::parsimPack() has become const. You only need to pay attention
   (and add "const" manually) if you have classes that redefine parsimPack().

 - cQueue and cMessageHeap had both length()/getLength() and empty()/isEmpty()
   methods; the former have been removed; use getLength() and isEmpty() instead.

 - There have been several other, smaller changes and improvements in many
   classes, e.g. cException, cStatistic, cDensityEstBase, cObjectFactory, etc.

 - Most methods, typedefs and other items deprecated in previous versions have
   been removed.

 - See doc/API-changelog.txt for a complete list of changes.

NED:

 - Allow a "module" to extend a "simple" (motivation: INET 3.x)

 - The message compiler no longer appends "_var" to the names of data members
   in generated classes. In cases where this change breaks existing code
   (classes with @customize(true) or with subclasses), you can emulate the
   old behavior by adding the following to the message definition:

        @fieldNameSuffix("_var");

 - In message compiler generated code, doPacking() has been renamed to
   doParsimPacking(), doUnpacking() to doParsimUnpacking().

Tkenv:

 - Feature: double-clicking a module in the object tree will open it
   in the main area instead of opening a new inspector window. A new
   inspector can still be opened from the context menu.

 - Added the "Hide namespaces" option in the Preferences dialog that turns off
   the display of the namespace part of C++ class names when they appear in
   the GUI.

 - Images under images/old/ are no longer accessible without the "old/"
   prefix, so if you use such an icon, you must add the "old/" prefix
   manually for them to continue to work. For example, you have to change
   "i=cloud" to "i=old/cloud" in display strings. The feature to let old/*
   images be accessible without the prefix was originally introduced to ease
   the transition from OMNeT++ 3.x to 4.0.

 - Several bug fixes

General:

 - The include/ folder was restructured: only the public header <omnetpp.h> was
   left in it, all other include files were moved into an omnetpp/ subfolder
   under include/.

 - The source tree (src/) has gone through several modernizing and cleanup steps:

   - Per-folder nested namespaces have been introduced, e.g. Tkenv code is now
     in the omnetpp::tkenv namespace.

   - Qualified includes with the folder, i.e. #include "stringutil.h" became
     #include "common/stringutil.h". This increases readability and reduces
     the chance of including a wrong header.

   - Use fully qualified header guards

   - Modernizing: use the 'nullptr' and 'override' keywords; use C++ names of C
     headers (e.g. <cstdio> instead of <stdio.h>).

   - Code style: renamed many identifiers (local variables, arguments, private
     data members, etc.) to have a consistent, camelcase naming); codebase
     reformatted to have a consistent indentation style.

 - Clarifications in the Academic Public License.

Contrib:

 - Removed JSimpleModule due to lack of users and good use cases

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.0b2


OMNeT++ 5.0 beta 1 (Feb 2015)
-----------------------------

Core:

 - Added the Canvas API, a figure-based 2D drawing API. The Canvas API allows
   augmenting simulations with graphical elements. Item types include various
   shapes, text and image, including an SVG-like "path" item (a generalized
   polygon/polyline, with arcs and Bezier curves). Transformations (scaling,
   rotation, skewing) are supported, as well as transparency.

 - New logging API. It features six log levels (FATAL, ERROR, WARN, INFO,
   DETAIL, DEBUG, TRACE), category support, compile-time and runtime global
   and per-module log level thresholds.

   Also, much more information is passed with each log line to the user
   interface code, allowing one to display a wealth of information in the log
   prefix (log level, file/line, event number, simulation time, module name
   and type, object name and type, and so on).

   Incompatibilities:
     - lower-case ev<< is no longer legal, use EV<< instead
     - ev.printf() was removed, use the stream API (EV<<) instead

 - Introduced cEvent as a base class of cMessage. cEvent allows scheduling
   of arbitrary code for a simulation time that runs independent of modules.
   Override the execute() method of cEvent to specify the code. cEvent is
   not intended for use in simulation models; the primary motivation is to
   allow implementing simulation time limit with an "end-simulation" event,
   and to encapsulate foreign events (e.g. SystemC events and crunching)
   for seamless integration with the simulation event loop.

 - Added support for simulation lifecycle listeners. Listeners are called back
   before and after network setup, on network initialization, before and after
   network finalization, and so on. The motivation was to allow more flexibility
   when writing initialization and shutdown code for schedulers, result
   file managers and other extensions.

 - Signal checking has been enabled by default for debug builds. This means
   that all signals must be declared with @signal in the NED definition of
   the module type that emits them, unless check-signals=false is configured.

 - Channel objects made equal to modules in the sense that now they also have
   IDs (the getId() method was moved from cModule to cComponent, the common base
   class of modules and channels), and they are also registered with cSimulation.

 - cTopology improvements:
   - added factory methods for links and edges
   - added methods to manipulate the graph (e.g. build a graph from scratch)

 - Removed int8..int64 and uint8..uint64. Models should use the standard integer
   types from <stdint.h> that end in "_t": int8_t, uint8_t, etc.

 - check_and_cast<> improved; added check_and_cast_nullable<> that accepts NULL
   pointer as input

 - New fingerprint computation algorithm. The goal was to make the fingerprint
   less sensitive to uninteresting changes (e.g. removal of an inactive module),
   and more sensitive to interesting ones (e.g. change in packet lengths).
   To get the old fingerprints, define USE_OMNETPP4x_FINGERPRINTS when compiling
   OMNeT++.

 - cClassDescriptor interface changes (method renaming and arg list changes)

 - Code cleanup, including:
   - removal of 3.x backward compatibility features (WITH_DOUBLE_SIMTIME,
     WITHOUT_CPACKET)
   - removal of deprecated classes, functions and macros (cLinkedList,
     cSimulation::operator[], Define_Function(), etc.)
   - cTopology: internals refactored (use STL instead of arrays, etc.)
   - error code enum members renamed from eXXX to E_XXX

Cmdenv:

 - Log lines can now be prefixed with information such as the module
   path; simulation time; event's class and object name; file/line
   of the log statement; class, name and pointer of the object
   containing the log statement; etc. The format of this prefix can be
   changed from ini files.

 - New inifile config options: cmdenv-log-format, cmdenv-log-level.

Tkenv:

 - The bgs (background scaling) display string tag has been removed. It was
   originally introduced more or less as a default zoom level, but lost its
   significance since interactive zoom has been introduced.

 - Zooming no longer affects icon sizes on the screen (but you still have
   the Increase/Decrease Icon Size, Ctrl+I/Ctrl+O menu items in Tkenv).

 - Finished implementation of the Canvas API Tkenv rendering. The
   implementation is based on figure renderers (see FigureRenderer class)
   that can be registered for various figure classes.
   The implementation is based on the Tkpath Tcl/Tk extension, which was
   significantly enhanced by our team.

 - Tkpath is now also used internally for drawing submodules, connections
   and the like. Benefits include:
   - unlimited zooming of image backgrounds
   - antialiased drawing (Tk canvas didn't have antialiasing)
   - OS X: images are no longer forced into 1-bit transparency
   - labels are now outlined (have a "halo") to make them readable on noisy backgrounds
   - range indicator fill is now semi-transparent (because Tkpath supports that)
   - performance improvements e.g. on OS X, and with image backgrounds

 - Display log prefix; log prefix format and log level configurable via the Options dialog

IDE:

 - Several minor improvements. NED editor support for canvas items is pending.

Example simulations:

 - Added samples/canvas, a demo for the Canvas API

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A5.0b1


OMNeT++ 4.6.1 (August 2023)
---------------------------

This is a modernized version of omnetpp-4.6 that makes it possible to build
and use the now 9-year-old package in today's software environments. The
primary objective of this release is to facilitate the execution of old
simulation models, e.g. for reproducing simulation results or as a basis of
porting them to more recent versions of OMNeT++.

Changes:

 - Source code updated to eliminate warnings issued by modern C++ compilers.
   Backported several smaller fixes.

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.


OMNeT++ 4.6 (Dec 2014)
----------------------

IMPORTANT:
    To install OMNeT++ on OS X, you must install both the Apple Java
    support package (http://support.apple.com/kb/DL1572) and (Oracle) JDK 1.7
    or later (from http://java.com).

Tkenv:

 - Improved zooming/panning support: use double click to zoom in around
   a point, and Shift + double click to zoom out; use Ctrl + left mouse
   button to drag out a rectangle to zoom to (a.k.a. marquee zoom;
   implementation based on patch from Christoph Sommer), right-click
   cancels marquee zoom; use left mouse button for panning

 - Further small UI fixes (see ChangeLog)

Core:

 - The testing tool opp_test has been revised and is now an official part
   of OMNeT++.

 - The Manual now has a new chapter that covers the testing of simulation
   models as well as the usage of the opp_test tool.

 - The message compiler opp_msgc was reimplemented as a part of nedtool.
   opp_msgc still exists as a wrapper script that points to nedtool.

IDE:

 - The IDE is now based on Eclipse 4.4 Luna, and requires JDK 1.7 or later.

 - The IDE is now a 64-bit application on Mac OS X.

Other:

 - OMNeT++ is now using C++11 by default (-std=c++11) when models are compiled.
   The simulator itself does not use any of the C++11 features, so C++11 support may
   be disabled in configure.user. Update your models to compile properly using the
   C++11 standard, as later OMNeT++ versions will require C++11 compliance.

 - OMNeT++ can be configured to use the 'omnetpp' namespace by setting
   USE_NAMESPACE=yes in configure.user

 - Clang compiler support: Clang is used by default on OS X, and can be turned
   on on Linux. Clang is still not supported on Windows (we are waiting for the
   MSYS Clang packages to mature).

 - The bundled MSYS/MinGW toolchain was updated to use MSYS2. MSYS2 resolves
   a number of issues:
   - No more memory errors during build
   - The make command now properly supports parallel build (-j4, -j8 etc.)
   - The bundled MSYS2 toolchain now contains the pacman package manager,
     so you can install additional packages if you wish
   - Change in OMNeT++: The msys/ directory has been moved to tools/win32/, and
     the mingw/ directory that contains the compiler has been moved to
     tools/win32/mingw32/.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.6


OMNeT++ 4.5.1 (August 2023)
---------------------------

This is a modernized version of omnetpp-4.5 that makes it possible to build
and use the now 9-year-old package in today's software environments. The
primary objective of this release is to facilitate the execution of old
simulation models, e.g. for reproducing simulation results or as a basis of
porting them to more recent versions of OMNeT++.

Changes:

 - Source code updated to eliminate warnings issued by modern C++ compilers.
   Backported several smaller fixes.

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.


OMNeT++ 4.5 (June 2014)
-----------------------

Core:

 - Compound modules are now represented with cModule, and the (almost
   empty) cCompoundModule class has been removed. This change was made
   to allow simple module classes to be used for compound modules.

 - Added cMessagePrinter and the Register_MessagePrinter() macro. By
   implementing a message printer you can customize the line Tkenv prints
   about a packet in the log's "Message/Packet Traffic" view. In a way,
   message printers are analogous to Wireshark dissectors.

Tkenv:

 - The Tkenv GUI has been redesigned for single-window mode to improve
   usability and user experience. Tkenv has also received a new, modern look
   and feel, due to the use of the Ttk widgets and a custom Ttk theme.

   Details:
   - single-window mode, with object navigator + 3 built-in inspectors
     that interact in a meaningful way (new inspector windows can still
     be opened)
   - keep inpector windows on top of the main window at all times
   - use Ttk widgets everywhere, with custom theme - this makes a huge
     difference in looks on all platforms but esp. on OS X
   - BLT no longer needed (we use Ttk instead)
   - inspectors are no longer tied to a single object; visited objects
     are remembered as navigable history (back/forward)
   - module graphics now remembers zoom level and settings per NED type
   - for each log line, the context module is now stored, and is shown
     as a line prefix where it makes sense (differs from event's module)
   - Tkenv now stores message sendings and also a clone of corresponding
     message objects (cMessage), and can show them in the log window
   - message printer classes can be contributed to customize the
     messages view of log windows
   - more concise status area that shows more information at the same time
   - part of the status area can be turned off to free up vertical space
   - timeline drawing now adapts to font size
   - on-demand scrollbars (i.e. they are hidden when not needed)
   - main menu cleanup (reorganized, removed obsolete items)
   - dialogs where it makes sense now remember size and location
   - additional hotkeys: Ctrl+Plus/Minus for Zoom, Ctrl+F5 Run Until,
     Ctrl+Q Quit, etc.
   - OS X: use Command key for hotkeys instead of Ctrl
   - countless other, smaller improvements

Other:

 - In the Windows bundle, the debugger have been upgraded to gdb-7.7;
   Tcl/Tk has been upgraded to version 8.6.0.
 - New configure.user option to prefer clang over gcc if both are installed
 - New configure.user option to enable/disable C++ 11 compilance (-std=c++11)
   Note that this is NOT supported on Windows because of issues with the bundled
   MinGW 4.7 compiler.
 - Tcl/Tk version 8.5 is required, 8.6 is preferred; BLT is no longer in use

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.5


OMNeT++ 4.4.2 (August 2023)
---------------------------

This is a modernized version of omnetpp-4.4.1 that makes it possible to build
and use the now 9-year-old package in today's software environments. The
primary objective of this release is to facilitate the execution of old
simulation models, e.g. for reproducing simulation results or as a basis of
porting them to more recent versions of OMNeT++.

Changes:

 - Source code updated to eliminate warnings issued by modern C++ compilers.
   Backported several smaller fixes.

 - Adjusted to make the code compatible with more recent versions of Tcl/Tk
   libraries. In addition to the original Tcl/Tk-8.4 and BLT-2.4z, Tkenv now
   works with Tcl/Tk-8.6 and BLT-2.5.3, too.

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.


OMNeT++ 4.4.1 (Feb 2014)
------------------------

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.4.1


OMNeT++ 4.4 (Dec 2013)
----------------------

Sim:

 - Support for optional signal checking. When signal checking is turned on,
   signals emitted by modules/channels must be declared with a @signal property
   in the module's or channel's NED description; undeclared signals will result
   in a runtime error. Signal declarations are of the form

        @signal[<signalName>](type=<dataType>).

   <signalName> may contain wildcards (?,*). Type is optional; if present,
   data type can be long, unsigned long, double, simtime_t, string, or a
   registered class name. To allow NULL pointers, append a question mark to
   the class name. Example:

        @signal[receivedPk](type=cPacket?);

   This feature is controlled by the check-signals=<bool> configuration
   option, and it is turned off by default for now.

 - Support for @statistic-style declarative result recording on dynamically
   registered signals (e.g. "conn-<n>" where n is an integer). Instead of
   @statistic, add a @statisticTemplate property in the NED file, and for
   each new signal call ev.addResultRecorders(...).

 - Support for programmatically adding result filters and recorders on
   signals: cResultRecorder has been revised to allow result recorders
   to be used without a corresponding @statistic attribute, e.g. they
   can now be added to signals programmatically.

 - Further signals-related changes:
   - added emit(simsignal_t, const cObject*) to cComponent
   - added receiveSignal() overload for bool to cIListener
   - introduced SimsignalType which replaces the older cITimestampedValue::Type
   - added cObjectFactory::isInstance() method that effectively wraps a
     dynamic_cast
   - added Register_Abstract_Class() macro that allows abstract classes to be
     used in signal declarations as type.

 - Added cNEDValue::parseQuantity()

 - New result filter: removeRepeats

 - Feature: Just-in-Time debugging support. It adds the following new
   configuration options: debugger-attach-on-startup=<bool>,
   debugger-attach-on-error=<bool>, debugger-attach-command=<string>,
   debugger-attach-wait-time=<time>. When any of the first two options are
   set to true, the simulation kernel will launch an external debugger
   attached to the simulation process at the appropriate time. The default
   debugger on Linux is Nemiver.

NED:

 - More freedom in NED property syntax: (1) Hyphens, dots and colons are now
   allowed in property names and indices; and (2) values no longer need
   to be enclosed in quotes if they contain parens, provided that parens are
   balanced.

Tkenv:

 - Feature: Animation filters (right-click on any message, and select
   "Exclude messages like '...' from animation" from the context menu.)

 - Feature: "Debug Next Event" (Ctrl+F9). It causes the simulation program
   to stop in the debugger just before entering the handleMessage() call.

 - Fixes and workarounds for various issues on Mac OS X (poor animation speed,
   missing icons on OS X Mavericks, hang on the F5 key, etc.)

 - Fix: the "Filter Window Contents" dialog could lose state

 - Source cleanup (consistent naming style for Tcl procedures)

IDE:

 - Use Eclipse 4.3 as base platform

 - Support for project-specific images: icons from the "images/" folder of the
   project and its dependencies are automatically used by the NED editor, and
   added to the Tkenv image path when the simulation is launched. (The folder
   name is currently hardcoded; it will become configurable in future versions.)

 - Sequence Chart: Fixed bug that caused including the same reuses multiple times.
   Fixed error handling when the error occurs in paint during startup.

Improved support for C++11, Mac OS X:

 - The source now compiles without any warning with both gcc and clang, using
   the "-std=c++11 -Wall" flags (clang is the default compiler on OS X since
   version 10.9 Mavericks).

 - Added proper support for cross-compilation using ./configure --host=i686-w64-mingw32

 - Updated install instructions for OS X 10.9 Mavericks. Specifically, gdb is
   no longer distributed with OS X; users are expected to install it themselves
   from MacPorts, and digitally sign it.

 - IDE: fixed the launcher not to throw error if the toolchain is clang.

 - Updated the source to compile with both Bison 2.3 and 3.0

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.4


OMNeT++ 4.3.2 (August 2023)
---------------------------

This is a modernized version of omnetpp-4.3.1 that makes it possible to build
and use the now 10-year-old package in today's software environments. The
primary objective of this release is to facilitate the execution of old
simulation models, e.g. for reproducing simulation results or as a basis of
porting them to more recent versions of OMNeT++.

Changes:

 - Source code updated for C++11 compatibility, and to eliminate warnings
   issued by modern C++ compilers. Backported several smaller fixes.

 - Adjusted to make the code compatible with more recent versions of Tcl/Tk
   libraries. In addition to the original Tcl/Tk-8.4 and BLT-2.4z, Tkenv now
   works with Tcl/Tk-8.6 and BLT-2.5.3, too.

 - Updated to compile with both Bison 2.3 and 3.0.

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.


OMNeT++ 4.3.1 (Sept 2013)
-------------------------

The IDE was made more friendly to first-time users:

 - When the IDE is started with an empty workspace (e.g. on first-time
   launch), it offers the user the following options: (1) Import the OMNeT++
   sample simulations into the workspace; and (2) Download and install the
   INET Framework.

 - The latter function is also available via the Help > Install Simulation
   Models... menu item. The menu item brings up a dialog with the list of
   simulation models available for automated installation, and lets the user
   choose. Currently only the INET Framework is listed there, but it is
   planned to add further models.

 - When the IDE is started for the first time, it now displays some helpful
   introductory pages in the editor area: "Getting Started" (practical
   advice for getting past the first 10 minutes spent in the IDE);
   "At a Glance" (explains the common file types like NED, msg and ini, and
   their purposes); "OMNeT++ Samples" (describes each example simulation in
   two sentences, then provides links for opening the project, viewing the
   README, launching the simulation, etc.) These pages are also available
   from the Help system (except the last one, which has a dedicated menu item
   under the Help menu.)

Further IDE improvements:

 - IDE launcher script: fixed #670 (vmargs conflict between omnetpp.ini and
   the launcher script). This bug could cause IDE crashes by PermGenSpace
   exhaustion.

 - Analysis Tool, Output Vector View: show "Go to" actions in the context
   menu (instead of the view's pulldown menu)

 - Analysis Tool: fixed #389 (Useless items in the Statistic Name filter):
   The filter hints of combos is now computed from the result items filtered
   by the other 2 combos.

 - Analysis Tool: fixed #388 (add '*.host[*].*' variant to module filter hints)

 - Ability to import sample projects even if workspace is different from
   omnetpp/samples

 - Trying to launch a closed project will now offer opening it

 - NED documentation generator: fixed #672 (Illegal group reference error)

 - Changed default appearance of the main welcome page to be more user
   friendly

 - Some other bug fixes

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.3.1


OMNeT++ 4.3 (March 2013)
------------------------

IDE:

 - Updated to Eclipse 3.8.2.

 - The CDT debugger no longer stops at the main() function by default.

 - The bundled GDB has been downgraded to 7.4, due to problems with the one
   in the 4.3rc1 version

Tools:

 - The configure script now correctly detects the X headers on Mac OS X 10.8.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.3


OMNeT++ 4.3rc1 (Jan 2013)
-------------------------

IDE:

 - Analysis tool: added support for computed scalars. Read the updated the
   User Guide for further details.

 - Analysis tool: added 'Logarithmic X axis' option to scatter chart.

 - Added NED editor support for named channels.

 - Added support for opening files from the command line, i.e. use:
   `omnetpp Aloha.ned``

 - Added full screen mode (Ctrl-Shift-F11).

 - Usability improvements in the Analysis Tool.

 - Better error reporting in the Sequence Chart and Event Log views.

 - The CDT C++ code analyzer has been turned off as it was reporting
   too many false positives.

 - The IDE has been switched over to use CDT's DSF debugger instead of
   the older CDI.

 - The IDE no longer runs the C/C++ Indexer before build (the code now
   collects the information for makefile dependency generation by other
   means)

 - Added pretty printing of STL containers (std::map, etc), simtime_t
   and other objects to the debugger; see the updated User Guide for
   details.

 - Updated the bundled MinGW (GCC, linker and GDB).

 - Updated to Eclipse 3.8.1; the IDE now requires Java 1.6+.

NED:

 - Added named channels support, e.g. "...<--> eth1: EthernetChannel <-->...",
   with the purpose of making it easier to address channel objects when
   assigning parameters from ini files. Channel definitions can now specify
   a default name (via the @defaultname property), too.

Sim:

 - Added the cPatternMatcher and cMatchExpression utility classes to the API.
   cPatternMatcher is a glob-style pattern matching class. cMatchExpression
   builds on top of cPatternMatcher and lets you combine patterns with AND,
   OR, NOT for matching fields of arbitrary objects.

 - Added hasEncapsulatedPacket() to cPacket.

 - Implemented calculateWeightedSingleShortestPathsTo() in cTopology.

 - Signals implementation now allows static initialization of simsignal_t
   variables.

 - Fixed a bug in Expression where '-2m' was evaluated to '2' (meter was lost)

Tools:

 - The simulator now supports Mac OS X 10.8 (you need to install XQuartz; see
   the Install Guide)

 - Simplified makefile output: Makefiles now output only the filenames to the
   console instead of whole commands. This makes the build output less noisy.
   If you need the old behavior, use the 'V=1' (verbose on) option on
   the make command line.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.3rc1

OMNeT++ 4.2.3 (August 2023)
---------------------------

This is a modernized version of omnetpp-4.2.2 that makes it possible to build
and use the now 11-year-old package in today's software environments. The
primary objective of this release is to facilitate the execution of old
simulation models, e.g. for reproducing simulation results or as a basis of
porting them to more recent versions of OMNeT++.

Changes:

 - Source code updated for C++11 compatibility, and to eliminate warnings
   issued by modern C++ compilers. Backported several smaller fixes.

 - Adjusted to make the code compatible with more recent versions of Tcl/Tk
   libraries. In addition to the original Tcl/Tk-8.4 and BLT-2.4z, Tkenv now
   works with Tcl/Tk-8.6 and BLT-2.5.3, too.

 - Updated to compile with both Bison 2.3 and 3.0.

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.


OMNeT++ 4.2.2 (March 2012)
--------------------------

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.2.2


OMNeT++ 4.2.1 (Jan 2012)
------------------------

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.2.1


OMNeT++ 4.2 (Nov 2011)
----------------------

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.2


OMNeT++ 4.2rc2 (Nov 2011)
-------------------------

Documentation:

 - Revised and significantly expanded the "C++ Development" chapter in the
   User Guide to better assist model developers; smaller improvements in
   other chapters

Tools:

 - Updated MSYS and MinGW binaries in the Windows distribution.

 - The build system now creates 64-bit binaries on Mac OS X 10.6 and 10.7
   (if your processor supports it). Previously the -m32 flag was added to
   the GCC command line to force the creation of 32-bit binaries. (This
   was necessary because earlier versions of CDT did not support 64-bit
   debugging).

 - Enabled pretty-printing of variables in gdb (see below)

IDE:

 - Better C++ debugging experience: std::string, std::vector, std::map and
   other standard classes as well as simtime_t are now displayed in a
   meaningful way; simulation objects (cObject) display their full paths.
   This functionality is enabled by gdb pretty printer extensions written
   in Python, and is thus available for command-line gdb debugging too.
   For activating the pretty printers, see misc/gdb/README.

 - NED documentation generator improvements: better-looking tables in the
   generated documentation; running Doxygen is now cancellable; etc.

 - Dropped PowerPC support for the IDE because Mac OS X 10.7 no longer
   supports this architecture.

Sim:

 - Refactored operator= and copy constructor in all classes. Now they
   delegate to the same function in the super class, and the common
   part of the two is factored out to private copy() functions.

 - Fixed all warnings to make it compile using -Wall -Werror.

 - Coroutines used for 'activities' are now implemented using the
   swapcontext() POSIX call (if it is available on the system).

Sample simulations:

 - Database example revived. It demonstrates using database as configuration
   source, as storage for output scalars and vectors, and as source of
   network topology. Currently it works with MySQL.

Countless bug fixes and smaller improvements.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.2rc2


OMNeT++ 4.2rc1 (Aug 2011)
-------------------------

Ini files:

 - Ini files: iterations (${...}) can now refer to other iteration variables,
   using the dollar or the dollar-brace syntax ($var or ${var}). This
   improvement makes it possible to have loops where the inner iteration range
   depends on the outer one. When needed, the default top-down nesting order
   of iteration loops is modified (loops are reordered) to ensure that
   expressions only refer to more outer loop variables, but not to inner ones.
   When this is not possible, an error is generated with the "circular
   dependency" message. Variables are substituted textually, and the text
   resulting from substitution is NOT evaluated except in the '<from>..<to>
   step <step>' syntax, and in the 'constraint=' ini file option. CAUTION:
   textual substitution means that variables in arithmetic expressions should
   be protected with parentheses. The text substitution model was chosen for
   greater flexibility as well as the ability to produce a more consistent
   semantics. See src/envir/ChangeLog for more details.

 - Incompatible change: In the constraint= configuration option, variables now
   MUST be referenced with the dollar sign ($foo or ${foo} syntax), and also
   be surrounded with parens (to ensure precedence after the textual variable
   substitution). For example, the expression x+y<10 must be changed to
   $x+$y<10, or for greater safety, to ($x)+($y)<10.

 - Parallel iteration: it is now supported to use named iteration
   variables and parallel iteration together (${foo=...!bar} syntax.)
   This was not supported previously.

 - Some iteration-related error messages have been improved to provide
   more context.

Sim:

 - Result filter/recorder interfaces and registration macros made public
   (moved to src/sim), so that users can contribute their own result filters
   and recorders. Files: cresultlistener.h, cresultfilter.h  cresultrecorder.h.
   Result filters have to be subclassed from cResultFilter or the more specific
   cNumericResultFilter or cObjectResultFilter base classes and registered with
   the Register_ResultFilter() macro; result recorders have to be subclassed
   from cResultRecorder or the more specific cNumericResultRecorder, and
   registered with the Register_ResultRecorder() macro. After that, the new
   filter or recorder can be used in the source= and record= attributes of
   @statistic, and with the **.result-recording-modes configuration option.

 - The Define_Function() macros have been renamed to Define_NED_Math_Function().
   For backwards compatibility, the old macro definitions remain, but issue
   a deprecation warning. Note that the preferred way of defining new NED
   functions is now the Define_NED_Function() macro.

 - The API for custom NED functions defined via Define_NED_Function() has been
   changed. cDynamicExpression::Value (the old value class) has been factored
   out to a new cNEDValue class. Data members are no longer public, they can
   be manipulated via various methods. NOTE: This change is NOT backwards
   compatible: user-supplied NED functions will need to be revised.
   Look at src/sim/nedfunctions.cc for code examples.

 - Measurement unit support: added "mps" (m/s) and "kmph" (km/h) as recognized
   units; changed "Kbps" to the more standard "kbps"; changed byte multiples
   notation to use the now-standard IEC binary prefixes: KB -> KiB,
   MB -> MiB, GB -> GiB, TB -> TiB. The latter changes affect backwards
   compatibility, i.e. you may need to update existing models.

Tools:

 - opp_run: bugfix: release-mode simulation models compiled as shared libraries
   could not be run using a debug-mode opp_run program (they either crashed
   or reported that a module was not registered with the Register_Module()
   macro.) As part of the fix, a new opp_run_release program has been
   introduced, and opp_run (which is now always compiled as debug) delegates
   to it when necessary.
      Due to makefile changes related to the fix, you may need to re-create the
   makefiles of your simulation when upgrading to OMNeT++ 4.2. (You only need
   to do that when working on the command line. The IDE automatically recreates
   the makefiles, so no action is needed on your part if you are using only the
   IDE.) See src/envir/ChangeLog for details of this change.

 - Implemented number filter expressions using multiple inputs for statistics source
   expressions. For example:

        @statistic[dropRate](source="count(packetDropped)/count(packetSent)");

 - opp_msgc: improvement: announcements and type definitions now observe
   whether they are above the namespace declaration (=outside the namespace)
   or below it (=inside the namespace). This change makes it consistent with
   cplusplus blocks that had this behavior for quite some time. A related
   improvement is that type lookup in namespaces has been modified to make it
   possible to use unqualified names when declaring fields. See
   src/nedxml/ChangeLog for details.

 - Akaroa support improved; samples/aloha/akaroa.ini was added as example

Documentation:

 - Several chapters in the User Manual have been updated; especially, the
   Message Definitions section was turned into a separate chapter, and
   was completely revised.

IDE:

 - Upgraded to Eclipse 3.7

 - Added Eclipse Marketplace to the Help menu. You can now install
   additional features from the market.

 - Before running the simulations, the IDE will show the command line in
   the console view to help you execute the same simulation from the
   command line.

 - Ctrl-Tab now works on freshly opened NED files even if you do not explicitly
   select a simple module. In that case, the IDE opens the .h and .cc files
   associated with the first simple module in the NED file.

 - Improvement on the Manage Build Configurations dialog: newly created build
   configurations are now configured, by copying the source entries from one
   of the existing build configurations.

 - Project Makemake Options property page: overhaul for better usability.

 - Documentation generator: turn on JAVADOC_AUTOBRIEF in newly generated
   doxy.cfg files. (When this option is on, there is no need for @brief
   in C++ doxy comments; instead the first sentence of the comment is taken
   automatically as brief description.)

 - Bug fixes on: IDE makefile generator; the Project Features feature;
   the Inifile Editor (hover info, content assist, etc); the NED editor
   (display string proposals for connections and channels, etc.); Organize
   Imports feature; NED documentation generator; Simulation Launcher (e.g.
   launching folders with simulations in them works again);

 - SVN support removed from the IDE. It had very few users, and can be
   installed from the Market if needed.

 - Removed Animation Player as it was only a "technology preview" bundled
   with the beta releases to gather feedback from the user community. The
   animator is scheduled to appear in the next major OMNeT++ release.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.2rc1


OMNeT++ 4.2b2 (May 2011)
------------------------

Most important feature: "Project Features" (see below).

NED:

 - Added support for conditional submodules. Syntax:

        udp: UDP if needsUDP {...}
        tcp: <tcpType> like ITCP if needsTCP {...}

 - Added the xml() NED function, which accepts a string argument and parses
   it as XML. Its most notable use is eliminating "empty.xml" files from INET:
   one can use xml("<root/>") in NED files instead.

 - Implemented default value for parametric submodule and channel types.
   NED now supports the following syntax:

        mobility: <default("NullMobility")> like IMobility;

   The effect is that the NullMobility NED type will be used for the mobility
   submodule if the type name is not specified otherwise, e.g. in the config-
   uration with something like

        **.mobility.type-name = "ConstSpeedMobility"

 - Added the firstAvailable() NED function, which is helpful with the Project
   Features feature (see below). It accepts any number of strings (see new
   varargs support for NED functions), interprets them as NED type names
   (either short names or fully qualified names), and returns the first one
   that exists and is also "available" (its C++ implementation class exists).
   Example usage:

        tcp: <default(firstAvailable("TCP_lwIP", "TCP_NSC", "TCP"))> {..}

   It chooses the TCP_lwIP, TCP_NSC or TCP module type for the tcp submodule,
   in this order, unless the type is explicitly defined to be something else
   in the configuration.

 - Parametric submodule type can now also be specified in NED files, using
   patterns that end in the new "typename" keyword. An example:

        network Net {
            parameters:
                host[*].tcp.typename = "TCP_lwIP";
                ...
        }

Ini files:

 - The "type-name" per-object configuration option (**.typename=) has been
   renamed in to "typename", for consistency with the similar new NED feature.

Sim:

 - Implemented varargs support for NED functions. If the signature (see
   Define_NED_Function() and Define_NED_Function2() macros) ends in ", ...",
   then the function will accept any number of additional arguments of any type.
   At runtime, the implementation has access to both the actual number and
   types of args. When passing extra args, optional arguments (those marked
   with '?' in the signature) must all be present, i.e. varargs can only come
   when all typed args are there.

IDE:

 - Upgraded to Eclipse 3.6.2

 - Implemented the "Project Features" feature, which makes it possible to
   disable unused parts ("features") of a project to reduce compilation time
   and complexity. It has been invented to help INET Framework users and
   developers deal with the growing size of the framework. Features are
   described in the project's .oppfeatures file, authored by INET developers.
   Users can activate/deactivate features in the Project Features page of
   the Project Properties dialog (this page is directly accessible from the
   Project menu and from the Build Configurations submenu of the project's
   context menu in Project Explorer). Features map to NED packages and C++
   source folders; disabling a feature maps to NED package exclusion, and
   folder exclusion in CDT (C++ Development) configuration. Features can also
   define C/C++ symbols (for #ifdefs), and extra libraries to link with.
   At build time, the IDE checks the project's configuration (NED, CDT) and
   if it is inconsistent with the selected features, it offers fixing it.
   Features can also be used from the command line by exporting Makefiles
   (or opp_makemake commands) that reflect a particular enablement of features.

 - Support for excluding (disabling) NED packages. This feature is needed for
   the Project Features feature. Exclusions can be edited on the NED Source
   Folders project property page, and are saved into the project's .nedfolders
   file.

IDE/Animation Player:

 - Implemented lazy loading of the eventlog file: the tool can now animate
   large eventlog files with acceptable performance.

 - Animation effects refined, e.g. packet transmission on a datarate channel.

 - Heavy bugfixing. Note that the tool is still being actively developed, and
   it is generally not yet ready for everyday use.

IDE/Inifile Editor:

 - Usability: the editor now comes up with the page (text or form) that was
   used last time, and not always with the form editor. (When you flip the
   editor to the other page, the page type is stored in the preferences.
   When an editor is opened, the setting is read from the preferences and
   the corresponding page is activated.)

 - Improved text hover (F2) and hyperlink (Ctrl+click) support: the editor
   can now show information and go to the definition of modules that occur
   in the inifile key. For example, for a `**.host[*].tcp.sackSupport = true`
   line, the user can hover over (or Ctrl+click) the `host[*]` part, and the
   editor will show relevant information and go to the definition of the
   `host[]` submodule vector in the NED network description.

 - Improved content assist: per-object configuration options are now filtered
   by the type of object that the key refers to; for example, if you type
   `**.ppp[*].txPkBytes.` <Ctrl+Space>, the editor will know from the NED files
   that txPkBytes is a statistic, and offer completions accordingly.

 - Content assist: different types of completions (modules, parameters,
   statistics, configuration options, etc) are now marked with icons.

 - Markers on included inifiles are now removed when the main inifile is
   closed. (Fix for #176)

 - Added the Copy action to the Module Parameters view context menu: you can
   now select the key for an unassigned parameter in the view, and copy/paste
   it into the editor area.

IDE/Wizards:

 - New Tictoc Example and New Source-Sink Example wizards: fix: the root
   Makefile executed opp_makemake in the src/ folder at each build,
   overwriting the IDE-generated Makefile.

 - New OMNeT++ Project wizard template: now it is possible to set "custom make"
   for a folder: specify `<foldername>:CUSTOM` in the makemakeOptions= template
   variable in template.properties. Use `.` to name the project root folder.
   Example: `makemakeOptions = .: CUSTOM, src: --deep -meta:recurse...`

 - New option for template.properties: preRunTemplate=<fti-file-name>. Example:
   preRunTemplate = main.fti. The option causes the given template file to be
   evaluated for its side effects, just after the user hits Finish but before
   any file is copied or template is evaluated. It is not allowed to produce
   text output, but the variables it sets will be substituted into other
   template.properties variables, and will also be available in other template
   files. (Note: all other templates are run in isolation, and cannot change
   variables for other templates!) The preRunTemplate option is the only way
   to programmatically set the value of makemakeOptions and similar
   template.properties options.

IDE/Graphical Editor:

 - Switched back double-click behavior to Open NED Type action. The Properties
   dialog can now opened with Ctrl+Enter (Alt+Enter was unavailable.)

IDE/Sequence Chart and Event Log:

 - Added show/hide options for arrows that represent mixed dependencies.
   (Such arrows are drawn when the sequence chart is filtered, and represent
   e.g. a sequence of message sendings and self-messages)

 - Eventlog: enhanced performance of event tracing filter

 - Simulations now record the termination message and result code into the
   eventlog file

 - Note: eventlog file format has changed (in order to better serve the
   Animation Tool) -- .elog files recorded with any prior version of OMNeT++
   have to be re-recorded.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.2b2


OMNeT++ 4.2b1 (Feb 2011)
------------------------

Partial list of changes since 4.1 (see ChangeLog files in individual folders
for a more complete list):

Simulation runtime:

 - Removed #include <winsock.h> from <platdep/timeutil.h>; the reason
   is that it conflicted with lwIP code recently integrated into INET.
   <platdep/sockets.h> still includes <winsock.h>, but now
   <platdep/sockets.h> (and <platdep/timeutil.h>) MUST precede <omnetpp.h>
   in all source files.

 - Ini files: implemented multiple inheritance for sections. Syntax:

        [Config Foobar]
        extends = Foo, Bar

   When the runtime looks for a config option or param assignment, sections
   are examined in a "fallback order" until the first match is found. In the
   above example the order would be: Foobar, Foo, Bar, General.
   OMNeT++ uses C3 linearization to compute the section fallback order,
   see e.g. http://en.wikipedia.org/wiki/C3_linearization

 - Ini files: better parsing for iteration variables, e.g. the value in
   ${x=uniform(1,2)} is now parsed as one item, not as "uniform(1" and "2)".
   Nested parens/brackets/braces and string literals are recognized;
   escaping commas and close-braces is now possible using backslash.

 - NED: fixed deep parameter assignments: parameter references were
   interpreted in the wrong context. Deep parameter assignments are when
   an inifile-like pattern is used to name the parameter(s) to be set, as in:

        network Inet {
            parameters:
                host[*].tcp.nagleEnabled = true;
            ...
        }

 - Resolved spurious errors: 'Class "..." not found -- perhaps its code
   was not linked in, or the class wasn't registered with Register_Class()'
   Added a check to ensure that the debug and the release versions of
   the simulation library are not loaded at the same time. This problem
   occurred when a model was built as 'release' and the debug version of
   'opp_run' was used to start it. As a result, the simulation seemingly
   started but complained about missing classes.

 - Fix: Some classes were thrown out by the linker if OMNeT++ was statically
   built, and the simulation was complaining at runtime about the missing
   cCompoundModule class.

 - Eventlog recording: .elog file format change: message creations and
   duplications (cloning) are now recorded; MC (ModuleCreated) entries now
   precede GC (GateCreated) entries. IMPORTANT: The sequence chart in the
   IDE will only work on .elog files in the new format, existing event logs
   need to be re-recorded!

IDE/Animation Player:

 - This is a new component in the IDE, you can access it by right-clicking
   an .elog file and selecting "Open With / OMNeT++ Animation Player" from
   the context menu. This is a preliminary implementation (Technology
   Preview) not yet optimized for large files, the practical maximum file
   size it can handle is about 10MB.

IDE/C++ Development:

 - Added a new "Clean Local" item to the project's context menu that does
   not clean referenced projects. This resolves the problem that CDT's
   "Clean Project" command also cleans the referenced projects, e.g. if
   you have a Foo project that depends on INET, cleaning Foo also cleans
   INET which takes a long time to recompile. Now you can use the
   "Clean Local" command to clean Foo only.

 - Added new "OMNeT++ Code Formatter" profile. This coding convention follows
   the rules used in the OMNeT++ kernel and the INET Framework. (No tabs, etc.)

 - Added MachO64 binary parsers and error parsers. This makes it possible to
   debug a 64-bit executable on Mac OS X 10.6

 - Linux only: Added support for profiling with Valgrind. To activate this
   feature, select "Profile as... / OMNeT++ Simulation" from the Project
   Explorer context menu. This feature requires Valgrind to be installed
   on your system. (Note: the main IDE toolbar does not show a Profile button
   next to the Run and Debug buttons at this time, but profiling is still
   available via the Project Explorer context menu.)

 - The Makemake page in Project Properties can now fix the "Source Folders
   differ across configurations" problem in the project setup, which usually
   occurs when you add a build configuration to an existing project later.
   The dialog now displays a "Fix it" link.

IDE/Graphical Editor:

 - Several performance optimizations for the NED Editor. It can open much larger
   files than before. NED validation is done now in a background thread and
   does not block the UI.

 - Select 'NED Type Selection' dialog now works correctly on first open. It
   shows the defining projects for the types.

 - Enhanced drag and drop capabilities in NED Graphical Editor. You can
   drop NED types into a compound module either to create submodules
   (if you drop it into the submodule area), or to turn it into
   an inner type (if you drop on the title of the compound module).

 - The Graphical Editor now displays the file's package name at the top.

 - Graphical Editor displays self-connections as arcs in the module's upper
   right corner.

 - The resize/move feedback figures have been changed from a simple outline
   to translucent blue.

 - Added a 'Properties' dialog that can edit one or several objects
   at a time, and allows you to change graphical and type information.
   It also has a preview panel.

 - Enhanced editing of type fields in the 'Property View'. The editor correctly
   enumerates the possible choices and adds an import statement if necessary.

 - 'Open Type' and 'Open Supertype' have been reassigned to the F3 function key.
   Double-click now opens the 'Properties' dialog.

 - The IDE now uses the use native (C++) layouter code for placing unpinned
   submodules, which means better performance and consistency with the
   simulation runtime (Tkenv).

IDE/Inifile Editor:

 - Support for inifile improvements in this release, namely multiple section
   inheritance and better parsing for iteration variables

 - Performance improvements: inifile is now analyzed in the background so that
   it does not block the UI, and features that need its results (hover text,
   content assist, etc) have a timeout how long they are willing to wait for it
   (see bug #132 for details). It is now also possible to disable inifile
   analysis. Further performance and UI responsiveness improvements are planned.

IDE/Sequence Chart:

 - Added 'Go to Simulation Time' and 'Go to Event Number' in the context menu.

 - 'Go to Simulation Time' in the 'Event Log' view now supports relative values.

 - The Sequence Chart tool now knows about message creations and duplications
   (cloning), and as a result, in certain situations it is now able to better
   identify relationships between events and draw a better sequence chart.
   NOTE: Due to changes in the .elog file format, the tool will only work with
   newly recorded files. Existing event logs need to be re-recorded!

IDE/Launcher:

 - The command-line for launching the simulation now uses relative paths instead
   of absolute ones. This makes the starting command-line easier to understand.

IDE/Documentation Generator:

 - Added tilde notation as an alternative to automatic hyperlinking of module
   names in comments. The latter was often too aggressive (e.g. linking to the
   IP module in the phrase "IP address" is wrong). Automatic hyperlinking can
   now be turned off in the generator, and then the user is expected to mark
   words to be hyperlinked with a tilde: "the ~IP module". The INET Framework
   has already been converted to use the tilde notation.

 - Added Javascript to autoload frameset page if a content page is opened by
   itself. This is useful when URLs to specific content pages are posted on
   the mailing list or sent over email.

 - Implemented @include in NED comments. Lines in the included file don't have
   to start with "//", but otherwise they are processed exactly as NED comment
   text.

 - Fix: External pages (@externalpage) now appear under the "Selected Topics"
   node of the navigation tree in the generated documentation.

 - Several minor fixes: recognize the `<u>` (underline) HTML tag; sanitize the
   file names for @page; added/refined test cases

Tkenv:

 - Parallel simulation: Placeholder modules are now displayed with a semi-
   transparent icon (or if it's a rectangle or oval, with dotted border).

 - Fix (bug #248): compute coordinates (and sizes) in double and not int;
   necessary if background scaling (bgs display string tag) is set up.

 - Better choice of default user interface font, especially on Linux.

 - The generic UI font and the font used in graphics are now configurable
   in the Simulation Options dialog.

 - The default font for log text is now proportional (same as the generic UI
   font), for better readability and space efficiency. It can be customized
   (switched back to a monospace font, etc) in the Simulation Options dialog.

 - Minor: menu items have been changed to title case

 - Tcl errors are now logged into .tkenvlog instead of dumping them on stdout.

Misc:

 - Added -g option to 'scavetool scalar' command. You can specify how the
   scalars are grouped into columns/variables.

 - The build system can now build large projects on windows. The 32K command-line
   limitation has been resolved.

 - Updated base system to Eclipse 3.6.1

 - Added a detailed IDE change log file doc/IDE-Changes

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.2b1


OMNeT++ 4.1.1 (August 2023)
---------------------------

This is a modernized version of omnetpp-4.1 that makes it possible to build
and use the now 13-year-old package in today's software environments. The
primary objective of this release is to facilitate the execution of old
simulation models, e.g. for reproducing simulation results or as a basis of
porting them to more recent versions of OMNeT++.

Changes:

 - Source code updated for C++11 compatibility, and to eliminate warnings
   issued by modern C++ compilers. Backported several smaller fixes.

 - Adjusted to make the code compatible with more recent versions of Tcl/Tk
   libraries. In addition to the original Tcl/Tk-8.4 and BLT-2.4z, Tkenv now
   works with Tcl/Tk-8.6 and BLT-2.5.3, too.

 - Updated to compile with both Bison 2.3 and 3.0.

 - Backported the setcontext()/switchcontext() based coroutine library from
   omnetpp-4.2, to make activity() modules work again. (The original "portable
   coroutines" library which used setjmp()/longjmp() to switch stacks no longer
   works with recent glibc versions.)

 - Updated the `setenv` and `configure` scripts, to make them more consistent
   with those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.

 - Added missing dependency to enable parallel build.


OMNeT++ 4.1 (June 2010)
-----------------------
Changes after 4.1rc2:

 - Testing on various Linux distros, updated Installation Guide

 - Simulation error messages now contain the event number and the simulation
   time

 - Tkenv: contents of simulation error dialogs are now inserted into the
   main window as well, for reference and easier copy/paste

 - Various bug fixes

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.1


OMNeT++ 4.1 rc2 (June 2010)
-------------------------------
Simulation kernel:

 - cChannel: added forceTransmissionFinishTime(simtime t), so that
   channels can support aborted transmissions (needed e.g. for Ethernet).

 - cChannel: added getNominalDatarate(). This is often needed in models,
   for example in INET to fill in the interface entry.

 - added forgetXMLDocument(filename) and flushXMLDocumentCache() to
   cEnvir (see bug #131)

 - cPar: added a note to the xmlValue() method that the lifetime of
   the returned objects may be limited. Modules should NOT hang on to
   cXMLElement pointers between events!

Envir:

 - Ini file inclusion now behaves as textual substitution: if an included
   ini file does not contain [General] or [Config XXX] lines, the file's
   content will be treated as if copy/pasted into the section containing
   the include. Ini files that do not start with a section heading will
   now be accepted and treated as if they started with [General].
   Rationale: allow factoring out orthogonal pieces of config into
   separate files.

IDE:

 - Added support for the `m` tag on connections allowing to set the preferred
   directions of a connection between a submodule and its compound module.

Misc:

 - various fixes on Tkenv, the IDE and the build system

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.1rc2


OMNeT++ 4.1 rc1 (May 2010)
--------------------------

Partial list of changes since 4.1b4 (see ChangeLog files in individual folders
for a more complete list):

Simulation kernel:

 - Enter_Method_Silent() may now take arguments (a printf-like arg list),
   and it logs the call into the event log file.

 - Re-added cChannel::calculateDuration() that was removed in 4.1b3

 - Fix: exponential(), normal(), truncnormal() and geometric() could produce
   +INF, due to log(dblrand()) in their code where dblrand() can produce zero.
   As a side-effect of the fix, the fingerprints of all simulations that use
   those distributions as input have changed.

NED:

 - New NED function: expand(). It substitutes `${}` config variables into a string.
   Example usage in a NED parameter:

        string fileName = default(expand("${resultdir}/${configname}-${runnumber}.txt"));

MSG:

 - Implemented @sizetype property for array fields; it can be used to specify
   the C++ type of the array size and array indices (by default it is unsigned
   int).

 - Fix: A "using namespace" line inside a cplusplus {{...}} block confused the
   parser.

Tkenv:

 - Implemented bilinear filtering for image/icon resize in Tkenv.

 - Simulation Options dialog: layouting algorithm can now be chosen
   from Fast, Advanced and Adaptive (default: Adaptive)

 - Implemented minimum icon size, resize window on layout and auto-adjust zoom
   options. New controls in the Simulation Options dialog:
    - "[] Resize window to fit network with current zoom level first"
    - "[] Adjust zoom so that network fills window"
    - "Minimum icon size when zoomed out (pixels)"

 - Added hotkeys for zoom in/out: Ctrl+N and Ctrl+M. Ctrl+N was already
   used by "Toggle module name", it was reassigned to Ctrl+D.

 - Improvements on the classic ("fast") layouting algorithm

 - Application icon for Tkenv

 - Several bug fixes (see ChangeLog)

IDE:

 - Analysis Tool: charts now support a much higher zoom level

 - NED Documentation Generator: generating full inheritance and usage diagrams
   can now be turned off (they were often too busy to be useful)

 - Several other bug fixes

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.1rc1


OMNeT++ 4.1 beta 4 (April 2010)
-------------------------------
Simulation kernel:
 - Added statistics to the built-in channel types, ned.DatarateChannel and
   ned.DelayChannel. They are not recorded by default; recording can be turned
   on with "**.channel.result-recording-modes = all" in the configuration.
   DatarateChannel records the following: busy state (as vector);
   utilization (time average as scalar); number of packets (as scalar);
   number of bytes (as scalar); number of packets discarded in channel
   disabled state (as scalar); throughput (bps, as scalar). You can see the
   declarations of these statistics by running "opp_run -h neddecls".

 - handleParameterChange() is now called for each component with NULL
   as the parameterName after the last stage of the initialization phase.
   This allows the components to refresh their cached parameter values
   if some parameters were changed during the initialization phase.
   Existing code needs to be updated to accept name=NULL and interpret
   it as "any parameter".

 - signals framework: changes in the cIListener interface since the previous
   beta; changed cTimestampedValue class (which is used to emit a signal for
   statistics purposes with a different timestamp than the current simulation
   time) to support all data types

 - histogram classes: added getters getNumFirstVals() and getRangeExtension-
   Factor(); for integer histograms, cell boundaries are now at whole numbers,
   not halfs; implemented dup(); default number of cells changed

 - cChannel's process() method (which was introduced in the previous beta)
   was renamed to processMessage()

NED:
 - Inner types can now refer to the parameters of their enclosing type.

 - Statistics recording: introduced the `source=key` within the @statistic
   property, which lets you use a derived value as statistic. For example:
   `@statistic[dropPercent](source=100*dropRatio;record=vector,last)`,
   where dropRatio is a signal emitted from the module. This feature also
   lets one emit message or packet objects as signals, and record some
   property of them as statistics. For example:
   `@statistic[bytesSent](source=packetBytes(pkSent);record=sum)`.

 - The @signal property can now be used to declare signals, for example
   those that can be used as source for @statistic. An example:
   `@signal[pkSent](type=cPacket)`.

 - Introduced optional statistic recording modes: now you can add optional
   items to the record= key of the @statistic property, by appending "?"
   to them. For example: `@statistic[eed](record=vector?,histogram?,mean)`.
   By default, only non-optional items are active; optional ones can be
   turned on with **.result-recording-modes = all (see below).

 - Implemented the @dynamic property. When a submodule is marked with @dynamic
   or @dynamic(true) inside a compound module, the submodule will not be
   instantiated at network setup; rather, it is expected that it will be
   instantiated at runtime, using dynamic module creation. A module created
   this way will pick up parameter values from the submodule declaration in
   the NED file as well as from the ini file. @dynamic submodules are
   displayed in the graphical NED editor as semi-transparent, but otherwise
   can be edited (and configured in the inifile editor) like other submodules.

Envir:
 - The `result-recording-mode=` configuration option was renamed to
   `result-recording-modes=`.

 - result-recording-modes now has support for optional recording modes (those
   marked with "?" in `@statistic(record=..)`, see above). The value "default"
   selects non-optional modes from `@statistic(record=..)`, and "all" selects
   all of them. The list can be fine-tuned by adding recording modes prefixed
   with "+" or "-" (for "add" and "remove", respectively). A lone "-" disables
   all recording modes. Some examples:

        **.result-recording-modes = default,+vector,-histogram
        **.result-recording-modes = all,-vector
        **.result-recording-modes = last

   The first one selects non-optional recording modes plus records everything
   as output vector, but disables histogram recording. The second one selects
   all recording modes but disables vector recording. The third one only
   records the last value from all statistics. There are further examples in
   src/envir/ChangeLog.

 - Created a result filter and result recorder API, for filters/recorders
   that can be used in the source= and record= keys of @statistic.
   Result filters must subclass from ResultFilter, recorders subclass from
   ResultRecorder; classes must be registered with the Register_ResultFilter()
   and Register_ResultRecorder() macros.

 - New command-line options: -h neddecls prints the NED declarations for
   built-in types (ned.DatarateChannel, etc); -h resultfilters and -h
   resultrecorders prints the list of available result filters and
   result recorders (such as min, max, mean, timeavg, last, histogram,
   vector, packetBytes, packetBits, etc).

 - Result file change: integer histograms are now marked with "attr type int"
   instead of "attr isDiscrete 1" in output scalar files.

IDE:
 - NED editor: added banner comment for module/channel types created
   graphically

 - Ini file editor usability: more specific names for table columns in form
   editor; split "Output Files" page into "Result Recording" and "Event Log"
   pages in form editor

 - Analysis editor: separate horizontal and vertical zoom controls;
   more usable context menu for charts; result file reading made more
   robust (better error messages on errors); at the same time, removed
   support for reading old-style (3.x) vector files

 - Sequence Chart: more usable context menu; display of method calls and
   message reuse arrows turned off by default

 - Improved the display of @statistic and @signal properties in Module
   Hierarchy view

 - Dozens of bugfixes and other improvements

Tooling:
 - Updated MinGW and MSYS tools in the Windows distribution. The new MSYS
   now should work on Win64 (although the compiler still produces 32bit code).
   The GCC compiler is updated to a pre-release version of MinGW 4.5 (this
   solves the debugging issues on Windows platform).

Build system:
 - Extra long command lines are supported now for the MinGW linker. (Uses a
   temporary file to pass args to the linker if the command line is >32K.)

Tkenv:
 - It is now possible to turn eventlog recording on/off interactively, not only
   via omnetpp.ini options; see "Toggle eventlog recording" toolbar button.

 - Module initialization messages are now not printed; we added an option to
   turn them back on in the Simulation Options dialog.

Cmdenv:
 - Printing of module initialization messages is now suppressed in express mode

Bugfixes since beta 3:
 - in express mode, log messages (ev<<) were not recorded into the eventlog
 - NED graphical editor added explicit "ned.IdealChannel" to connections
 - NED editor failed to add blank lines between components created graphically
 - pack/unpack error for "long long" type fixed (on MinGW compiler)
 - fixed (reduced) the size of filter text field in the NED editor palette
 - Go To C++ Definition in NED file is working again
 - opp_test: %subst to understand backrefs ($1) in the replacement string
 - many other bug fixes

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.1b4


OMNeT++ 4.1 beta 3 (March 2010)
-------------------------------
Simulation library:
 - New channel programming model. The user needs to override the process()
   method of cChannel, which has the following parameters: the message;
   the time the message will reach the source gate of the connection; and
   a reference to a struct (cChannel::result_t) in which the method should
   return the results. result_t fields to be filled in are the propagation
   delay, the transmission duration, and a flag to indicate whether to model
   that the message got lost in the channel (when this is set, the simulation
   kernel will delete the message after process() returns).

 - New class: cHistogram. It is a merger of cDoubleHistogram and cLongHistogram
   (which have been kept for compatibility). cHistogram can work in "integers"
   or "doubles" mode; the difference is that in "integers mode", cell boundaries
   will be chosen to be integers. The default behavior is to automatically
   choose the mode after precollecting some (by default 100) observations.
   The significance of cHistogram is that the "histogram" recording mode
   of the new statistics framework (see below) uses cHistogram in its default
   setup.

 - New statistics recording framework using signals. The module's C++ code
   emits statistics as signals, and collector objects are attached externally,
   by the simulation framework. Statistics are declared in NED files using
   @statistic properties, and actual recording (whether to record values as
   output vector, or as histogram, or the average/minimum/maximum etc value
   as scalars) can be configured from ini files. The runtime cost of
   statistics that are not recorded is minimal, which allows module authors
   to emit lots of statistics in the code, and let the end users decide which
   statistics they want to record and in what form. Earlier, such functionality
   would have required extensive C++ programming in module.

 - Added a simTime() NED function that returns the current simulation time.
   simTime() makes it possible to create time-dependent distributions, e.g.:
   interArrivalTime = exponential(simTime()<10s ? 0.5s : 2s)

Envir:
 - New configuration option: warmup-period. This is associated with the new
   statistics recording framework: when specified, values produced during
   the first X seconds of the simulation will not be calculated in the
   results. For existing statistics that are calculated manually inside
   modules and recorded e.g. as recordScalar() in finish(), the module C++
   code of course needs to be modified to respect the warmup period; the
   length of the warmup period can be obtained from C++ code as
   simulation.getWarmupPeriod().

 - New configuration option: result-recording-mode (renamed to ~-modes in
   beta4); this option specifies what to record from statistics declared
   in NED. Example value: vector,histogram,timeavg,min,max.

 - New configuration option: debug-statistics-recording. When turned on,
   the simulation program will dump what result recorders have been attached
   to which module signals.

 - Renamed "vector-recording-interval" configuration option to
   "vector-recording-intervals" (plural) which is a more precise name, plus
   more consistent with eventlog-recording-intervals.

NED:
 - New properties associated with the statistics recording framework (above):
   @signal defines a signal emitted by the module, and @statistic defines
   a statistic that is available for recording. (A statistic can use a signal
   "as is", or can be some function of it; for example, a signal can be
   a packet, and a corresponding statistic can be the length of the packet.)

IDE:
 - The IDE is now based on Eclipse 3.5.2 and CDT 6.0.2.

 - Content assist for icons (with preview!), colors and other display string
   tags in the NED source editor. Content assist is now also supported
   for channel parameters, and @signal and @statistic properties.

 - Statistic and signal declarations are now shown in the Module Hierarchy view.

 - Content assist for statistics names in the ini file editor.

 - Several usability enhancement in the Analysis Editor:
   - SVG export for charts.
   - Redesigned filter interface on the Browse Data page provides more space.
   - Tree view in the Browse Data page: it shows results in a tree organized
     along runs, modules, results and result properties as tree levels.
     Tree levels can be freely configured.
   - Charts in the Chart Sheet are resized fill the editor window automatically.
   - The number of columns can be configured on the Chart Sheet.

 - Direct method calls can be visualized in Sequence Charts (needs to be
   turned on manually)

 - Generated NED documentation now includes signal and statistic declarations.

 - New FreeMarker template editor for editing of ".ftl" files, used for the new
   templated wizards feature (see 4.1b1).

Tooling:
 - The git package has been updated to version 1.6.5.

Documentation:
 - Extensive documentation review and proofreading.

Bugfixes since beta 2:
 - Fixed regressions related to parameter handling.
 - The IDE no longer crashes on multi-processor Windows machines.
 - git commands are working correctly now on Windows (git merge etc.)
 - The new version of Eclipse platform (3.5.2) solves the 'dead button' problem
   found on Linux systems with the latest version of GTK.
 - Documentation generator now correctly generates docs from NED files
   containing the @titlepage property.
 - Boolean parameters now can be recorded as scalars.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.0b3


OMNeT++ 4.1 beta 2 (Jan 2010)
-----------------------------
API:
 - Added a "signal mechanism" to support emitting signals from a model. Signals
   can be used to decouple production of simulation results (statistics) from
   recording them, for model change notifications, publish-subscribe style
   module communication, and several other purposes. See the Manual for more
   information.

Simulation kernel:
 - Source files are now compatible with GCC 4.4 version.

 - Performance and memory optimizations.

IDE:
 - Result charts and sequence diagrams can now be exported in SVG format. This
   allows the creation of publication quality charts and diagrams.

Tooling:
 - The bundled MinGW GCC compiler and MSYS binaries have been updated
   to their latest version (on Windows).

Documentation:
 - A very detailed Installation Guide has been created. It provides step-by-step
   installation instructions for the most popular operating systems.

 - An IDE Developers Guide has been created that describes how to enhance the
   OMNeT++ IDE and extend it with new functionality.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.0b2


OMNeT++ 4.1 beta 1 (Jan 2010)
-----------------------------

Simulation kernel:
 - Added setCapacity()/getCapacity() to cArray. setCapacity() can be used
   to trim back the internally allocated array after a large number of
   items have been removed from it.

 - cGate: added getNameSuffix() method.

 - FES performance improvement: cMessageHeap was changed to more efficiently
   handle the insertion and removal of events scheduled for the current
   simulation time. Such events are actually quite common due to plain
   (zero delay, zero datarate) connections between modules.

 - Performance improvement: cMessage::getArrivalTime() was consuming
   10-20% of CPU time in many configurations. Solution is return
   a reference (const simtime_t&) instead of a value.

 - Fixed cPacketQueue::remove() incorrect behavior when called
   with a packet that was not in the queue

 - Dynamic module creation: modules now pick up parameter values from the
   parent compound module's submodules section in the NED file. This will be
   useful with the @dynamic() submodule property (see later beta releases).

 - Fix: parse() and some other cPar methods did not cause handleParameter-
   Changed() to be called on the owner module

 - Many other bugfixes and improvements; see include/ChangeLog and
   src/sim/ChangeLog

NED:
 - Deep parameter assignments: it is now possible to assign parameters of
   sub-submodules, sub-sub-submodules, etc. from a compound module, using
   inifile-like wildcards. For example, **.tcp.mss=1024 may be used in a
   network NED file to configure TCPs inside all hosts in the network.
   This feature also involves a slight NED DTD change (doc/ned2.dtd).

 - Re-assigning a parameter that has an already assigned non-default value
   in an ancestor is no longer allowed.

 - Compound modules may now have a custom C++ implementation class,
   i.e. the @class() property is now understood for compound modules as well

 - Fix: operands of the '%' (integer modulo) NED operator now must be
   dimensionless (otherwise the result of truncation depends on the choice
   of the measurement unit)

Msg files:
 - opp_msgc: fixed namespace support. It is now possible to use types
   from other namespaces as base class, field type, or field enum.
   The src/nedxml/ChangeLog file contains some examples for this.

 - opp_msgc: improved cplusplus {{..}} blocks: their contents can be placed
   into or outside the namespace {...} block in the generated header file,
   depending on whether the block occurs before or after the namespace
   declaration in the msg file.

 - opp_msgc: removed support for enum inheritance (which was an undocumented
   and generally unused feature, with several issues)

 - opp_msgc: the message compiler can now be used to generate reflection info
   (cClassDescriptor) for existing classes. If you add @existingClass(true)
   into a message/class/struct, opp_msgc will not generate code for the class,
   only a descriptor object. (Corresponds to -Xnc command-line option.)

 - opp_msgc: generating descriptors can also be controlled via properties now.
   @descriptor(false) instructs opp_msgc not to generate a cClassDescriptor,
   and @descriptor(readonly) instructs it not to generate field setters into
   the cClassDescriptor. (Corresponds to -Xnd, -Xns command-line options.)

Envir:
 - When parallel simulation is enabled, host name and process ID will be
   included by default in the output file names.

 - A related fix: host name and PID are now inserted before default file
   extension instead of appending them at the end.

IDE:
 - The IDE is now based on Eclipse 3.5.1 and CDT 6.0.1.

IDE features:
 - NED editor:
    - adaptive palette: items with matching @labels properties and those
      already in the compound module are shown at the top
    - palette filtering: easily accessible text field to filter palette
      contents (case insensitive substring search)
    - when creating connections, connection chooser now offers connections
      with matching gates, using @labels properties of gates
    - new parameter editor dialog for channels and modules
    - completion proposals for connection parameters
    - visual feedback for multiple and conditional connections
    - better validation of NED files in the editor

 - Parameter grouping in the Module Hierarchy view

 - The simulation launcher now checks for errors (and warns if any) before
   launching simulations

 - Sequence Chart: attaching vectors to axes made more intuitive

 - Analysis Tool: many chart sheet and chart usability enhancements

IDE extensibility:
 - Support for easy creation of templated wizards that can contribute to
   the New OMNeT++ Project, New OMNeT++ Simulation, New Simple Module,
   New NED File, etc. dialogs. Such wizards are simple text files inside
   the templates/ folders of OMNeT++ projects. Wizards for creating wizards
   have also been provided. Templated wizards is one of the killer features
   of OMNeT++ 4.1. Read the IDE Customization Guide for further info!

 - Automatic discovery and activation of Eclipse plugins dropped into the
   plugins/ folders of OMNeT++ projects. This allows easy deployment of
   project-specific IDE extensions. Check the IDE Developers Guide for more
   information.

Tkenv:
 - Two-way connections are now rendered as a single line (without arrows)

 - Enhanced connection layouting for modules with a large width or height.

 - Right-clicking on a point covered by several objects now brings up a
   popup menu that contains a submenu for each object under the mouse.
   Previously only the topmost object was accessible, making crowded areas
   hard to inspect.

Tooling:
 - Source files are now compatible with GCC 4.4 version (fixed missing #includes)
 - Updated MinGW GCC compiler to 4.4 (on Windows)
 - Updated MSYS binaries (on Windows)

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.1b1


OMNeT++ 4.0.2 (August 2023)
---------------------------

This is a modernized version of omnetpp-4.0p1 that makes it possible to build
and use the now 14-year-old package in today's software environments. The
primary objective of this release is to facilitate the execution of old
simulation models, e.g. for reproducing simulation results or as a basis of
porting them to more recent versions of OMNeT++.

Changes:

 - Source code updated for C++11 compatibility, and to eliminate warnings
   issued by modern C++ compilers. Backported several smaller fixes.

 - Adjusted to make the code compatible with more recent versions of Tcl/Tk
   libraries. In addition to the original Tcl/Tk-8.4 and BLT-2.4z, Tkenv now
   works with Tcl/Tk-8.6 and BLT-2.5.3, too.

 - Updated to compile with both Bison 2.3 and 3.0.

 - Backported the setcontext()/switchcontext() based coroutine library from
   omnetpp-4.2, to make activity() modules work again. (The original "portable
   coroutines" library which used setjmp()/longjmp() to switch stacks no longer
   works with recent glibc versions.)

 - Updated the `setenv` and `configure` scripts, to make them more consistent with
   those in more recent or modernized releases. This includes:

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - Allow running tests in other than "debug" mode.

 - Added missing dependency to enable parallel build.


OMNeT++ 4.0p1 (Nov 2009)
------------------------

Simulation kernel:
  - Fixed compilation problems on systems with GCC 4.4.x

IDE:
  - Fixed compatibility problems with the xulrunner package on certain
    GNU/Linux systems. (Problem caused lockups on first start after
    installation)
  - Fixed compatibility problems with newer versions of GTK. (Problem
    caused buttons not to respond to mouse clicks.)


OMNeT++ 4.0 (March 2009)
-------------------------

This is a major revision of the whole simulation platform, with fundamental
changes in nearly every component. A summary of changes since version 3.4b2:

GUI:

  - An Eclipse-based comprehensive simulation IDE has been introduced to replace
    the previous standalone GUI programs gned, scalars and plove. The IDE supports
    all stages of a simulation project: developing, building, configuring and
    running simulation models, and analyzing results. It also supports
    visualizing simulation execution traces as sequence charts, and generating
    documentation. We are also bundling version control (cvs, svn, git) Eclipse
    plug-ins with the IDE. The IDE is supported on the three major platforms,
    Linux, Mac OS X and Windows. Since Eclipse is extremely extensible, we expect
    that OMNeT++-based simulation frameworks will contribute their own custom
    wizards into the IDE.

Tooling:

  - On the Windows platform, we have standardized on using the MinGW compiler.
    We are bundling a version of MSys and MinGW with the distribution, along
    with MinGW versions of several open-source programs and libraries needed
    or found useful with OMNeT++, such as gdb, perl, libxml, gmp, graphviz,
    Tcl/Tk, svn and git. MinGW was chosen over Cygwin because MinGW builds and
    uses libraries in the native Windows (MSVC-compatible) binary format, and
    builds programs that execute without a Unix emulation layer. The MSVC
    compiler is only supported in the commercial version of OMNeT++.

Build system:

  - In order to facilitate working with large simulation models like the INET
    Framework, the makefile generator opp_makemake has been extended with the
    --deep option. With --deep, opp_makemake generates a makefile that takes
    care of building a whole source directory tree.

  - Another big change is out-of-directory builds for both the OMNeT++ libraries
    and simulation models: object files and other by-products of the build
    process go in a separate directory tree (out/).

  - Simulation models can now easily be compiled with debug/release compiler
    options, simply with the commands "make MODE=debug" and "make MODE=release";
    there is no need to modify configure.user.

Simulation kernel:

  - Simulation kernel internals have been redesigned with memory efficiency in
    mind, to support large-scale simulations better. Techniques include string
    pooling (storing freqeuently occurring strings such as module, gate and
    parameter names only in one copy), shared parameter value instances,
    gate vector descriptors (gate name, type and size are only stored once
    for the whole gate vector), and optimal packing of object fields (e.g.
    packing several boolean variables into an unsigned int).

  - The simulation kernel as a library has been made significantly easier to
    embed in other (non-OMNeT++) programs. The way of using the OMNeT++
    simulation kernel as a plain C++ library has been documented in the Manual,
    and two corresponding code examples have been added to the distribution.

  - There have been several complaints about precision loss due to simtime_t
    being represented with the C type "double". In 4.0, double has been replaced
    with an int64-based fixed-point representation; the precision can be
    configured in omnetpp.ini as a power of ten, with the default being
    picosecond resolution (1e-12).

  - Regarding the API, several functions have been given better or more
    consistent names, with the most visible change being that getter methods
    have been prefixed with the word "get". Migration of simulation models
    to 4.0 is assisted by scripts that perform this renaming (and several other
    adjustments) in the source code.

  - cMessage has been split into a base cMessage plus a cPacket class.
    The bit length, encapsulated message and error flag fields have been
    moved to cPacket.

  - Modules have now the possibility to receive nonzero-duration messages
    at the beginning of the reception; this is done by reconfiguring the
    gate object.

  - sendDirect() calls now expect the propagation delay and transmission
    duration values in separate arguments; this was done to facilitate
    the animation of wireless transmissions later.

NED:

  The NED language has been revised and significantly extended. An overview
  of changes follows.

  - The language syntax has been changed to make it more consistent. A migration
    tool is provided to convert old NED files to the new syntax.

  - A Java-like package system has been introduced to make the language scale
    to large model frameworks and to prevent name clashes; NED files files are
    now read from directory trees listed on the *NED path*.

  - Channels have been made first-class citizens. They can have arbitrary
    parameters like modules do, and may have custom C++ implementation classes.
    Three predefined channel types have been created, ned.IdealChannel,
    ned.DelayChannel and ned.DatarateChannel.

  - Inheritance is now supported for module and channel types. Derived modules
    and channels may add new parameters and gates, may set or modify parameter
    values and gate vector size, and (in the case of compound modules) may add
    new submodules and connections.

  - Inner types are now supported. This is most useful for making channel
    definitions local to the network definition that uses them.

  - Module and channel interfaces have been introduced, to make "like"
    relationships more explicit. Module and channel interfaces can be used as
    a placeholder where normally a module or channel type would be used, and the
    concrete module or channel type is determined at network setup time by a
    parameter. Concrete module types have to "implement" the interface they can
    substitute.

  - Inout gates and bidirectional gates are now supported. In the C++ code,
    inout gates appear as (input,output) gate pairs.

  - Parameters can now have default values. The default value can be overridden
    in the ini file. If the ini file does not assign any value to the parameter,
    the default values get used automatically.

  - Support for arithmetic expressions has become more complete, e.g. string
    manipulation is also possible.

  - It is possible to annotate module or channel types, parameters, gates and
    submodules with properties. Metadata can carry extra information for
    various tools, the runtime environment, or even for other modules in the
    model. Examples include display strings, parameter prompt string and unit
    of measurement.

  - Default icons (in general, default display strings) are now supported.
    Display strings are represented as metadata annotation (@display property),
    which can be modified via inheritance.

Ini files:

  - Instead of runs, named configuration sections were introduced in the ini
    file.

  - The concept of "runs" have been refined to provide parameter study support;
    Ini files now can specify parameter ranges and the runtime is able to explore
    them. The notion of experiment, measurement and replication was introduced to
    help the general workflow of result analysis.

  - Configuration options are now checked; mistyped configuration options are
    reported. Configuration options can be given on the command-line as well,
    overriding the values specified in the ini file.

  - The new "fingerprint" ini file option allows for quick-and-simple regression
    tests. The fingerprint is basically a hash of event times and module IDs
    during the simulation, and it can detect if the simulation follows a
    different trajectory after a supposedly "harmless" code change.

Result analysis:

  - The new result analysis tool in the IDE (which replaces plove and scave)
    supports the notion of experiments, measurements and replication. The IDE
    now stores all operations required to create a diagram or chart in a "recipe"
    file. It can recreate the charts and diagrams automatically after re-running
    the simulation or the whole experiment.

  - The vector and scalar file format has changed to support the new features
    of the IDE. Vector files are now indexed for efficiency.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.0


OMNeT++ 4.0rc2 (Feb 2009)
-------------------------
Improvements since rc1:

Documentation:
 - Significant work went into updating the Manual. The following chapters are
   either new, or have been rewritten or significantly expanded:
     - 3 The NED Language
     - 7 Building Simulation Programs
     - 8 Configuring Simulations
     - 9 Running Simulations
     - 10 Network Graphics And Animation
     - 11 Analyzing Simulation Results
     - 15 Plug-in Extensions
     - 16 Embedding the Simulation Kernel
     - 17 Appendix: NED Reference
   Chapters that need further work are:
     - 4 Simple Modules
     - 6 The Simulation Library

 - Revised the User Guide. Text and screenshots have been updated to the
   current state of the UI.

IDE:
 - added the GIT plugin (now the IDE supports CVS, Subversion and GIT)

 - added the NED documentation generator. The generated documentation is
   under the Creative Commons license.

 - the IDE now generates GPL/LGPL/BSD copyright into newly created files;
   license may be declared by adding a @license property in package.ned

 - various bugfixes

Simulation kernel:
   There were several changes to make it easier to use the simulation kernel
   as a C++ library, to embed simulations into programs:

 - support for multiple instances of cSimulation. "simulation" is no longer
   a global variable but a macro that expands to cSimulation::getActive-
   Simulation(). Multiple cSimulation objects may coexist; before invoking
   any function in the simulation library, the correct cSimulation instance
   must be activated with cSimulation::setActiveSimulation().

 - one environment object per cSimulation instance. "ev" now maps to
   cSimulation::getActiveEnvir(), which is short for cSimulation::getActive-
   Simulation->getEnvir().

 - added cNullEnvir, a default do-nothing implementation for cEnvir, from
   which users can start subclassing their own custom environment objects

 - created cRunnableEnvir by factoring out run() method from cEnvir;
   simulation user interfaces like Cmdenv or Tkenv subclass from
   cRunnableEnvir, but other custom environment objects do not need to.

 - cConfiguration was split to a smaller cConfiguration plus a cConfigurationEx
   class that adds the rest of the methods. When the simulation kernel is
   used as library, only cConfiguration needs to be implemented.
   cConfigurationEx is needed only when one wants to plug in a different
   configuration object into the existing Envir library, replacing
   SectionBasedConfig.

 - find more details and examples in the manual, in include/ChangeLog,
   in src/sim/ChangeLog, and in the new sample simulations embedding
   and embedding2.

Samples:
 - added embedding examples

NED:
 - fix: channel types could not refer to local parameters ("this.param" caused
   an error)

 - refactored the registration of user-defined NED functions: added the
   possibility to register a documentation string (which gets printed in the
   "opp_run -h nedfunctions" output); cNEDFunction and Define_NED_Function()
   now expects the function signature in a C-like syntax.

Message files:
 - opp_msgc: improvement related to bug #33. Type *announcement* now can
   specify the base class of the declared class: "class B extends A;".
   To declare that the class does not have any base class interesting to
   opp_msgc, one must declare it as "extends void". See src/nedxml/ChangeLog
   for examples and more details.

Tkenv:
 - message display strings made consistent with module display strings: the
   "o" tag was removed, use "b=width,height,shape,color,borderwidth" instead.
   Also, a special color name "kind" has been introduced, which maps
   to a color depending on the message kind.

 - fixed bug #34: in Fast mode, bubble didn't get displayed for recently
   created modules

Cmdenv/Tkenv:
 - added -a option: lists all configurations and the number of runs in them

 - bugfixes: --repeat and --constraint command-line options were ignored
   when using -x option

 - on the command-line, accept per-object configuration options, also without
   object name pattern. Thus, both of the following are accepted, and are
   equivalent:

        opp_run --**.vector-recording=false
        opp_run --vector-recording=false

Tools:
 - added opp_runall tool

 - scavetool: renamed "summary" command to "list"; split the "filter" command
   into "vector" and "scalar" commands. The new "scalar" command can apply
   processing operations too. Currently it only supports the "scatter" operation,
   which creates a scatter plot dataset from the scalars.

 - opp_test: added a %extraargs option (was needed for new test/envir tests)

Packaging:
 - the Windows package now contains MinGW-GIT (the "git" command-line tool,
   and gitk). Please use the hyphen-less form of commands (e.g. git checkout
   instead of git-checkout).

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.0rc2


OMNeT++ 4.0rc1 (Dec 2008)
-------------------------
Improvements since beta 8:

General:
 - added the "opp" prefix to library names to prevent name clashes with
   other programs: liboppsim.so, liboppenvir.so, libopplayout.so,
   liboppcommon.so; oppsim.dll, oppenvir.dll, opplayout.dll, oppcommon.dll,
   etc.

IDE:
 - fixed the crash caused by static linking of libstdc++.so (bug #7)
 - improved stability of CDT integration
 - improved diagnostics: when simulation launch fails, Console View displays
   the command line, PATH, other environment variables, etc to help track
   down the problem.
 - several other bugfixes (see bugtracker)

Samples:
 - queueinglib changed to build a dynamic library, which is run using opp_run

 - added queueinglibext, which shows how to extend an existing model with
   C++ code from a new project

Tkenv:
 - reimplemented filtering, to fix poor performance and weird scrolling of
   log windows (bugs #16 and #17). The log is now stored in a memory buffer,
   and filtering re-fills the text widget from the buffer.

 - new feature: when you open a per-module output window, it comes up
   containing previous messages (and not empty as it used to). This
   feature was made possible by storing the log.

 - UI updates during Fast and Express Run changed from event-based
   (i.e. "every x events") to time-based ("every x milliseconds").
   Time only checked every 16 and 256 events though, respectively,
   to save CPU cycles.

 - implemented font selection for text windows and list controls

 - settings are now saved to $HOME/.tkenvrc, only inspectors are saved
   to .tkenvrc in the current directory. (Motivation: font selection is
   tyically meant to be global by the user, not per-simulation)

Cmdenv:
 - status updates in express mode are now time-based (every 2s by default),
   not event number based

 - other status update improvements: always autoflush status updates;
   print a final status update when simulation stops

Tools:
 - seedtool was renamed opp_lcg32_seedtool, and added a warning in its help
   that it is an obsolete tool and probably should not be used.

Bugs fixed: https://github.com/omnetpp/omnetpp/issues?q=is%3Aissue+is%3Aclosed+milestone%3A4.0rc1



OMNeT++ 4.0 beta 8 (Nov 2008)
-----------------------------
Improvements since beta 7:

IDE:
 - Makemake configuration made more flexible. Now any directory may contain
   a makefile, either generated or custom one. The corresponding property
   page was redesigned, and also moved to the OMNeT++ category (was under
   the C/C++ Build category). It is now also possible to export a makefile-
   creation script to support command-line builds. Note: syntax of the
   .oppbuildspec file changed (file written by 4.0b8 will not be understood
   by 4.0b7).
 - added Parameter Editor dialog as a field editor in the Property View
 - moved Cleanup NED Files command from the Navigation menu to the Source menu
 - several smaller fixes and improvements

Tkenv:
 - implemented module output window filtering: modules can be selected
   using a checkbox tree dialog. Bound to Ctrl+H.
 - implemented "Run until message", available in the Run Until dialog and
   in the context menu of message objects
 - event banner text revised, plus now includes message name and class
   as well; added "Short event banners" checkbox to options dialog
 - fixed popup menu behaviour on Linux (use tk_popup instead of post)

Other:
 - opp_makemake: generates separate target for each recursively invoked
   makefile. Several smaller fixes and improvements
 - beginnings of a regression test suite for opp_makemake: test/makemake.


OMNeT++ 4.0 beta 7 (Oct 2008)
-----------------------------

First public beta release of 4.0.


OMNeT++ 4.0 beta 1..6 (Mar 2008..Sep 2008)
------------------------------------------

Internal testing releases.


OMNeT++ 3.4b1/b2 (Nov 2006)
---------------------------

This release significantly improves Tkenv:

 - until now, Tkenv has been using various form-style inspectors to display
   the most important data members of objects. These inspectors have now been
   replaced with more comprehensive and consistent treeview-based
   inspectors.

 - broadcast animation has been implemented. When the feature is turned on,
   multiple send() or sendDirect() calls occurring within a handleMessage()
   call will be animated together, concurrently. This significantly improves
   the animation of wireless simulations. Note: the new animation mode
   implies that send/sendDirect calls are no longer animated at the
   place of the call as previously, but together at the end of processing
   the event (that is, after handleMessage() has already returned) -- that is,
   out-of-sequence with writing the log, for example. The broadcast animation
   feature can be turned off in the Simulation Options dialog.

 - implemented message filtering for the timeline, and improved on object
   filtering (Find/Inspect Objects dialog) by extending patterns with
   AND, OR, NOT operations and object field matchers.

 - implemented scrollback buffer limiting for event log windows (default is
   100,000 lines)

Simulation library:
 - several methods and classes already marked as DEPRECATED in their
   documentations and doc/API-Changes.txt are now marked as such, using
   GCC's __attribute__((__deprecated__)) facility, which results in warnings
   during compilation of code that uses them. Those pieces of code need to be
   changed in order to be compatible with future OMNeT++ versions.

 - opp_msgc was improved: as an experimental feature, class fields can now be
   annotated with properties of the form @propertyname(value). The following
   properties are accepted: field-level: @enum(enumname), @editable or
   @editable(true), @getter(methodname), @setter(methodname),
   @sizeGetter(methodname), @sizeSetter(methodname), @label(some-text),
   @hint(some-text); class-level: @customize, @omitGetVerb (or:
   @customize(true), @omitGetVerb(true)).

Other:
 - makefile improvements, from Laura Marie Feeney and Michael Tuexen.
 - the FDDI sample simulation has retired (it used lots of deprecated APIs)


OMNeT++ 3.3.2 (August 2023)
---------------------------

This is a modernized version of omnetpp-3.3p1 that makes it possible to build
and use the now 17-year-old package in today's software environments. The
primary objective of this release is to facilitate the execution of old
simulation models, e.g. for reproducing simulation results or as a basis of
porting them to more recent versions of OMNeT++.

Main changes:

 - Minor source code adjustments to enable compilation with modern C++ compilers
   and eliminate compiler warnings. Backported several bug fixes from newer versions.

 - Adjusted to make the code compatible with more recent versions of Tcl/Tk
   libraries. In addition to the original Tcl/Tk-8.4 and BLT-2.4z, graphical
   parts now works with Tcl/Tk-8.6 and BLT-2.5.3, too.

 - Backported the setcontext()/switchcontext() based coroutine library from
   omnetpp-4.2, to make activity() modules work again. (The original "portable
   coroutines" library which used setjmp()/longjmp() to switch stacks no longer
   works with recent glibc versions.)

 - Updated the `setenv` and `configure` scripts, to make them more consistent with
   those in more recent or modernized releases.

    1. `configure` now mandates having `setenv` sourced before it runs.
    2. `setenv` now sets `OMNETPP_IMAGE_PATH`, in order to prevent model
       frameworks from accidentally making omnetpp's bundled icons unaccessible
       from Tkenv by simply appending to `OMNETPP_IMAGE_PATH`.
    3. `configure` now creates `configure.user` from `configure.user.dist`
       if it does not already exist.

 - The Makefiles were updated with more dependencies to enable parallel build.


OMNeT++ 3.3p1 (Oct 2006)
----------------------

Source code adjustments for improved compiler and library compatibility.


OMNeT++ 3.3 (Oct 2006)
----------------------

This release contains several GUI enhancements in Tkenv, and several bugfixes.
Tkenv:

 - display string enhancement: connections with zero thickness set ("o=,0")
   now do not appear in the network graphics.

 - added object icons to inspector listboxes

 - added Run/Fast/Express buttons to every module inspector window, as well as
   a "Find/inspect messages, queues, etc within this module" button. These
   functions are all available via hotkeys too (F5,F6,F7,Ctrl+S).

 - added "Copy to clipboard" and "Save window contents" toolbar icons to all
   text windows. (Copy has already been available with Ctrl+C.)

 - main window position gets saved and restored

Bugfixes:
 - fix: gamma_d() was bogus for the alpha<1 case [reported by Patrick McDonnell]

 - fix: worked around a fiber-related Visual C++ problem that caused certain
   activity()-based modules crash when the simulation was restarted.

 - fixed a bug which sometimes caused Tkenv to crash when inspecting an
   internally refcounted message.

 - fixed a bug in Tkenv's setObjectListResult() [crash reported by Rodolfo
   Ribeiro Gomes and others]

 - fix: on some Linux systems, Tkenv used unreadably small font size in some
   dialogs

 - opp_msgc improvements: don't accept reserved words as identifiers; added
   OMNeT++ version check into generated code

 - fix: the simulation can now be in a directory whose path name contains spaces
   (e.g. "Program Files"); opp_nmakemake also fixed to accept include paths
   with spaces.

 - many other minor fixes and improvements; see the ChangeLogs of the
   corresponding source directories for more details.


OMNeT++ 3.2p1 (Jan 2006)
------------------------

This is a maintenance release. Changes include:

 - support for VC8.0 (Microsoft Visual C++ 2005 [Express]).
 - opp_nmakemake -b <dir> failed for directory names containing `++`
 - fixed bug in Plove/Scalars "Save picture" function


OMNeT++ 3.2 (Oct 2005)
----------------------

The following brief list wraps up the most important new features and
changes since the 3.1 release. Please read on to the sections about the
the prereleases (3.2pre1..4) for more details on these points, and for
a more complete list. Even more details can be found in the doc/API-Changes
file and the ChangeLog files of the individual source directories.

General:
 - support for database (MySQL) storage of: model topology, model
   configuration, output vectors and scalars and model topology.

Simulation core:
 - simplified the signature of simple module constructors; Module_Class_Mem-
   bers() is no longer required. Please check below or in doc/API-changes
   for backward compatibility information.

 - cleanup-time garbage collection turned OFF by default. It can be turned
   on manually, but the preferred way is to add proper destructors to
   simple modules.

 - implemented reference counting of encapsulated messages, primarily to
   increase the performance of wireless simulations.

 - WATCH() revisited: now its variants support STL container classes, and
   basically any data type that has operator<<.

 - modules can now get notified when a module parameter changes. Just redefine
   the new handleParameterChange(const char *parname) method of cModule.

 - several other API improvements -- see doc/API-Changes.

 - several opp_msgc improvements

GUI:
 - added the TimeBar to Tkenv, and made the "Find/Inspect Objects" dialog
   nonmodal (a long-standing wish).

 - several GNED refinements, the most visible one being that on opening a NED
   file it automatically tries to open all its imports as well (feature can
   be turned off).

 - big usability improvements in Plove and Scalars: Chart Settings dialog
   has been redesigned, and your custom settings can be saved as defaults.
   Plove also has new filters, and some problems got fixed in existing ones.


OMNeT++ 3.2pre4 (Sep 2005)
--------------------------

Simulation kernel:
 - cleanup-time garbage collection turned OFF by default. Simple modules
   have to deallocate all dynamically objects themselves, that is,
   proper destructors have to be added to every simple module class.
   Related new omnetpp.ini entries: perform-gc=<yes/no>, print-undisposed=
   <yes/no>.

 - added method cSimpleModule::cancelAndDelete(cMessage *msg) method, to
   facilitate writing destructors.

 - requirement for simple module constructors changed: they are invoked
   without args now. Module_Class_Members() is now unnecessary, but still
   accepted. See include/ChangeLog for details. All sample simulations
   have been updated accordingly.

   POTENTIAL INCOMPATIBILITY: modules with handcoded constructors need
   to be modified. Add default values to constructor arguments:
   (const char *name=NULL, cModule *parent=NULL).

 - dup() method got moved to cPolymorphic, and return type changed to
   cPolymorphic. In the next release we'll drop MSVC 6.0 support,
   and move to covariant return types (which MSVC 6.0 doesn't support).

New examples:
 - database integration. With the given extensions, one can transparently
   redirect output vectors and output scalars into a MySQL database, read
   module parameters from a MySQL database (all without modification of
   existing models), or build a network from topology stored in a database.

Other:
 - NED: enabled parent module gates to get connected with `++`.
   `parentmodgate++` connects the first unconnected gate, but it doesn't
   create new ones -- it is an error to exceed `sizeof(gate)`.

 - Precision in output vector/scalar files, that is, the number of
   significant digits printed can be set from the configuration now:
   `[General]` / `output-scalar-precision=<int>`, `output-vector-precision=<int>`.
   The default is 12, but it might not be straightforward to decide what
   is the "optimal" setting -- see src/envir/ChangeLog for a discussion.

 - configure script revised significantly: better detection of compiler
   (e.g. the Intel compiler is autodetected now); detection of linker
   flags needed for OMNeT++ models to be put into static/dynamic libs
   (-z allextract/--whole-archive, --export-dynamic); detection of "make"
   program (must be GNU make); improvement in detecting BLT and MPI.
   Now it also allows variables to be defined on the command line before
   running ./configure, as an alternative to configure.user.

 - nedtool and the preload-ned-files= omnetpp.ini entry now distinguishes
   between two types of listfiles, @listfile and @@listfile; the difference
   is in the interpretation of relative paths contained in the list file;
   see src/nedc/ChangeLog for details.

 - GNED: further improvements


OMNeT++ 3.2pre3 (Aug 2005)
--------------------------

Tkenv:
 - the "Find/Inspect Objects" dialog became nonmodal, and it can be kept
   open while the simulation is running.

 - added the "Run until next local event" command to modules' context menu
   (which pops up when you right-click an icon)

GNED:
 - reorganized and improved connection properties dialog (knows what gates
   submodules have, which gates are vectors, only offers output gates as src
   gate and input gates as dest gate, etc.)

opp_msgc:
 - "const fix": generate both const and non-const getter methods for fields
   of class or struct type.

 - it is no longer required to write out "fields:" in an otherwise empty
   class definition

 - added the possibility to set a base class field. E.g. kind=5 will
   generate the setKind(5) line into the constructor.

 - generate a protected and unimplemented operator==(), to prevent
   accidental usage of compiler-generated one

 - added the possibility (-P option) to declare generated classes to be
   dllexport/dllimport on Windows.

 - generated C++ header now contains relevant part of the .msg files as
   Doxygen comment

nedtool, dynamic NED loading:
 - don't look for the module type if the module vector size is zero

opp_nmakemake:
 - added options (-s, -t, -d) to enable building Windows DLLs from simple
   modules, plus -P option to support opp_msgc's similar option

Simulation kernel:
 - implemented reference counting of encapsulated messages. This should
   dramatically increase performance of wireless simulations, where lots
   of message duplication is done (AirFrames) with most copies discarded
   without looking into them (noisy channel or wrong L2 dest address).
   There' nominal (<1%) performance penalty for other simulations.

omnetpp.ini:
 - one can now use parameters like $MODULE_FULLPATH in the 2nd argument
   of xmldoc(), i.e. the paths expression to select part of an XML file
   to load. This allows for assigning XML params of several modules with a
   single line in omnetpp.ini. Consider the following:

       **.xmlConfig=xmldoc("all-in-one.xml","config[@mod=$MODULE_FULLPATH]")

   where all-in-one.xml contains `<config mod="net.host1.rt">...</config>`
   elements for every module.

 - also fixed some anomalies in the XML path expression interpreter.


OMNeT++ 3.2pre2
---------------

Simulation kernel:

 - WATCH() overhaul: now anything can be watched that has operator<<. If it
   also has operator>>, you can use WATCH_RW() to make it editable on the GUI.
   Objects and pointers to objects can be watched with WATCH_OBJ() and
   WATCH_PTR(). STL container classes can be watched via WATCH_VECTOR(),
   WATCH_PTRVECTOR(), WATCH_MAP(), WATCH_PTRMAP(), etc.

 - modules can now get notified when a module parameter changes. Just redefine
   the new handleParameterChange(const char *parname) method of cModule.

Tkenv:
 - flush omnetpp.vec every time the simulation is stopped (to enable looking
   at it with Plove to decide if simulation needs to be run longer)

 - added support for OPP_PLUGIN_PATH environment variable, and
   `[Tkenv]/plugin-path=` omnetpp.ini entry.

 - cGate inspector: channel attributes (delay, error and datarate)
   can now be changed from the GUI.

 - added support for custom items on the context menu (e.g. right-click
   on a router, "Inspect Routing Table..." Usage: place a .tcl file into
   the plugin path (./plugins by default) that calls the extendContextMenu
   Tcl procedure.

GNED: Michael Franzen contributed several improvements and fixes:
 - new option: "Open imported files automatically": tries to open all the
   files that are imported by an opened NED file. (It takes care that
   every NED file is opened only once.)

 - an "import path" can be specified, which is a list of directories
   where imported files are searched for (Andras)

 - new option: "Autoextend gates using gate++ notation". When this option
   is enabled (Options Menu), GNED does not generate "gatesizes" sections,
   and automatically uses the extension operator "++" on all gates that
   are connected. However, you always can use indices in the property page
   of modules to override automatic gate extension.

 - "Auto-calculate gate size": when switched on, this option generates
   "gatesize" entries for every gate of a module that is endpoint for
   indexed connections. The code searches for the maximum index of all
   connections ending at that port and generates the "gatesize" entry
   from that. If no indexed connection to the port is found, the
   "gatesize" entry is removed again.

 - "Set 'nocheck' for new modules": generates "connections nocheck" for
   all newly generated modules. This feature has a counterpart in the
   property page of modules: an "Allow unconnected gates" checkbox
   to enable/disable this feature for each module individually.

 - plus several smaller fixes

Heaps of small API improvements, see include/ChangeLog. A sample:
  - cOutvector: added recordWithTimestamp()
  - cDisplayString: setTagArg() now accepts numeric (long) arg too.
  - cMessage: if "vector" is a vector gate, msg->arrivedOn("vector")
    now checks if msg arrived one of the vector's gates.
  - cXMLElement: added getFirstChildWithTagName() and getNextSiblingBy-
    TagName()

Thanks to Michael Tuexen, Ignacio Arenas Jimenez and Wei Yang Ng who suggested
several of the above improvements.


OMNeT++ 3.2pre1
----------------
  - Plove and Scalars: improved Chart Settings dialog, and default settings
    can now be saved. New Plove filters, and fix in the winavg filter.
  - Tkenv: added Timeline Status Bar
  - Tkenv: fixed the r= display string tag (it displayed a circle half the
    specified size)
  - Tkenv: cMessage inspector now displays message size in bytes as well (not
    just in bits); also, it now displays time *until* arrival time (a delta)
    in addition to the absolute arrival time.
  - new cMessage member functions: byteLength(), setByteLength(), addByte-
    Length(). They just invoke the length(), setLength(), addLength() methods.
  - new utility function simtimeToStrShort()
  - fixed gcc-4 compilation errors, and removed -fwritable-strings compiler
    option
  - fixes and improvements on opp_neddoc
  - configure script now accepts NO_TCL=1 variable (for emergency use only)
  - opp_nmakemake: improved output of recursive build
  - Readme.MSVC updated on how to build DLLs from simple modules
Plus a number of smaller fixes, see the ChangeLogs in individual directories.
Documentation update is still TBD.


OMNeT++ 3.1 (Mar 2005)
----------------------
This release contains several bugfixes since 3.0, as well as new features
and new example simulations. To see a detailed list of problems fixed, check
the ChangeLog files in the various source directories. Thanks all of you who
have discovered and reported the problems.

A brief list of enhancements since version 3.0:

Real-time/hybrid simulation:
- Real-time simulation has been implemented, and it is demonstrated by the
  "Dyna/Real Time" sample simulation.

- An example for hybrid (hardware-in-the-loop) simulation is now provided,
  which is the "Sockets" sample simulation. It uses TCP sockets to communicate
  with the outside world -- in real-life applications this will probably need
  to be replaced with some other means of communication.

- In relation to the above: new methods got added to the cScheduler interface.

For Windows:
- Debug info (*.pdb) files have been included in the installer, so one can now
  single-step into the simulation kernel and user interface library sources
  while debugging.

Simulation library:
- New utility class: cStringTokenizer. It splits strings to "tokens" along
  given separator characters (by default spaces).

- Changed the way scheduled events or pending messages are handled when their
  destination module gets deleted during runtime. Before, these messages were
  deleted by the cSimpleModule destructor, but this made it very long to clean
  up large models. Now, messages are not deleted in the destructor, but left
  in the FES -- they will be discarded in cSimulation::selectNextModule() when
  they're met.

- cDisplayString's getTagArg() method was modified to never return NULL; it
  returns "" instead when the requested tag is not in the display string.

Configuration:
- Ini file runs can be given descriptions, and they will be displayed in the
  Tkenv run selection dialog. Just add the description="some text" line
  under the [Run x] headings.

- New omnetpp.ini entry to help debugging: [General]/debug-on-errors=
  true/false. If turned on, runtime errors (such as scheduling a message
  into the past) will cause the program to break into the C++ debugger
  if one is running, or just-in-time debugging is enabled. This makes it
  possible to investigate the context of the error (stack frames, variables
  etc). It works by doing INT 3 (debugger interrupt) on Windows, and
  raise(SIGABRT) on Linux.

- Slight change in the rules of handling included ini files, NED list files
  and all filenames mentioned in them. All filenames are now understood as
  relative to the location of the file which contains the reference,
  rather than relative to the current working directory of the simulation.
  This change is not backwards compatible, but has been found not to affect
  any of the simulation frameworks already ported to 3.0.

Tkenv:
- Implemented the "t=<string>,<color>" display string tag for connections.
  It makes the given string to appear near the connection arrow.

- Tooltips can now be added to submodule icons, connection arrows and messages.
  The tooltip text has to be given in the "tt=<tooltip-string>" display
  string tag, and will be displayed if the mouse rests over the component for
  a while. Tooltips allows more info to be displayed than can be squeezed
  into the "t=" display string tag.

- Added support for -r <run-number> command-line option, which has the same
  effect as (but takes priority over) the [Tkenv]/default-run ini file entry.

Misc:
- opp_makemake, opp_nmakemake: implemented -X option (ignore subdirectory
  during recursive build)

- The -h command-line option is now understood by both Cmdenv and Tkenv,
  and handled in a consistent way

- The place/ icon directory was renamed to misc/ (Please update display
  strings containing "i=place/..")

- nedc got removed from the distribution

Known problem: OMNeT++ windows have no Maximize button under recent
KDE versions. This is a known Tk+kwin problem which was fixed in Tk
early 2004.



OMNeT++ 3.0 (Dec 2004)
----------------------

The following list summarizes *all* changes since OMNeT++ 2.3:

 - The simulation kernel has received an overhaul.

 - A new step-by-step tutorial, revised documentation and several new
   sample simulations have been included to shorten your learning curve.

 - Dynamic NED loading will cut model build time and simplify your work.

 - Documentation generation tool will document your own large simulation
   models, and help you quickly get an overview about the structure
   of 3rd party models.

 - The new Random Number Architecture features a modern, very long cycle
   Random Number Generator (Mersenne Twister) for more realistic simulation
   results, and RNG mapping for more flexibility in simulation experiments.

 - Simulation models can now conveniently access XML configuration files
   for complex input.

 - Plove now features a powerful interactive charting component, and also
   an internal data-flow engine for processing output vectors before
   plotting.

 - The new Scalars tool complements Plove, and lets you analyze and plot
   scalar data produced by simulations.

 - Support for Parallel Distributed Simulation

 - Several Simulation GUI usability enhancements will save you time, and
   make your work more efficient.

 - New icons, and enhanced animation capabilities (icon coloring, queue
   length display, status text, transmission range, transient bubbles,
   etc.) improve the overall look and feel of your models, give you more
   insight, and make the models instantly presentable.

 - For better visualization of model dynamics, OMNeT++ now supports
   animation of direct message sending and C++ method calls across modules.

 - The automatic network layouting using the SpringEmbedder algorithm will
   let you work with large networks in the GUI, without the need for
   individual positioning of nodes.

 - Ready to receive your extensions: plug-in interfaces for new random
   number generators, new configuration databases and simulation event
   schedulers complement existing plug-in interfaces for recording simulation
   data, and let you implement custom input/output, real-time simulation,
   hardware-in-the-loop simulation and more.

 Delta from 3.0 beta 1:

 - User Manual revised

 - Several bugfixes and new test cases, significant performance improvements
   in the network setup code; more intuitive error messages at places.

 - The new <parametername>.use-default=true notation in omnetpp.ini makes it
   possible to apply the NED file defaults (specified via input()) to some
   or all unassigned module parameters.

 - histogram classes (cLongHistogram, cDoubleHistogram) revisited


OMNeT++ 3.0 beta 1 (Nov 24 2004)
--------------------------------

Changes:
 - added new TicToc tutorial
 - nedtool: implemented -h option (put output files into current dir, not source
   file's dir) [patch from Johnny Lai]
 - Windows binary release contains 4 sets of libs now: vc6-debug, vc6-release,
   vc7-debug, vc7-release.
 - several fixes and improvements


OMNeT++ 3.0 alpha 9 (Nov 3 2004)
--------------------------------

Changes:
 - fixed configure script problems in alpha 8

 - new random number architecture: support for Mersenne Twister and Akaroa
   RNGs; unlimited number of RNGs; RNG mapping (module-local RNG numbers
   can be mapped arbitrarily to physical RNGs from omnetpp.ini. Details
   in src/envir/ChangeLog and on omnetpp.org.

 - Tkenv fixes and usability improvements: (1) allow user interactions during
   message animation: animation speed can be changed with the slider, STOP
   button can be clicked, inspectors can be closed, etc;  (2) improved
   "Fast Run until next local event" function: now it always animates events
   that occur in the local module;  (3) fixed display string handling bugs: in
   "t=", commas and equals signs weren't handled properly.

 - signatures of fullPath() and info() functions changed to use std::string.
   The new signatures are both safer (no danger of accidental buffer overrun
   or confusion caused by using static buffers) and easier to use.
   POTENTIAL INCOMPATIBILITY: fullPath() calls in sprintf(), ev.printf(),
   new cException() etc calls should be changed to fullPath().c_str().
   (ev<< statements are not affected.)

 - support for categorized icons (icons grouped into subdirectories with
   the bitmaps folder) in GNED and Tkenv

 - linking problems (linker symbol conflicts) with recent Akaroa versions
   resolved


OMNeT++ 3.0 alpha 8 (Oct 2004)
--------------------------------

Changes:
 - in .msg files, struct fields can have initializers (a constructor gets
   generated)
 - Tkenv: minor improvements
 - configure script refinements


OMNeT++ 3.0 alpha 7 (Sept 2004)
-------------------------------

Several improvements and bug fixes since 3.0a6. Detailed list of changes:

 - loading XML config files via "xml"-type NED parameters or
   ev.getXMLDocument(): If the document has an attached DTD (DOCTYPE
   declaration), the document will be validated, and attribute default values
   will be completed from the DTD.

 - Scalars tool: enhanced Copy to clipboard functionality: different
   variables can be put on different columns. This makes it easier to use
   Excel's PivotTable or OpenOffice's DataPilot) functionality to analyze
   data.

 - NED: gate vectors, if there's no "gatesizes:", will be created with zero
   size. (Previously they were created with size one -- this is a potential
   incompatibility.)  A zero-sized gate vector is internally represented by a
   single gate object whose size() method returns zero, and cannot be
   connected.

 - Tkenv: bitmap path can now be specified in omnetpp.ini, using the
   [Tkenv]/bitmap-path= entry. Value should be "quoted", and directories
   should be separated by ";". The contents of the OMNETPP_BITMAP_PATH
   environment variable (or if that's not set, the compilation-time bitmap
   path) gets concatenated to this one.

   IMPORTANT: if you forget the quotes, ONLY THE FIRST DIRECTORY will be
   used with the rest ignored, because ";" is the comment mark in omnetpp.ini!

 - opp_makemake, opp_nmakemake: facilitated creating makefiles which only
   have relative paths in them. The -b <basedir> option was added. <basedir>
   is supposed to be the toplevel directory of large multi-directory
   simulation model (such as IPSuite or MF). The -b option will cause all path
   names (passed with -I, -L, etc.) to be converted from absolute to relative.
   This means that makefiles will not need to be re-generated or edited when
   the project is compiled in a different directory.

 - opp_neddoc: backslashing can be used to prevent intentional hyperlinking
   of a word which is also a component (e.g. module) name. That is, if you
   write "\IP", "IP" won't get hyper-linked in the HTML output, even if
   there's a module type called IP. Single backslashes will be removed from
   the HTML output -- double them if you want them to appear.

 - opp_neddoc: on Windows, listing directories on the command line which had
   no .ned or .msg files in them caused opp_neddoc to stop with "file not
   found" error.


OMNeT++ 3.0 alpha 6 (Aug 2004)
------------------------------

POTENTIAL INCOMPATIBILITY: because the resolution of "*" has changed (it no
longer matches "."), you may need to revise your existing omnetpp.ini files.

New features introduced in this release, in nutshell: enhanced wildcard
resolution in omnetpp.ini; direct support for XML config files via the "xml"
NED parameter type; Cmdenv now lets you choose which modules' ev output you
want to print; plugin interface for using configuration data sources other
than omnetpp.ini. Several improvements in opp_neddoc and opp_makemake, and
some bugfixes.

For more details, see article
  http://www.omnetpp.org/article.php?story=20040722133521209


OMNeT++ 3.0 alpha 5 (June 2004)
-------------------------------

Plove has been completely rewritten under the skin. It is no longer a wrapper
around grep, awk and gnuplot -- it boasts a powerful internal extensible
data-flow engine (for filtering and processing output vectors before plotting),
and a BLT-based plotting window (which allows a multiple plots via tabbed
window, and these plots are a bit easier to customize than gnuplot-based ones
were -- oh yes and they allow arbitrary zooming on the fly). The new Plove
can also do scatter plot. Calculating histograms and cumulative distribution
functions and various other refinements will come soon.

What else? opp_msgc now generates the pack/unpack operations so parallel
simulation has become a lot more transparent. Plus a couple of bugfixes of
course. The test suite (test/core) is supposed to pass completely, except
for one little known bug in ev<< w/ Cmdenv.


OMNeT++ 3.0 alpha 4 (May 2004)
------------------------------

This alpha release introduces a new tool, Scalars for analysing output scalar
(.sca) files. Scalars requires BLT. Another novelty is the new "control info"
field in cMessage which provides a better way to implement communication
between protocol layers than the traditional "interface packet" approach
used in older IPSuite versions. (IPSuite has also been switched over to employ
the control info approach.)


OMNeT++ 3.0 alpha 3 (March 2004)
--------------------------------
Bugfixes.


OMNeT++ 3.0 alpha 2 (March 2004)
--------------------------------
Improvements on opp_(n)makemake, Plove, Tkenv, parallel simulation; bugfixes.


OMNeT++ 3.0 alpha 1 (Jan 2004)
------------------------------
Sames as 3.0 Preview


OMNeT++ 3.0 Preview, 2004 January
---------------------------------

This is an intermediate code snapshot -- it was released to show you what
features are planned for release 3.0, and possibly to get early feedback
about them. It has not been fully tested.

The model documentation tool opp_neddoc has been introduced. It generates
fully hyperlinked documentation with diagrams from comments in .ned and .msg
files. It can also link to Doxygen-generated C++ documentation. It works both
on Unix and Windows.

Several Tkenv network animation enhancements: next event marker; animation
of sendDirect() and method calls between modules; message icons; auto-layouting
of networks; icon coloring; module "bubbles" (callouts); display of queue
length next to module icon; display of a status string above module icon;
optional modifier icon on top of submodule icon; submodule icon optionally on
top of a rect/oval; display of "transmission range"; color names in display
strings are accepted in HSB (hue-saturation-brightness) too.

Changing display strings was made more comfortable from simple modules.

Other Tkenv enhancements: powerful object search dialog; enhanced plugin
support (see test/tkenv/plugin); better tabnotebook and multi-column listbox
widgets via BLT. All Tkenv settings restored from .tkenvrc now.

New sample simulations: Tictoc (a step-by-step tutorial), Aloha, CQN (Closed
Queueing Network), Queueing, Routing, RoutingDB, Point-to-Point Transmitter.
Other changes are that Dyna and Dyna2, Fifo and Fifo2 was merged, Topo was
renamed to NEDDemo, Token renamed to TokenRing. The Nim example has finally
retired. The new/revised sample simulations demonstrate new Tkenv features
and also more simulation kernel features. They also mark a shift from
activity() based modules towards handleMessage()-based ones.

Dynamic loading of NED files has been implemented, which means it is possible
to use NED files directly, without nedc and C++ compilation. NED files
containing compound modules and networks can be loaded dynamically.
NED files can be loaded interactively (Tkenv "Load NED file" toolbar icon)
or automatically (via omnetpp.ini, preload-ned-files= entry). (alpha!)

Simulation kernel: internal object ownership handling, has been redesigned,
which both reduced memory footprint (sizeof(cObject)) and increased speed.
Another efficiency improvement is that for short object name strings,
the string is now stored inside cObject (and not dynamically allocated via
new char[...].)

Distributed simulation support rewritten, currently experimental. (alpha!)

Several new tests in test suite, also for testing opp_neddoc and Tkenv.

TODO:
The Manual is still to be updated at places. Further items are the
introduction of new RNG architecture based on modern RNGs, and testing
and completion of nedxml/nedtool (which will bring the retirement of nedc).


OMNeT++ 2.3 release (June 2003)
-------------------------------

A major revision of the User Manual has taken place. In addition to
documenting new features, several existing sections have been revised,
updated and expanded for clarity and informativeness, based on feedback
from the community.

Even if you already have a printed copy of an earlier manual, this is
a good time to discard it and print a new one -- even experienced OMNeT++
users will find a wealth of new information in it. For the list of
changes, see the "Document History" table at the front of the manual.

Deprecations! To ensure your simulation will be compatible with future
releases, please check doc/API-Changes.txt and remove use of
deprecated functions from you simulation models. Functions deprecated
now are likely to be removed in next major release.

Improvements:
- message subclassing: generated message classes now accept message kind
  in the constructor.

Bugfixes since 2.3b2:
- fixed problem with deleting dynamically created modules
- fixed opp_msgc problem with RedHat9's broken Perl (doesn't recognize [^\s]
  in regexps)
- minor improvements: opp_nmakemake now autodetects C++ file extension
  (.cc or .cpp); opp_msgc doesn't choke on -I flag


OMNeT++ 2.3b2 release (March 2003)
----------------------------------

This is a bugfix release. Resolved issues include:
- wrong handling of channels (bug was introduced in 2.3b1)
- opp_test failed on Unix if "." was not in the PATH
- various smaller bugs in GNED, Tkenv, Envir, and opp_nmakemake
- added several test cases (e.g. for channel testing) in test/

See doc/API-Changes.txt for the list of changes since 2.2, including those
that affect compatibility. ChangeLogs in respective source directories
contain even more details.


OMNeT++ 2.3b1 release (February 2003)
-------------------------------------

Licensing change: The license used in previous versions has been replaced
with a new "Academic Public License" (doc/License). This license gives
noncommercial users rights to use, modify and distribute copies of OMNeT++.
The possibility of using OMNeT++ under the GPL has been removed. Commercial
users of OMNeT++ should obtain a commercial license.

Major changes of this release in nutshell:
- The simulation kernel has been ported to use C++ exceptions and RTTI
  (Run-time Type Information) and on Windows, the Fiber API. Also, there
  has been extensive internal refactoring and refinement of the APIs.

- The "message subclassing" feature has been officially introduced. It is now
  documented in the manual, and used by several sample simulations.

- Added the implementation of several distributions: continuous distributions
  gamma, beta, Erlang, chi^2, student-t, Cauchy, triangular, lognormal, Weibull
  and Pareto; discrete distributions Bernoulli, binomial, geometric, negbino-
  mial and Poisson [contributed by Werner Sandmann and Kay Michael Masslow].

- Contains an alpha version of the new NED compiler architecture,
  built upon XML foundations. This includes a tool for generating
  documentation from NED files.

- Akaroa support. Akaroa does MRIP (multiple replications in parallel).

- Tkenv GUI has been polished -- both functionality and usability has been
  greatly improved. Plove GUI was also redesigned.

For the list of changes that affect portability of simulation models,
see the new doc/API-Changes.txt file. You can find even more detailed
information in the ChangeLogs of the respective source directories.

A more detailed list of changes follows:

Simulation kernel refactoring:
- On Windows, now Win32 native coroutines are used (Fibers API). This change
  made it possible to introduce the use of C++ exceptions.

- Error handling in simulation kernel (and in other libs too) has been ported
  to C++ exceptions. This not only resulted in cleaner kernel code, but also
  has benefits for you:
  * you can now use try-catch for better error handling. The API documentation
    describes which methods throw exceptions. The common exception class
    is cException.
  * methods like end(), endSimulation(), deleteModule() terminate the module
    immediately even when called from handleMessage(). In earlier versions,
    handleMessage() had to go through in any case.
  * you can now safely use C++ libraries that throw exceptions, e.g. STL.
  Methods like cSimpleModule::error() and opp_error() are remained for backward
  compatibility, but they simply throw cException. opp_warning() remained but
  its use is discouraged.

- Use of C++ RTTI (Run-time Type Information). From now on, you can use
  dynamic_cast<> in your code, and you don't need to provide a className()
  method in your classes (because the kernel can figure it out using typeid
  in the base class, cObject).

- Eliminated gcc 3.0.x warnings, and changed to the I/O headers <iostream>, etc.

- Several smaller auxiliary classes have been turned into inner classes of
  their respective 'main' classes. (For example, cQueueIterator became
  cQueue::Iterator, sTopoNode became cTopology::Node, etc; full list in
  include/ChangeLog.) Compatibility typedefs exist for the old names.

- Cleanup in cObject and cSimulation: several internally used methods have been
  eliminated or moved to better places. (For example, inspector-related methods
  cObject::inspector() and cObject::inspectorFactoryName() have been removed,
  and inspectors are now fully internal matter of Tkenv.)

- Refactoring on dynamic module creation: instead of modtype->buildInside(mod),
  one should now write mod->buildInside(), which is more natural.
  (The former syntax still works -- a cModuleType::buildInside() is still
  provided for backward compatibility). buildInside() delegates task to
  doBuildInside() (a new method), so it's doBuildInside() which should be
  redefined in subclasses, and also nedc generates now doBuildInside() code.

- Container classes are now more consistent in what they do when the requested
  object doesn't exist:
  * Convention 1: methods returning an object reference (e.g. cPar&) now
    always throw an exception if the object is not found. (Until now, some
    methods issued a warning and returned null reference, i.e. *(cPar*)NULL).
  * Convention 2: methods returning a pointer (e.g. cGate*) return NULL
    pointer if the object is not found. (Until now, some methods issued
    a warning and returned NULL pointer.)
  These conventions necessitated the following changes:
  * par() of cModule and cMessage now throws an exception if parameter was not
    found. (In simple module code, check for existence of a parameter before
    accessing it can now be replaced with try-catch.)
  * cModule: cGate *gate() methods no longer issue a warning if the gate is
    not found, just return NULL.
  * cArray::remove(cObject *) and cQueue::remove(cObject *) methods
    now return NULL instead of the original pointer if the object was
    not found.

- cPar can now use compiled expressions subclassed from cDoubleExpression (a
  new class). This is needed for code generated by the new nedtool.

- cSimulation changes:
  * lastModuleIndex() renamed to lastModuleId();
  * operator[] deprecated (use module(int) method instead)
  * del() renamed to deleteModule(); add() renamed to addModule()

- Channels are now represented by real objects (subclassed from cChannel)
  rather than parameters in cGate. Channels can be assigned to connections
  via cGate's setChannel() method; at the same time, DEPRECATED cGate's
  setDelay(), setError(), setDatarate(), setLink() methods (these attributes
  should be set on the channel object).

- cSubModIterator: operator[] deprecated (use operator()(int) method instead)

- For further, more internal changes and for more details on the above changes
  see include/ChangeLog and src/sim/ChangeLog.

Simulation kernel new features:
- Added convenience functions to cGate, cMessage, and cArray (see
  API-Changes.txt for details).

- A new utility class cDisplayStringParser lets you easily manipulate display
  strings.

Message subclassing, new NED compiler architecture:
- The Perl-based message compiler opp_msgc is now available in bin/. Message
  subclassing is documented in the manual, and it is demonstrated by several
  sample simulations. 40+ test cases deal with testing the generated code.

- Added nedtool, the new NED and message compiler (alpha). nedtool converts
  NED from/to XML, providing much better integration possibilities (for example
  it is now possible to use XML/XSLT to generate NED topologies from external
  data sources -- or to extract NED info to arbitrary XML format.)

- Added nedxml library (the library under nedtool) to src/, and its API-doc
  to doc/nedxml-api.

Manual:
- Manual source has been converted from Word to Latex [thanks to Ulrich Kaage
  for this non-trivial and time consuming task!]. He also added an Index.
  HTML conversion (using L2H, by myself) is not 100% yet, you should regard
  the PDF as the authoritative form.

- New chapter about message subclassing, plus various updates, fixes and
  clarifications [thanks to Phillip Feldman for his comments]

Build:
- In addition to Cygwin, the MinGW compiler is now also supported in Windows.
  Older Cygwin versions with the 2.95.x compiler cannot be used any more.

- Improved MSVC support: opp_nmakemake creates MSVC makefiles; running the
  test suite now works smoothly with MSVC; opp_stacktool displays reserved
  stack size for executables.

- MSVC projects now need *different* compiler flags. Every source should be
  compiled with exceptions enabled and using /GR, otherwise there will be RTTI
  exceptions. Because of the Fiber API, reserved stack size should be set to
  a low value, 64K.

- New utilities (Unix, MSVC): opp_neddoc creates HTML documentation for NED
  files; opp_neddocall creates HTML documentation for all nedfiles in a
  directory tree; splitvec to split output vector files.

- There are new targets in Makefiles that are generated by opp_makemake:
  Target neddoc.html generates a file named neddoc.html by using the
  opp_neddoc tool on all NED files. Target htmldocs generates HTML
  documentation of all source files (*.h and *.cc) using Doxygen. The
  HTML files are placed into the sub-directory ./htmldoc. Also, Makefile
  now generates ../doc/api/tags. This tag file can be included when generating
  model documentation.

- Improved configure script. Now we support Tk8.4, and also optional packages
  like Akaroa.

- Test suite: the number of test cases has grown from 19 to about 120 since
  omnetpp-2.2; now it covers a bigger part of the core functionality (modules,
  messages, queues, parameter) as well as message subclassing and some aspects
  of NED and Cmdenv. Tests for the distributions are in test/distr.

GNED, Plove:
- The Plove GUI has been redesigned, with focus on intuitiveness and usability.
  It also features icon toolbars, tooltips and more self-explanatory dialogs.

- The number of module icons available for simulations has grown from 41 to ~90
  since the 2.2 release. GNED also has an improved icon selector dialog now.

- There have been other small fixes and improvements in GNED, e.g. fixed the
  bug which caused display strings always being added to connections.

- All Tk GUIs now make use of the combobox widget from Bryan Oakley.

Envir changes (apply both to Cmdenv and Tkenv):
- Integrated Steffen Sroka's Akaroa extension that supports multiple
  independent replicated simulation runs.
  Akaroa can be activated by specifying
      [General]
      outputvectormanager-class=AkOutputVectorManager
  in the ini file. By default, all output vectors are under Akaroa
  control; the <modulename>.<vectorname>.akaroa=false setting can be used
  to make Akaroa ignore specific vectors. For more info see the Akaroa
  home page and the OMNeT++ documentation.

- Added fname-append-host=yes/no ini file setting. When turned on, appends
  the host name to the names of the output
  files (output vector, output scalar, snapshot files). This is
  useful when the simulation is run on multiple machines concurrently
  (parallel independent runs (Akaroa) or parallel/distributed simulation),
  and all running instances write to the same directory (e.g. shared
  via NFS mount). [Idea from Steffen Sroka's Akaroa extension]

- 2 bugfixes: (1) output scalar file did not get opened sometimes; (2) output
  scalar file got removed between runs

Cmdenv:
- Cmdenv can now display various performance measures like event/sec,
  event/simsec, simsec/sev, number of message objects, etc. during execution,
  so you can compare Tkenv and Cmdenv performances.

- The omnetpp.ini entries that control Cmdenv execution mode and amount of
  printed info have been changed. An express-mode=yes/no (default: no) switch
  has been introduced, with a meaning similar to Tkenv. Normal mode
  (express-mode=no) can be controlled with the module-messages=yes/no,
  event-banners=yes/no, and message-trace=yes/no entries (the third adds new
  functionality), while express mode is affected by the status-frequency=
  <num-events>, performance-display=yes/no entries. autoflush=yes/no has
  also been added. The display-update=, verbose-simulation= entries are now
  obsolete.

- The -h command-line switch now lists all networks and modules linked into
  the executable.

Tkenv:
- Inspector windows are now restored from a ./.tkenvrc file if you restart the
  simulation. More precisely: as soon as an inspected object (identified by
  fullpath and classname) gets created in the simulation, its window pops up.
  Objects must have unique names for this to work properly! [Thanks to Nicky
  Van Foreest for the idea]

- For better navigation of simulation objects, an object tree view has been
  added to the left side of the main window. It can be turned off from the
  toolbar for performance.

- Inspector windows have been re-though to make them more intuitive, consistent
  and informative. All inspector windows now feature an inspector bar with
  object fullpath, classname and pointer value (useful for debugging), and a
  color code to make it easier to match different inspectors of the same
  object. Axis labelling have been added to output vector inspectors.
  Added tooltips to graphical module inspectors, speeded up animation of on
  very long connection arrows, and refined functionality of the Step icon
  into 3 distinct icons ("Run until next local event", "Fast", "Stop").

- Improved File|New run and New network dialogs (now also available from the
  toolbar) offer choice from a combobox. Other improvements: main text window
  is now cleared when simulation is restarted; more tooltips & toolbar icons;
  better output in Message Trace window; increased event number status bar
  field width by 3 digits (it was limited in size to 1.000.000 events);

- With "ring" layout, changed display order of modules from clockwise to the
  more natural counter-clockwise.

- Extensive internal refactoring mostly because of the introduction exception
  handling and reorganization of inspector registration and creation.

Sample simulations:
- HCube, Token Ring and Dyna2 now use message subclassing. Token Ring has
  also been significantly revised. The PVMEx example has been removed.

- A new example simulation Topo has been created to demonstrate NED features.
  Currently it shows how to build various parametric topologies (chain, mesh,
  ring, star, random graph, binary tree, hexagonal grid) using NED.


OMNeT++ 2.2p1,p2,p3
-------------------
Only exist in source patch form.


In the OMNeT++ 2.2 release (May 2002)
-------------------------------------

It's almost a year since the last, 2.1 release, and that's why this change
list tends to be a bit lengthy. Sorry about that.

For this release I got many more bugfixes, bug reports and suggestions than
for any previous one. Thanks to everybody who helped! Huge credit goes to
Ulrich Kaage in Karlsruhe who built up a server infrastructure (CVS,
bugtracker, Wiki, etc.) and was restless in testing and improving OMNeT++.
Thanks Uli! This is the first release since the CVS went online last year.

This release contains experimental support for the long-demanded feature known
as 'message subclassing'. This means that C++ message classes can be generated
from NED code, replacing the practice of dynamically adding cPar objects to
messages. Data fields added via the subclassing mechanism are inspectable in
Tkenv. The new NED syntax is experimental; the compiler extensions have been
temporarily implemented in Perl. The new Dyna2 sample simulation demonstrates
this feature. See doc/msgsubclassing.txt for more information.

The simulation kernel now has an indexed and hyperlinked on-line API reference,
generated from header file comments. (We're using Doxygen.) At the same time,
the Reference chapter in the manual was deleted.

Distributed simulation can now use the MPI library which is more readily
available nowadays than PVM. (Code contributed by Eric Wu).

The simulation kernel was made const-correct; because of this, older code
may need changes (especially where one didn't use the Module_Class_Members()
macro to define module classes.) There were other changes on the simulation
kernel to make it cleaner.

cGate::setDisplayString() was fixed: changes are now immediately reflected
in the Tkenv graphics as they should. cModule's display string methods were
reorganized: the displayString(int type) method was split into displayString()
and displayStringAsParent(), eliminating the type parameter. Similar change
was made to the set...() methods. (Old methods remained but are now
deprecated.)  All setDisplayString() methods are now compatible with
auto-layouted modules (randomly placed modules won't jump around in Tkenv
after each display update).

New methods: opp_nextrand(long& seed); opp_strlen(..); opp_strprettytrunc(..);
opp_mkindexedname(..) (the latter was formerly called indexedname()).
fullName() no longer uses a static buffer; with fullPath(), it is possible
to avoid static buffers by using the new fullPath(buffer,buflen) method.

In NED, an expression may now refer to a submodule parameter with the following
syntax: submod.par or submod[index].par. A limitation is that the index
might not be an expression itself.

Foundations of a simulation kernel regression test suite have been laid
down. The test/ subdirectory contains a few initial test cases that rely
on the new opp_test tool. The test tool can be very well used to build
model tests, too. Also, as tests tend to be compact and easy to write, the
test tool is also excellent for experimenting with OMNeT++ library classes.
See doc/opp_test.txt for details.

The Envir library has been extended with a plugin mechanism: one can now
plug in code that actually handles recording output vectors (e.g. one can
write them to database instead of files, apply filtering before writing
them out, or send them to a simulation controller app) without having
to modify the user interface library, the simulation kernel or existing models.
Similar mechanisms exist for output scalars and snapshots. The new
include/envirext.h header contains the interfaces the plugins have to
conform to: cOutputVectorManager, cOutputScalarManager, cSnapshotManager.

Tkenv's plugin handling was improved: plugins are searched in the directories
in OMNETPP_PLUGIN_PATH (set to ./plugins by default). DLL file names that are
looked for now correctly depend on the platform (*.dll on windows, .so* on
Unix).

For the default output vector mechanism (vec files), number precision was
increased to 9 digits (fprintf format changed).

The module parameter change logging feature is no longer supported.

Size limitations on omnetpp.ini were removed (only max line length=1024 chars
remained).

ChangeLogs are now GNU-style, with the most recent entries at the top.

Exit codes of simulation programs were made more natural: 0 if the simulation
terminated normally, 1 otherwise. Also, SIGTERM and SIGINT signals are now
handled more gracefully: they call finish() before terminating the simulation
(Unix only).

In GNED, regexp find/replace now understands the \1,\2,... notation in replace
strings. Backwards search was implemented in the Find/Replace dialogs of GNED
and Tkenv. Fixed Plove's problems with tabs/spaces in vector files and
slashes/backslashes on Windows, and also GNED's problems with command-line
arguments.

The makefiles now use a new Perl-based dependency generator which works on all
platforms. opp_makemake: added -L option, and removed Makefile.in from the
'depend' target.


In the OMNeT++ 2.1 release (May 2001)
-------------------------------------

This release is devoted to making OMNeT++ easier to install, easier
to use and easier to learn. Most important, there is now a binary package
for Windows. The install package comes with Tcl/Tk, Gnuplot and some
Unix utilities (grep, awk) bundled, and in addition to the OMNeT++ programs
and libraries compiled with MSVC, it also contains prebuilt executables
of the sample simulations. To shorten the learning curve, the package
contains Nick van Foreest's Queueing Tutorial. Some MSVC integration stuff
(AppWizard, macro to add a NED file to the project) was also added.

Changes in the source distribution: it now includes the Tutorial and
the sources of the MSVC integration components. To facilitate creating
binary distributions, omnetpp.h and the other simulation kernel headers
have been moved out of src/sim/, to a new include/ directory. There was no
change on the simulation kernel itself.

Further enhancements: Plove can now be built as a single executable with
compiled-in Tcl code; on Windows, Plove and GNED can now be compiled as GUI
apps (using WinMain() instead of main()). I added makefiles for Win95/98/ME
(although I couldn't test them). Borland C++ is no longer supported:
I removed the old project files (*.ide), but added back (unsupported)
Makefile.bc's.


In the OMNeT++ 2.0 patch 1 (March 2001)
---------------------------------------
Fixes build issues with Cygwin, PVM, and static libs under Unix.


In the OMNeT++ 2.0 final version (February 2001)
------------------------------------------------

There are a number of enhancements mainly on the GUI parts, GNED and Tkenv,
but the simulation API remains fully compatible. Bugs that were reported
since the 2.0b5 release have largely been fixed. All those who cared to
report bugs and suggested features -- thanks!

In cQueue, a number of methods were made virtual to enable customizing the
class (e.g. adding statistics for maintaining average queue length, etc.).
Some bugfixes on cQueue, cArray and cBag. I also added the OMNETPP_VERSION
symbol to the sim header files (currently #defined as 0x0200).

Tkenv was enhanced quite a bit. Now you can now open module output windows for
all modules (not only for simple modules), and the window will accumulate
ev<< output from that module and all submodules. (This feature is especially
useful for tracing dynamically created modules). Now you can search for text
in all module windows (^F Find, F3/^N Find next). Hotkeys were assigned to the
run commands too (F4 Step, F5 Run, F6 Fast, F7 Express, F8 Stop), and the
behaviour of the "step within module" command (^F4) was refined. I added a
Performance status bar (it displays events/sec, simulated sec/sec,
events/simulated sec readings). Another toolbar shows (among others) the number
of message objects currently in existence -- this feature may help you discover
the most common kind of memory leaks: forgetting to delete messages.
Display string handling was improved: modules in a module vector may now be
positioned individually, and submodule display strings may use the parent
module's parameters, too. See src/tkenv/ChangeLog for details.

The most important addition to GNED is its new, experimental XML support (needs
the TclXML-1.2 package). XML will play a much more significant role in future
releases of OMNeT++, as a universal data exchange format. Many smaller
enhancements: GNED now accepts NED file names as command line arguments;
.gnedrc was introduced; more & better Properties dialogs (Channel, Connection,
Module, etc. Properties); GNED is more careful now when you want to exit the
program; the File menu now has a Save all item. Plus a number of other fixes
to improve the robustness and reliability of the code.

A short section was added to the manual about parallel simulation in general.


In the OMNeT++ 2.0 beta 5 version (June 2000)
---------------------------------------------
Fixed the errors/warnings that occurred when compiling 2.0b4 with gcc 2.95.2.


In the OMNeT++ 2.0 beta 4 version (May 2000)
--------------------------------------------
This is release is a preparation to the 2.0 final release: most of the changes
improve on the ISO compliance, cleanliness, portability or quality of the code
and fix bugs, but there are a number of new features too.

Steps toward better ISO compliance: the source was made 'const char *'-correct,
and the TRUE/FALSE #defines were replaced by the true/false C++ keywords.
Also, nearly all '-Wall' gcc warnings were eliminated from the source.

Now it is possible to create DLLs from the libraries on Windows. This wasn't
as easy as I thought, because there are fundamental conceptual differences
between Windows DLLs and Unix shared objects. (For me it seems like the Unix
approach is a lot more flexible.) The MSVC makefiles were further refined
using a contribution from Balazs Kelemen.

Improvements on the simulation library: Multi-stage module initialization
support via cModule's initialize(int stage) and int numInitStages() functions.
The send...() functions now have versions which take cGate* pointers.
sum() and sqrSum() members added to the statistics classes (cStdDev and
subclasses); isNumeric() member added to cPar; remove(cObject *) added to
cArray. Also, three functions were added to cModule to facilitate navigating
the module hierarchy: int findSubmodule(...), cModule *submodule(...) and
cModule *moduleByRelativePath(...).

New Cmdenv/Tkenv features: you may link more than one user interfaces into the
simulation executable, and you can select one at runtime with the
[General]/user-interface=Tkenv (or =Cmdenv) ini file option or the -u Tkenv
(or -u Cmdenv) command-line argument to the simulation executable.
The per-module extra stack size needed by the user interface libraries
can now be configured from the ini file: [Cmdenv]/extra-stack= and
[Tkenv]/extra-stack= options.

The setDisplayString() sim.library functions are now properly handled by Tkenv,
so you can change the network graphics at runtime. Module parameters in display
strings (e.g. in "p=$xpos,$ypos;i=router") are also implemented finally.
However, implementing the message appearance customization feature (message
display strings) was left for next releases.

In NEDC (and GNED), fixed the ?: operator and changed its precedence to bind
looser than || and &&. Changed nedc-generated code to suppress warnings about
unused variables (which are normal anyway in machine-generated code); this
could only be done for MSVC and Borland C++ because gcc seems to lack such a
pragma.

Unfortunately I didn't get a chance to further refine GNED. It would really
help to have contributors to GNED development, so if you're interested, please
write! (Basically only Tcl/Tk programming is required [no need to touch the
C++ part], and I could give any support needed.)

Makefiles change: the 'install-local' target was eliminated: the new-built
libraries and programs are immediately copied to the lib/ and bin/ subdirs.

Several other smaller changes like fixed PVM execution; improved error
handling, bugfix related to wrong transmissionFinishes() behavior, etc; see the
sim/ChangeLog file.


In the OMNeT++ 2.0 beta 3 version (Jan 2000)
--------------------------------------------
License change: you can now elect to use GPL for the whole OMNeT++ package.

Compiling with MSVC is now supported. Makefile.vc files have been added for
system components, and project files for sample simulatons. No DLL support yet.
MSVC support required a number or small changes in the sources too.

The documentation was significantly expanded and reorganized. RefMan got merged
into the User Manual, and the new User Manual now contains a lot of background
information, new hints and techniques, references to other simulation packages,
etc. New sections include in-depth analyis of handleMessage() and activity(),
and a comparison with Parsec.

GNED has been further enhanced: you can create submodules by dragging a module
type from the tree view to the canvas; submodule names can be edited in-place;
there are dialogs to edit submodule, connection, channel, etc. specifications
directly.

A new sample called Demo was added which works as a GUI shell to the other
sample simulations, and also demonstrates how you can link several simulation
models into a single executable. The other samples were cleaned up a bit too
(e.g. indentation & commenting made more consistent).

Some API changes were made to improve the clarity of the code. See API_CHANGES
in the doc/ directory.

TVEnv was removed from the distribution.


In the OMNeT++ 2.0 beta 1 and 2 versions (Sep,Nov 1999)
-------------------------------------------------------

License conditions have been alleviated: from this release on, one can elect
to use GPL for the base components of OMNeT++.

The configure script has been replaced by GNU Autoconf. The makefiles have been
brushed up (more standard symbol names, autodepend, version number in lib names,
etc). Cygwin is now fully supported. DOS+BC3.1 support has been removed for
lack of interest. File names longer than 8+3 are now used within the package.

Most of the effort since the last release was put into GNED. As a result,
GNED can now directly read/write NED files, so you can use it to work on your
existing NED sources. Moreover, GNED is now a fully two-way tool: you can edit
compound modules in graphic or in NED source form, and switch to the other view
any time. The GUI has been improved too: toolbar icons, tooltips, NED syntax
highlight. GNED is still in beta and there's some missing functionality.

Tkenv's user interface has also undergone some beautifying and bugfixing:
now it has toolbar icons, tooltips, etc.

OMNETPP_BITMAP_PATH can now contain a list of directories; it includes "." and
"./bitmaps" by default, so individual simulation models can have their own
icons in their own directories.

Smaller enhancements were made to the simulation library. Some of them are:
dynamic simple modules can now be created and booted in one step by using
cModuleType's createScheduleInit() function; callInitialize() and callFinish()
was added to the module classes (for compound modules they work recursively);
behavior of cancelEvent() slightly changed. cMessage now has isScheduled()
and isSelfMessage() member functions; a "context pointer" was also added to
cMessage to facilitate writing modules which use a large number of self
messages concurrently.

See the ChangeLog in the directory of the individual components for details
on what has been changed/fixed/improved.

Contributions have been added. They include NED syntax highlight for Emacs,
and recipes and sample code to use Octave with OMNeT++.

Which is less visible to you, the development has moved to use CVS.


In version of Feb, 1999 (OMNeT++ 1.1)
-------------------------------------

handleMessage() support added as alternative to activity(). Finite
State Machines (FSMs) are now also supported. They are demonstrated
in the fifo2 sample simulation.

The cPacket class and the protocol.h header was added; its aim is
to standardize on protocol models. Negative message kind values
are now reserved.

Added contributed howto about using OMNeT++ on NT with CYGWIN. Also
added contributed report on using message subclassing.

The way of writing custom inspectors has been redesigned to make it
more flexible. Also, Tkenv now has a plugin interface.

Several bugfixes and smaller improvements: Tkenv message inspector
redesigned; bugs in the ini file reader and the statistics classes
were fixed; vsscanf() was eliminated; the makefiles are now created
from makefile.in files; wait(), receive() and surroundings were
optimized; bug in GNED bug fixed etc.


In version of Aug, 1998 (OMNeT++ 1.0)
-------------------------------------

Data collection by output vectors can be configured in the ini file
(ie. OPNET 'probes'). Display strings can now be specified in the ini file
as an alternative to embedding them in ned files.

GNED has been totally rewritten. Now it supports colors, icon selection
etc. and correctly generates display strings. Cut/copy/paste capability.

A models/support library added with several base/utility classes and modules.

Ported to Windows 95/NT. Borland C++ 5.0 project files are included.

Numerous smaller improvements on Plove and Tkenv. Possibility
to make standalone simulation executables with Tkenv's TCL script
parts compiled into the executable.

To-do list and coding conventions (style guide) has been added to the
docs; please stick to it. Tkenv is now better documented; and so is the
cKSplit class.

As usually, additions and changes in the User Manual are marked with
(new) in the text. It does not necessarily mean the feature documented
there is new; most probably only the documentation is.

As usually, you're welcome to hack on the code and send in patches!
GNED is a very good target to start at. If you plan to do development,
you can sign up on the OMNeT++ mailing list and exchange ideas with
others.


In version of July, 1998
------------------------

Documentation now includes quickstart reference for OPNET users.
Additions to the User Manual are marked with (new!) in the text.

Tkenv has been improved significantly. Better animation: message names
are displayed and message kind is color-coded. New connection inspector
shows a connection path graphically. Module inspector has been redesigned
in notepad-style. Right-click popup menus in inspector windows.
Variables can be changed at run-time by cWatch inspectors. You can now
use the menu and play with the inspectors while simulation is running.
Several smaller improvements.

Routing support through the cTopology class. Other new classes are
cWeightedStdDev, cLinkedList. Direct message encapsulation support.
cPar can now store void* pointers. Output scalar file added which
accumulates results from several simulation runs. Several refinements
on the class library, more informative error messages.

Improved makemake script supports large models which span across
multiple directories. Plove made more intuitive with mc-like key bindings
and right-click popup menus.


In version of June 5, 1998
--------------------------

Documentation now comes also in HTML. We now have an LSM entry.

Tkenv made the default user interface.

Network graphics (module icons, connection arrows etc) and message
flow animation has been added to Tkenv. Usefulness of Tkenv dramatically
improved. Default graphics for models originally written without
graphics (no modifications needed).

By default, libraries are now dynamically linked which drastically reduces
executable sizes. You can revert to static linking by editing in `configure'.
You can load shared objects (e.g simple module code) at run-time
(-l command-line option).

Source directory tree changed to be more 'unix-like'.

New histogram class with user-defined and equal-frequency cells added
(thanks to Gabor Lencse), existing ones cleaned up and significantly
enhanced.

Wildcards (*,?) can be used in omnetpp.ini to supply values to several
model parameters at a time. Ini file inclusion and multiple ini files
support (via multiple -f command-line options) added. Total stack size
for the coroutines can now be given in the omnetpp.ini file.

Several bug fixes and minor improvements on Plove, GNED etc.


Version of May, 1998
--------------------
Wasn't publicly announced.


In version of March, 1998
-------------------------

An new interactive tool, Plove has been added. Plove is for analyzing
and plotting output vector files. Plove uses gnuplot to do the actual
work. In addition to viewing, you can save the gnuplot graphs to files
(postscript, latex, pbm etc) with a click. Filtering the results
before plotting (averaging, truncation, smoothing, etc) is possible.
Some filters are built in, and you can easily create new filters or
modify the existing ones.

Portability: Unix and (with some limitations) Win95/NT


In version of February, 1998
----------------------------

Type checking has been added to the NED language and the compiler.
New operators have also been added (like binary and/or/xor, shifting etc).

Now it is not necessary to declare module parameters and gates in .cc files
(Module..End stuff can be left out).

A new example program, HCube was added. It simulates an arbitrary-size
hypercube network with a simple deflection routing.

A configur script was added that -- on Unix platforms -- finds the
installation directories of Tcl/Tk, X, PVM and Turbo Vision, and
customizes the makefiles accordingly.

The Jar compiler has been renamed to NEDC because the name 'Jar' was
used by other software (a DOS archiver; Java tar on Solaris etc.)

Some scripts in util/ have also been renamed.

Now it is possible to specify values with time units (s,ms,us,m,h,d etc.)
in omnetpp.ini.

Misc improvements: some bugs fixed; some error messages in the sim. kernel
made more informative etc.


In version of December, 1997
----------------------------

A graphical model editor (GNED) is here. It's written entirely in Tcl/Tk.
Try it. It is the first, simple version; there's more to come.

The way of defining simple modules has been simplified a lot. Check out the
manual or see the example programs!

A new example program (fifo) was added that models a single-server queue and
also demonstrates how to derive a simple module from another one.

Important: the [All runs] ini file section has been renamed to [Parameters] to
reduce misunderstandings.


In version of October 15, 1997
------------------------------

Several member functions (and even classes) have been renamed for the sake
of a more readable code as well as consistency in naming and style. The utils
directory contains a Unix script (convert) to upgrade your files.

The cKSplit class was added -- another, experimental on-line distribution
approximation method.

Several bugfixes and improvements, updated manual.

The arg.list to the constructor of the histogram classes has changed --
beware, the compiler might not warn you!


In version of September 15, 1997
--------------------------------

Tkenv has been developed upon a lot -- try it with the improved Token Ring
example!

Parallel execution via PVM has been tested and significantly improved.
Syncpoints (a tool for synchronization) were added. See the user manual
for details. Go ahead and do Parallel Discrete Event Simulation (PDES)
with OMNeT++!

The implementation of the message queue (Future Event Set) has become more
efficient: the underlying data structure has been changed to heap.

Most important change in the simulation class library: cQueue's interface
has changed. See the manual or the source code.

Most important change in the ini file: The [General]/number-of-runs option
has been replaced by the [Cmdenv]/runs-to-execute option and the
-r command-line switch.

Check the version.log files for the numerous other changes. Many bugfixes
and code cleanups were made.
