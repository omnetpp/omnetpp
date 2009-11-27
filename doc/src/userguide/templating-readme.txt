= Dummy

== Custom Wizards

=== Motivation

TODO: mire szolgal; kinek kellene ilyet csinalni es mi celbol...

The OMNeT\++ IDE offers several wizards via the File|New menu: New OMNeT++
Project, New Simple Module, New Compound Module, New Ini File, and so on.
By default these wizards already offer useful functionality to the user,
but using the information in this chapter, it is possible to add to them
new UI elements and content templates that are specific to simulation models.
For example, one can create a New INET Ad-Hoc Network wizard, a New MiXiM
Simulation Wizard, an Import INET Network From CSV File wizard, a
New MiXiM Radio Model wizard, and so on.

These wizards can be prepared with low effort and without any Java or C++
programming, and can be distributed with the corresponding model framework
(i.e. INET or MiXiM). When end users import the model framework project
into the IDE, the wizards get automatically contributed to the corresponding
wizard dialogs; the end user does not need to install them or otherwise do
anything to deploy the wizards.

When you are writing a model framework which will be available for the
general public, wizards are a great way to teach your users about your
simulation model. While documentation and example simulations are somewhat
passive ways of educating your users, wizards are more interactive and
encourage users to experiment with the model and explore its capabilities.
The right set of capable wizards can give a jump start to users, and
emphasizes learning by doing.


=== Overview

Custom wizards are read from the templates/ folder of OMNeT++ projects.

Wizards are implemented by mixing a templating engine (for generating
the output files) and a GUI description language (for custom wizard pages).
Because of the use of a templating engine, we'll also refer to custom
wizards are "content templates".

In the templates/ folder, every subfolder that contains a template.properties
file is treated as a content template. (Other folders are ignored.) Every
content template folder may contain several types of files: the
template.properties file contains general information about the wizard;
*.ftl files are template files that will be copied (without the .ftl extension)
into the target folder after template variable substitution; *.xswt files
describe custom wizard pages. More rarely used files are *.fti (template
include) files included by *.ftl files; and *.jar files that can be used
to extend the wizard's functionality with dynamically loaded Java code.
All other files are regarded as files that have to be copied into the
target folder verbatim when the wizard runs. The wizard folder may contain
subdirectories which may also contain files of the above type (except
template.properties of course).

When the template is used, the contents of the template folder (and subfolders)
will be copied over into the new project preserving the directory structure,
with the exception of template.properties and other known special-purpose files.
(It is also possible to specify other files and folders to ignore during copying.)

Files with the ".ftl" extension are regarded as templates, and they will undergo
template processing (and the ".ftl" extension gets chopped off) before copying.
Other files are copied verbatim.

When the wizard is being used, a pool of variables is kept by the wizard dialog.
These variables are the ones which will be eventually substituted into the \*.ftl
files ($\{varname} syntax). These variables are initialized from the key=value
lines in the template.propeties files; they can get displayed and/or edited
on custom wizard pages; and eventually they get substituted into \*.ftl files.
Some variables have special meaning and are interpreted by the wizard dialog
(e.g. the nedSrcFolders variable determines which folders get denoted as
NED Source Folders by the New OMNeT++ Project wizard). Variables can be used
to generate output file names, can be used as input file names, and can serve
as input and working variables for arbitrarily complex algorithms programmed
in the template (*.ftl) files.


=== Template Processing

Template processing uses the FreeMarker library (http://freemarker.org), and
all template syntax supported by FreeMarker can be used. Especially, variable 
references of the syntax $\{varName} will be replaced with the variable value.

The FreeMarker language also offers constructs that make it a full programming
language: variable assignments, conditionals, loops, switch statement,
functions, macros, expression evaluation, built-in operators, etc, as well
as access to fields and methods of classes defined in Java. This means that
any algorithm can be expressed in the FreeMarker language, and if that would
not be enough, one can also pull in existing or custom-written Java libraries.

The following variables are predefined:

templateName::  name of the template (see later)
templateDescription::  template description (see later)
templateCategory::  template category (see later)
date::  current date in yyyy-mm-dd format
year::  year in yyyy format
author::  user name ("user.name" Java system property)
licenseCode::  license identifier for the @license NED property
licenseText::  copyright notice for the given license
bannerComment::  banner comment for source files; includes license text
templateFolderName::  name of the folder in which the template files are
templateFolderPath::  workspace path of the folder in which the template files are
templateProject::  name of the project that defines the template

The New OMNeT++ Project wizard also defines the following variables:

projectName, rawProjectName::
                        the project name, "as is"
ProjectName::  sanitized project name with first letter capitalized
projectname::  sanitized project name in all lowercase
PROJECTNAME::  sanitized project name in all uppercase

sanitization means making the name suitable as a NED or C/C++ identifier
(spaces, punctuation and other unfit chars replaced with underscore, etc.)

Other variables may come from custom template pages, see later.


=== The template.properties File

The template.properties file is a standard Java property file (key=value syntax).
That file can be used to supply a template name, a template description,
and various other options.

Recognized property file keys:

templateName::  The template's display name; defaults to the folder name.
                This is the name that appears in the wizard's template selection
                page.
templateDescription::  
                Description of the template. This may appear as a tooltip
                or in a description pane in the wizard.
templateCategory::  
                Template category is used for organizing the templates into a
                tree in the wizard's template selection page. Defaults to the
                name of the project that provides the template.
templateImage::  
                Name of the icon that appears with the name in the wizard's
                template selection page.
supportedWizardTypes::  
                Comma-separated or JSON-syntax list of wizard types (e.g.
                "nedfile", "simplemodule", "project", "inifile") that
                this template supports. More about this later.
ignoreResources::  
                Comma-separated or JSON-syntax list of non-template files or
                folders; those files won't get copied over to the new project.
                Wildcards are accepted. The template.properties file and
                custom wizard page files automatically count as nontemplates,
                so they don't have to be listed.

The "New OMNeT++ Project" wizard also recognizes the following options. These
options can be overridden from custom wizard pages.

addProjectReference::  
                True or false; defaults to true. If true, the template's project
                will be added to the referenced projects list of the new project.
sourceFolders::  
                Comma-separated or JSON-syntax list of C++ source folders
                to be created and configured. By default, none.
nedSourceFolders::  
                Comma-separated or JSON-syntax list of NED source folders
                to be created and configured. By default, none.
makemakeOptions::  
                Comma-separated list of items in the syntax "folder:options",
                or a JSON-syntax map of strings; it sets opp_makemake options
                for the given folders. There is no default.

There are additinal options for adding custom pages into the wizard, as described
in the next section.


=== Custom Wizard Pages

The following properties can be used to define custom pages in the wizard. <i> is
an integer page ID; their ordering defines the order of wizard pages.

page.<i>.file::
                The name of the XSWT file that describes the wizard page layout.
page.<i>.title::
                Title of the wizard page, displayed in the page's title area.
                Defaults to the template name.
page.<i>.description::
                Description of the wizard page, shown in the dialog's title area
                below the title. Defaults to text in the format "Page 1 of 3".
page.<i>.condition::
                A condition for showing the page. If it evaluates to false, the
                page will be skipped when it would normally come in the page
                sequence of the wizard. This makes it possible not only to skip
                pages, but also to show different pages based on some choice
                or choices the user made earlier in the wizard (decision tree).
                The condition will be evaluated by the template engine, so any
                valid FreeMarker expression that can produce true or "true" as
                a result will do. The expression must be supplied without the
                ${ and } marks: the string you enter will be substituted into
                "${( <condition> )?string}" string (will replace "<condition>"),
                and evaluate as such. An example will be provided later.

All property file entries are available as template variables too. Also, most
property values may refer to other property values or template variables, using
the $\{name} syntax.

Custom wizard pages are defined in XSWT (http://xswt.sourceforge.net,
http://www.coconut-palm-software.com/the_new_visual_editor/doku.php?id=xswt:home).

NOTE: In the future, Java-based custom wizard pages will also be supported.
        XSWT has the advantage that it maps one-to-one to SWT, and a .xswt file
        can be easily machine-translated to equivalent Java source code. Another
        advantage of XSWT is that it supports custom widgets and classes seamlessly,
        via a Java-like import resolution mechanism and the use of Java reflection.

NOTE: Currently we use XSWT 1.1.2. Newer XSWT integration builds from
        http://www.coconut-palm-software.com/~xswt/integration/plugins/
        did not work out well.


=== Binding of Template Variables to Widgets

XSWT allows one to tag widgets with id attributes. Widget IDs will become the
names of template variables, with the values being the content of widgets.
For example, text widgets (org.eclipse.swt.widgets.Text) provide a string value
(Java type "String"), while checkboxes and radio buttons provide a boolean
(Java type "Boolean").

XSWT also allows filling up the widgets with default values, but this feature
should be used with care, because the defaults set in XSWT may not make it
to the file templates! This is so because if the user clicks Finish early,
the rest of the wizard pages (and their widgets) may not be created at all,
so values set in the XSWT will take no effect.

For this reason, defaults should ALWAYS be set in the property file. To do so,
simply use a key with the ID of the widget; those values will be written
into the wizard page when the page is created. For example, if you have a
text widget with id="numNodes" and a checkbox with id="generateTraffic", then
the following lines

  numNodes = 100
  generateTraffic = true

will fill in the text widget with "100" and select the checkbox. Widgets that
do not have such lines in the propery file will be left alone.

Compound data structures (arrays, maps, and any combination of them) can be
specified in the JSON syntax (http://json.org). They can be iterated over
in the templates, and can be used as input/output for custom compound widgets.
Examples:

  apps = ["ftp", "smtp", "news"]
  layers = {"datalink":"ieee80211", "network":"ip", "transport":["tcp","udp"]}

The property file takes precedence over values in the XSWT file.


=== XSWT Data Binding

This is the way how values get transferred between widgets and template
variables (R=read, W=write):

Button::         This SWT class represents buttons, checkboxes and radio
                buttons, depending on its style attribute (SWT.PUSH,
                SWT.CHECK, SWT.RADIO).
                * W: the string "true" selects the checkbox/radiobutton,
                   everything else clears it.
                * R: returns a Boolean.

Combo, CCombo:: Represent a combo box and a custom combo box. It can be
                made read-only (with the SWT.READ_ONLY style); a read-only
                combo only allows list selection but no manual editing.
                The list items can be specified from XSWT. Variables only
                work with the textedit part (cannot add/remove list items).
                * W: the string value gets written into the combo. If the combo
                   is read-only and contains no such item, nothing happens.
                * R: returns the currently selected item as string.

DateTime::       A widget for editing date/time.
                * W: accepts a string in the following format: "yyyy-mm-dd hh:mm:ss".
                   If the string is not in the right format, an error occurs.
                * R: returns a string in the same format, "yyyy-mm-dd hh:mm:ss".

Label::          Label widget (not interactive).
                * W: sets the label to the string
                * R: returns the label

List::           A listbox widget that allows selection of one or more items,
                depending on the style attribute (SWT.SINGLE or SWT.MULTI).
                List items can be specified from XSWT. Template variables only
                work with the selection (cannot add/remove list items).
                * W: accepts a string with comma-separated items, and selects the
                   corresponding item(s) in the listbox. Items that are not
                   among the listbox items are ignored.
                * R: Returns a string array object (String[]) that can be
                   iterated over in the template.

Link::           A label with hyperlink-like functionality; not very useful for
                our purposes. Handled similarly to Label.

Scale::          A graphical widget for selecting a numeric value.
                * W: accepts strings with an integer value. Non-numeric strings
                   will cause an error (a message dialog will be displayed).
                * R: returns an Integer which can be used in arithmetic
                   expressions in the template.

Slider::         A scrollbar-like widget for selecting a positive numeric value.
                Handled in a similar way as Scale.

Spinner::        Similar to a textedit, but contains little up and down arrows,
                and can be used to input an integer number.
                Handled in a similar way as Scale.

StyledText::     A textedit widget which allows displaying and editing of
                styled text. Handled similarly to Text.

Text::           A textedit widget. It can be single-line or multi-line,
                depending on the style attribute (SWT.SINGLE, SWT.MULTI).
                * W: accepts a (potentially multi-line) string.
                * R: returns the edited text as a string.

Table and tree widgets are currently not supported in a useful way, the main
reason being that SWT Tables and Trees are not editable by default.

Some non-interactive widgets which cannot be connected to template variables
but are useful in forms as structuring elements are: Composite, Group, Sash,
TabFolder/TabItem.


=== Conditional Custom Pages

Now that templating and XSWT were covered, we can revisit how one can use page
conditions. Consider the following practical example:

Suppose a wizard for wireless networks. On the first page of the wizard there
is a "[] configure routing" checkbox with the ID "wantRouting". If this
checkbox gets selected, you want to display a second page where the user can
select a routing protocol, then further configuration pages depending on
the particular routing protocol the user chose. To achieve this, you would
add the following lines to template.properties:

  page.1.title = General   <-- page with the "wantRouting" checkbox
  page.2.title = Choose Routing Protocol  <-- page with the "protocol" combobox
  page.3.title = AODV Options
  page.4.title = DSDV Options

  page.2.condition = wantRouting
  page.3.condition = wantRouting && protocol=="AODV"
  page.4.condition = wantRouting && protocol=="DSDV"

You get the idea.


=== Wizard Types

The IDE offers several OMNeT++-related wizard dialogs: New OMNeT++ Project,
New NED File, New Simple Module, and so on. Every content template can
contribute to one or more of those wizard dialogs. The template author
has to list the supported wizards in the supportedWizardTypes property file
entry. Values are:

*  project
*  simulation
*  nedfile
*  inifile
*  msgfile
*  simplemodule
*  compoundmodule
*  network
*  wizard

A possible setting in the template.properties file is:
supportedWizardTypes = project, simulation, nedfile, network

In turn, the wizard will set the wizardType template variable when it executes,
so template code can check under which wizard it runs (using <#if>..</#if>), and
act accordingly.

Specific wizard dialogs will also define extra variables for use in the
templates, e.g. the wizard types that create a single file will put the
newFileName variable into the context.

In the next sections we describe the individual wizard types.

==== "project"

Presented in the "New OMNeT++ Project" dialog. Extra variables:
  nedPackageName
  withCplusplusSupport

==== "simulation"

TODO

==== "nedfile"

TODO

==== "inifile"

TODO

==== "msgfile"

TODO

==== "simplemodule"

TODO

==== "compoundmodule"

TODO

==== "network"

TODO

==== "wizard"

TODO show how to write templates that are suitable for more than one wizard.

=== Using Custom Widget Classes

Since XSWT works via Java reflection, your own custom widgets can be used
the forms very much like normal SWT widgets. No glue or registration code
is needed, just add their package to the <import> tags at the top of the XSWT
file.

However, some Java code is needed so that the wizard knows how to write
template variables into your wigets and how to extract them after editing.
This functionality can be added via the org.omnetpp.cdt.wizard.IWidgetAdapter
interface. This interface must be implemented either by the widget class
itself, or by a class named <widgetclass>Adapter in the same package.
The interface has methods to tell whether the adapter supports a given widget,
to read the value out of the widget, and to write a value into the widget.

In addition to basic data types (Boolean, Integer, Double, String, etc),
it is possible to use compound data types as well, i.e. those composed of
the List and Map interfaces of the Java Collections API. The default values
can be given in the template.properties file in the JSON notation, and the
result can be used in the templates (iteration via <#list>, etc).


=== Using External Java Code

Jar files placed into the plugins/ subdirectory of an OMNeT++ project will be
loaded automatically, and will be avalable for all templates. Jar files in
that directory may be plain Java jars and Eclipse plug-in jars. (The latter
makes it also possible to contribute new functionality into the IDE via
various extension points, but this is outside the scope of this discussion
about wizards.)

In addition, jar files placed in the folder of the template will
be loaded automatically when the template is used, and the classes in it will
be available for that template. Custom SWT widget classes can be imported and
used in XSWT forms, and other code can be used in the template files via the
bean wrapper (e.g. ${classes["org.example.SomeClass"].someStaticMethod(...)},
see the example wizards.)


=== Utility Classes Available from the Templates

The following Java classes have been added to the context:
  Math		java.lang.Math
  FileUtils	U
  StringUtils	org.apache.commons.lang.StringUtils
  CollectionUtils	org.apache.commons.lang.CollectionUtils
  IDEUtils	TODO
  LangUtils	TODO

Example template code:

  The value of 10*cos(0.2) is ${10*Math.cos(0.2)}.

  The words of the sentence '${sentence}':
  <#list StringUtils.split(sentence) as word>
     ${word}
  </#list>

  Properties in the file ${filename}:
  <#assign props = FileUtils.readPropertyFile(filename)>
  <#list props?keys as key>
     ${key} ==> ${props.get(key)}
  </#list>


==== Math:

Represents the Java Math class, which contains mathematical functions.
See http://java.sun.com/j2se/1.5.0/docs/api/java/lang/Math.html

Math functions:

  method: double cos(double x)

  method: double sin(double x)

  method: double pow(double x, double y)

etc.

==== FileUtils:

Contains utility functions for reading files in the following formats: XML,
JSON, CSV, property file, and functions to read and return a text file
as a single string, as an array of lines, and as a an array of string arrays
(where string arrays were created by splitting each by whitespace).

There are two sets of functions, one works on files in the Eclipse workspace,
and the other files on "external" files, i.e. files in the file system.
Files are interpreted in the Java platform's default encoding (unless XML files,
which specify their own encoding.)

FileUtils functions:

  method: org.w3c.dom.Document readXMLFile(String fileName)
  method: org.w3c.dom.Document readExternalXMLFile(String fileName)
  
Parses an XML file, and return the Document object of the resulting
DOM tree.

  method: Object readJSONFile(String fileName)
  method: Object readExternalJSONFile(String fileName)
  
Parses a JSON file. The result is a Boolean, Integer, Double, String,
List or Map, or any data structure composed of them.

  method: String[][] readCSVFile(String fileName, boolean ignoreFirstLine, 
                         boolean ignoreBlankLines, boolean ignoreCommentLines)
  method: String[][] readExternalCSVFile(String fileName, boolean ignoreFirstLine, 
                         boolean ignoreBlankLines, boolean ignoreCommentLines)
                         
Reads a CSV file. The result is an array of lines, where each line is
a string array. Additional method parameters control whether to discard the
first line of the file (which is usually a header line), whether to
ignore blank lines, and whether to ignore comment lines (those starting
with the # character). Comment lines are not part of the commonly accepted
CSV format, but they are supported here nevertheless, due to their
usefulness.

  method: Properties readPropertyFile(String fileName)
  method: Properties readExternalPropertyFile(String fileName)

Parses a Java property file ('key=value' lines) in the workspace.
The result is a Properties object, which is effectively a hash of
key-value pairs.

  method: String[][] readSpaceSeparatedTextFile(String fileName, boolean ignoreBlankLines, boolean ignoreCommentLines)
  method: String[][] readExternalSpaceSeparatedTextFile(String fileName, boolean ignoreBlankLines, boolean ignoreCommentLines)

Reads a text file, and return its contents, split by lines, and each line
split by whitespace. Additional method parameters control whether to ignore
blank lines and/or comment lines (those starting with the # character).
The result is an array of lines, where each line is a string array of the
items on the line.

  method: String[] readLineOrientedTextFile(String fileName)
  method: String[] readExternalLineOrientedTextFile(String fileName)

Reads a text file in the workspace, and returns its lines. Comment lines
(those starting with a hash mark, #) are discarded. The result is a
string array.

  method: String readTextFile(String fileName)
  method: String readExternalTextFile(String fileName)

Reads a text file, and return its contents unchanged as a single string.

  method: boolean isValidWorkspacePath(String path)

Returns true if the given string is syntactically a valid workspace path.

  method: boolean isValidWorkspaceFilePath(String path)

Returns true if the given string is syntactically a valid workspace file path.
This function does not check whether the file exists, or whether the given path
actually already points to a resource of a different type.

  method: IWorkspaceRoot getWorkspaceRoot()

Returns the workspace root object. The workspace contains the user's 
projects.

  method: IProject asProject(String path)

Returns the handle for the workspace project with the given name.
Throws exception if the path is not a valid workspace project path.
This function does not test whether the project exists. To test that,
call the exists() method on the returned handle.

  method: IContainer asContainer(String path)

Returns the handle for the workspace container (i.e. project or folder) with the given name.
Throws exception if the path is not a valid workspace container path.
This function does not test whether the container exists. To test that,
call the exists() method on the returned handle.

  method: IFile asFile(String path)

Returns the handle for the workspace file with the given name.
Throws exception if the path is not a valid workspace file path.
This function does not test whether the file exists. To test that,
call the exists() method on the returned handle.

  method: IResource asResource(String pathName)

Returns the handle for the workspace project, folder or file with 
the given name. If the resource does not exist and the path contains
more than one segment (i.e. it cannot be a project), it is returned as 
a file handle if it has a file extension, and as a folder if it
does not.

  method: File asExternalFile(String path)

Returns a java.io.File object for the given path. The object can be used to
access operations provided by the File API, such as exists(), length(), etc.

  method: void copy(String path, String destPath, IProgressMonitor monitor)
  
Copies a workspace resource (file, folder or project) given with its path 
to the destination path. For projects and folders, it copies recursively 
(i.e. copies the whole folder tree). From the project root directory it
leaves out dot files, hidden files, and team private files.

  methods:  void copyURL(String url, String destFilePath, IProgressMonitor monitor)
  
Copies the file at the given URL to the given destination workspace file.

  methods:String createTempFile(String content)
    
Writes the given string to a temporary file, and returns the path of the 
temporary file in the file system. The file will be automatically deleted
when the IDE exits, but it can be also deleted earlier via deleteExternalFile().

  method: void createFile(String fileName, String content)

Creates a workspaces text file with the given contents, in the platform's default encoding.

  method:  void createExternalFile(String fileName, String content) {

Creates a text file in the file system with the given contents, in the platform's default encoding.

  method:  void deleteFile(String fileName) throws CoreException {

Deletes the given workspace file. It is OK to invoke it on a nonexistent file.

  method:  void deleteExternalFile(String fileName) {

Deletes the given file from the file system. It is OK to invoke it on a nonexistent file.

  method:  void createDirectory(String fileName) throws CoreException {

Creates a workspace folder. The parent must exist.

  method:  void createExternalDirectory(String fileName) {

Creates a directory in the file system. The parent must exist.

  method:  void removeDirectory(String fileName) throws CoreException {

Deletes a workspace folder. The folder must be empty. It is OK to invoke
it on a nonexistent folder.

  method:  void removeExternalDirectory(String fileName) {

Deletes a directory in the file system. The directory must be empty. 
It is OK to invoke it on a nonexistent directory. 


==== StringUtils

Represents the Apache Commons StringUtils class, which contains over a hundred
utility functions for manipulating strings.
See http://commons.apache.org/lang/api/org/apache/commons/lang/StringUtils.html

StringUtils methods

  method: boolean isEmpty(String s)

  method: boolean isBlank(String s)

  method: String capitalize(String s)

  method: String upperCase(String s)

  method: String lowerCase(String s)

  method: boolean startsWith(String s, String suffix)

  method: boolean endsWith(String s, String prefix)

  method: String[] split(String s)

  method: String join(String[] strings)

etc.


==== CollectionUtils

Represents the Apache Commons CollectionUtils class, which contains some useful
functions for manipulating collections (like lists). Functions include computing
set union, intersection, and difference.
http://commons.apache.org/collections/apidocs/org/apache/commons/collections/CollectionUtils.html

CollectionUtils methods

  method: Collection union(Collection a, Collection b)
  
  method: Collection intersection(Collection a, Collection b)
  
  method: Collection subtract(Collection a, Collection b)
  
TODO

==== IDEUtils

Provides entry points into various aspects of the IDE. This includes access to
the Eclipse workspace (projects, folders, files), and the NED index. The former
is documented in the Eclipse Platform help; documentation for the latter can
be found in the sources (Javadoc).
See http://help.eclipse.org/galileo/index.jsp?topic=/org.eclipse.platform.doc.isv/guide/resInt.htm
and http://help.eclipse.org/galileo/topic/org.eclipse.platform.doc.isv/reference/api/org/eclipse/core/resources/IWorkspaceRoot.html

IDEUtils methods

  method: NEDResources getNEDResources()
  
  method: MsgResources getMsgResources()
  
TODO

==== LangUtils

Provides a collection of Java language related utility functions.

LangUtils methods

  method: boolean hasMethod(Object object, String methodName)

Returns true if the object has a method with the given name.
Method args and return type are not taken into account.
Search is performed on the object's class and all super classes.

  method: boolean hasField(Object object, String fieldName)

Returns true if the object has a field with the given name.
Field type is not taken into account. Search is performed on
the object's class and all super classes.

  method: boolean instanceOf(Object object, String classOrInterfaceName)

Returns true if the given object is instance of (subclasses from or
implements) the given class or interface. To simplify usage, the class
or interface name is accepted both with and without the package name.

  method: String toString(Object object)

Produces a user-friendly representation of the object. In case of
collections (lists, maps, etc), the representation is JSON-like.

  method: List<Object> newList()

Creates and returns a new mutable List object (currently ArrayList).

  method: Map<Object, Object> newMap()

Creates and returns a new mutable Map object (currently HashMap).

  method: Set<Object> newSet()

Creates and returns a new mutable Set object (currently HashSet).

  method: Class<?> getClass(Object object)

Returns the class of the given object. Provided because BeanWrapper 
seems to have a problem with the getClass() method.

==== ProcessUtils

  method: ProcessResult exec(String command, String[] arguments, String workingDirectory, String standardInput, double timeout)

Executes the given command with the arguments as a separate process.
The standard input is fed into the spawn process and the output is read 
until the process finishes or timeout occurs. The timeout value 0 means wait 
infinitely long to finish the process. Arguments at the end of the argument 
list are optional.

  method: String lookupExecutable(String name)

Finds the given executable in the path, and returns it with full path.
If not found, it returns the original string.

=== Editing XSWT Files

Double-clicking on an XSWT file will open it in the XSWT editor. The editor
provides basic syntax highlighting (and not much else currently). An extremely
useful feature of the IDE is the XSWT Preview, where you can see preview
the form being edited (it updates when you save the file). The Preview should
open automatically when you open the XSWT file; if it does not (or you close it),
you can access it via the Window|Show View... menu item.

Some (custom) widgets may not appear in the Preview; this is because the
Preview does not load jar files from the projects.
XXX This may get fixed.


=== Editing Template Files

The Freemarker Editor is opened when you double-click files with the ftl or
fti extension. (The latter stands for Freemarker Template Include, and it is
intended for template fragments that you include into other templates. Otherwise
the wizard ignores fti files, i.e. does not copy them into the new project or
folder.) The Freemarker Editor offers basic syntax highlight, validation
(error markers appear during editing if the template is not syntactically
correct), and basic content assist. Content assist can help you with directives
(<#...> syntax) and builtin operations (like ?number, ?size, ?default, etc).
The content assist popup appears automatically when you type '<#' (actually
a closing '>' is also needed for the editor to recognize the tag), and
when you hit '?' within a directive or an interpolation ($\{...}).



=== References

An XSWT tuturial and documentation can be found at:
http://www.coconut-palm-software.com/the_new_visual_editor/doku.php?id=xswt:home

Documentation for the FreeMarker template language:
http://freemarker.org/docs/index.html

---

TODO document: supportedWizardTypes = project, simulation, nedfile, network
  and in the concrete template it comes back as wizardType, which can be checked against. together with optionalFiles
  e.g. don't generate package.ned if it's a simple NED File wizard

+ TODO document new FileUtils methods (create/delete file/dir)

+ TODO document ProcessUtils

TODO jar files get loaded from the normal folder, and from the project's "plugins" folder. Jar files are not copied over.

TODO "New NED File" wizard milyen valtozokat definial alapbol!
        "wizardType" (=="nedfile)
        "newFileName"  -- the file name
        "nedPackageName" -- expected NED package for that folder
        "nedTypeName" -- derived from the file name
        "bannerComment" -- with copyright notice, etc

TODO document: .fti "freemarker template include" files (they open in the FreeMarker editor, but get ignored on template initiation (ie. don't get copied over))

TODO document glob patterns in verbatimFiles etc

TODO: optionalFiles removed! now all files, if they would be empty, are NOT saved, (existing file, if exists,
 is left there. If you want to generate an empty file, put some comment into it (most files formats allow
 comments), or use FileUtils.writeTextFile() [note: it exists???]. If you want to delete an existing file,
 here's how you do it: FileUtils.deleteFile()

TODO document <@setoutput file=.../>
   redirects output to the given file. The filename can contain slashes too, i.e.
   one can write files in a different folder. If the folder does not exist, it will
   be created.

   If filename is empty, the directive restores output to the original file name (the template's name).
   This also works if a folder name is given and only the file name is missing
   (<@setoutput file="src/">): then it will write the file with the original name
   but into the specified folder.

   If there are multiple setoutput's with the same file name within a template,
   the content will be concatenated. Concatenation only works within one template;
   if you have multiple templates writing into the same file, they will overwrite
   each other's content, and it is undefined which one will win.

   Empty and blank files (ie. those containing only white space) will not be created,
   i.e. processing will skip writing it without any question, warning or error.
   This allows you to easily create conditional files. This also means that you
   cannot create empty files this way. However, this "limitation" is easy to overcome
   as most file formats (ned, c++, ini, xml, etc) have a comment syntax, so you
   can just write a file that contains only a comment ("// file intentionally left blank").
   Alternatively, you can create an empty file using the FileUtils Java utility class
   (<@do FileUtils.createTextFile("empty.txt", "")!/>). Note: although blank files
   are not created, the template engine will not delete an existing file that
   happens to be already there with the same name.

   Typical usage: <@setoutput file=fileName?default("")/>, which means that if
   the fileName variable undefined (absent), use "" as file name, i.e. save to
   the original file name.

TODO document FileChooser and ExternalFileChooser, NedChooser: custom widget: file selector (workspace / external file)
TODO NED type chooser, project chooser

TODO setoutput path="..."

TODO mention http://www.jsonlint.com/


TODO document the <#assign dummy=expr!> trick! alternative: <@do FileUtil.deleteFile(file)!/>

TODO tutorial: XSWT, FTL, how to edit stuff; how to use Java classes (new widgets in XSWT,
JARs in the templates)

TODO pelda: "[] make new project dependent on this one" checkbox
TODO Util: hasMethod(), hasField(), instanceof(), newInstance()

TODO check: BeanWrapper cannot access inherited methods? (e.g. toString())

TODO: special markup in the template: "<?output="bubu.txt"> ... </?output>
to support creation of files with runtime-decided names.
TODO pelda: "Name of examples folder", "Name of source" folder

TODO document page.NN.class = org.foo.SomePageClass;
  class must extend ICustomWizardPage;
  class must have a public constructor with the following signature: (String name, IContentTemplate creatorTemplate, String condition)
  class must be accessible to the class loader (TODO clarify...)

