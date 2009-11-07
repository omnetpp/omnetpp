TODO: mire szolgal; kinek kellene ilyet csinalni es mi celbol...



Wizard templates are read from the templates/ folder of OMNeT++ projects.

In that folder, every subfolder that contains a template.properties file is
treated as a project template. (Other folders are ignored.)

When the template is used, the contents of the template folder (and subfolders)
will be copied over into the new project preserving the directory structure,
with the exception of template.properties and other known special-purpose files.
(It is also possible to specify other files and folders to ignore during copying.)

Files with the ".ftl" extension are regarded as templates, and they will undergo
template processing (and the ".ftl" extension gets chopped off) before copying.
Other files are copied verbatim.


TEMPLATE PROCESSING

Template processing uses the FreeMarker library (http://freemarker.org), and
all template syntax supported by FreeMarker can be used. Especially, variable
references of the syntax ${varName} will be replaced with the variable value.

The following variables are predefined:
  templateName          name of the template (see later)
  templateDescription   template description (see later)
  templateCategory      template category (see later)
  rawProjectName        project name, "as is"
  ProjectName           sanitized* project name with first letter capitalized
  projectname           sanitized project name in all lowercase
  PROJECTNAME           sanitized project name in all uppercase
     * sanitization means making the name suitable as a C/C++ identifier name
       (spaces, punctuation and other unfit chars replaced with underscore, etc.)
  date                  current date in yyyy-mm-dd format
  year                  year in yyyy format
  author                user name ("user.name" Java system property)
  licenseCode           license identifier for the @license NED property
  licenseText           copyright notice for the given license
  bannerComment         banner comment for source files; includes license text
  templateFolderName    name of the folder in which the template files are
  templateFolderPath    workspace path of the folder in which the template files are
  templateProject       name of the project that defines the template

Other variables may come from custom template pages, see later.


THE TEMPLATE.PROPERTIES FILE

The template.properties file is a standard Java property file (key=value syntax).
That file can be used to supply a template name, a template description,
and various other options.

Recognized property file keys:

  templateName  The template's display name; defaults to the folder name.
                This is the name that appears in the wizard's template selection
                page.
  templateDescription
                Description of the template. This may appear as a tooltip
                or in a description pane in the wizard.
  templateCategory
                Template category is used for organizing the templates into a
                tree in the wizard's template selection page. Defaults to the
                name of the project that provides the template.
  templateImage
                Name of the icon that appears with the name in the wizard's
                template selection page.
  addProjectReference
                True or false; defaults to true. If true, the template's project
                will be added to the referenced projects list of the new project.
  ignoreResources
                Comma-separated or JSON-syntax list of non-template files or
                folders; those files won't get copied over to the new project.
                Wildcards are NOT accepted. The template.properties file and
                custom wizard page files automatically count as nontemplates,
                so they don't have to be listed.
  optionalFiles Comma-separated or JSON-syntax list of files that should be
                suppressed (i.e. not created) if they would be empty after
                template processing. Wildcards are NOT accepted.
  sourceFolders
                Comma-separated or JSON-syntax list of C++ source folders
                to be created and configured. By default, none.
  nedSourceFolders
                Comma-separated or JSON-syntax list of NED source folders
                to be created and configured. By default, none.
  makemakeOptions
                Comma-separated list of items in the syntax "folder:options",
                or a JSON-syntax map of strings; it sets opp_makemake options
                for the given folders. There is no default.

There are additinal options for adding custom pages into the wizard, see below.


CUSTOM WIZARD PAGES

The following properties can be used to define custom pages in the wizard. <i> is
an integer page ID; their ordering defines the order of wizard pages.

  page.<i>.file
                The name of the XSWT file that describes the wizard page layout.
  page.<i>.title
                Title of the wizard page, displayed in the page's title area.
                Defaults to the template name.
  page.<i>.description
                Description of the wizard page, shown in the dialog's title area
                below the title. Defaults to text in the format "Page 1 of 3".
  page.<i>.condition
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
the ${name} syntax.

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


BINDING OF TEMPLATE VARIABLES TO WIDGETS

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


XSWT DATA BINDING

This is the way how values get transferred between widgets and template
variables (R=read, W=write):

  Button        This SWT class represents buttons, checkboxes and radio
                buttons, depending on its style attribute (SWT.PUSH,
                SWT.CHECK, SWT.RADIO).

                W: the string "true" selects the checkbox/radiobutton,
                   everything else clears it.
                R: returns a Boolean.

  Combo, CCombo Represent a combo box and a custom combo box. It can be
                made read-only (with the SWT.READ_ONLY style); a read-only
                combo only allows list selection but no manual editing.
                The list items can be specified from XSWT. Variables only
                work with the textedit part (cannot add/remove list items).

                W: the string value gets written into the combo. If the combo
                   is read-only and contains no such item, nothing happens.
                R: returns the currently selected item as string.

  DateTime      A widget for editing date/time.

                W: accepts a string in the following format: "yyyy-mm-dd hh:mm:ss".
                   If the string is not in the right format, an error occurs.
                R: returns a string in the same format, "yyyy-mm-dd hh:mm:ss".

  Label         Label widget (not interactive).

                W: sets the label to the string
                R: returns the label

  List          A listbox widget that allows selection of one or more items,
                depending on the style attribute (SWT.SINGLE or SWT.MULTI).
                List items can be specified from XSWT. Template variables only
                work with the selection (cannot add/remove list items).

                W: accepts a string with comma-separated items, and selects the
                   corresponding item(s) in the listbox. Items that are not
                   among the listbox items are ignored.
                R: Returns a string array object (String[]) that can be
                   iterated over in the template.

  Link          A label with hyperlink-like functionality; not very useful for
                our purposes. Handled similarly to Label.

  Scale         A graphical widget for selecting a numeric value.

                W: accepts strings with an integer value. Non-numeric strings
                   will cause an error (a message dialog will be displayed).
                R: returns an Integer which can be used in arithmetic
                   expressions in the template.

  Slider        A scrollbar-like widget for selecting a positive numeric value.
                Handled in a similar way as Scale.

  Spinner       Similar to a textedit, but contains little up and down arrows,
                and can be used to input an integer number.
                Handled in a similar way as Scale.

  StyledText    A textedit widget which allows displaying and editing of
                styled text. Handled similarly to Text.

  Text          A textedit widget. It can be single-line or multi-line,
                depending on the style attribute (SWT.SINGLE, SWT.MULTI).

                W: accepts a (potentially multi-line) string.
                R: returns the edited text as a string.

Table and tree widgets are currently not supported in a useful way, the main
reason being that SWT Tables and Trees are not editable by default.

Some non-interactive widgets which cannot be connected to template variables
but are useful in forms as structuring elements are: Composite, Group, Sash,
TabFolder/TabItem.


CONDITIONAL CUSTOM PAGES

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


USING CUSTOM WIDGET CLASSES

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


USING EXTERNAL JAVA CODE

Jar files placed into the plugins subdirectory of an OMNeT++ project will be
loaded automatically, and will be avalable for all templates. Jar files in
that directory may be plain Java jars and Eclipse plug-in jars. (The latter
makes it also possible to contribute new functionality into the IDE via
various extension points, but this is outside the scope of this discussion
about wizards.)

In addition, jar files placed in the lib/ subdirectory of a template will
be loaded automatically when the template is used, and the classes in it will
be available for that template. Custom SWT widget classes can be imported and
used in XSWT forms, and other code can be used in the template files via the
bean wrapper (e.g. ${classes["org.example.SomeClass"].someStaticMethod(...)},
see the example wizards.)


UTILITY CLASSES AVAILABLE FROM THE TEMPLATES

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


Math:

Represents the Java Math class, which contains mathematical functions.
See http://java.sun.com/j2se/1.5.0/docs/api/java/lang/Math.html

Math functions:

  double cos(double x);
  double sin(double x);
  double pow(double x, double y);
  etc.

FileUtils:

Contains utility functions for reading files in the following formats: XML,
JSON, CSV, property file, and functions to read and return a text file
as a single string, as an array of lines, and as a an array of string arrays
(where string arrays were created by splitting each by whitespace).

There are two sets of functions, one works on files in the Eclipse workspace,
and the other files on "external" files, i.e. files in the file system.
Files are interpreted in the Java platform's default encoding (unless XML files,
which specify their own encoding.)

FileUtils functions:

org.w3c.dom.Document readXMLFile(String fileName);
org.w3c.dom.Document readExternalXMLFile(String fileName);

    Parses an XML file, and return the Document object of the resulting
    DOM tree.

Object readJSONFile(String fileName);
Object readExternalJSONFile(String fileName);

    Parses a JSON file. The result is a Boolean, Integer, Double, String,
    List or Map, or any data structure composed of them.

String[][] readCSVFile(String fileName, boolean ignoreFirstLine, boolean ignoreBlankLines, boolean ignoreCommentLines);
String[][] readExternalCSVFile(String fileName, boolean ignoreFirstLine, boolean ignoreBlankLines, boolean ignoreCommentLines);

    Reads a CSV file. The result is an array of lines, where each line is
    a string array. Additional method parameters control whether to discard the
    first line of the file (which is usually a header line), whether to
    ignore blank lines, and whether to ignore comment lines (those starting
    with the # character). Comment lines are not part of the commonly accepted
    CSV format, but they are supported here nevertheless, due to their
    usefulness.

Properties readPropertyFile(String fileName);
Properties readExternalPropertyFile(String fileName);

    Parses a Java property file ('key=value' lines) in the workspace.
    The result is a Properties object, which is effectively a hash of
    key-value pairs.

String[][] readSpaceSeparatedTextFile(String fileName, boolean ignoreBlankLines, boolean ignoreCommentLines);
String[][] readExternalSpaceSeparatedTextFile(String fileName, boolean ignoreBlankLines, boolean ignoreCommentLines);

    Reads a text file, and return its contents, split by lines, and each line
    split by whitespace. Additional method parameters control whether to ignore
    blank lines and/or comment lines (those starting with the # character).
    The result is an array of lines, where each line is a string array of the
    items on the line.

String[] readLineOrientedTextFile(String fileName);
String[] readExternalLineOrientedTextFile(String fileName);

    Reads a text file in the workspace, and returns its lines. Comment lines
    (those starting with a hash mark, #) are discarded. The result is a
    string array.

String readTextFile(String fileName);
String readExternalTextFile(String fileName);

    Reads a text file, and return its contents unchanged as a single string.

boolean isValidWorkspacePath(String path);

    Returns true if the given string is syntactically a valid workspace path.

boolean isValidWorkspaceFilePath(String path);

    Returns true if the given string is syntactically a valid workspace file path.
    This function does not check whether the file exists, or whether the given path
    actually already points to a resource of a different type.

IProject asProject(String path);

    Returns the handle for the workspace project with the given name.
    Throws exception if the path is not a valid workspace project path.
    This function does not test whether the project exists. To test that,
    call the exists() method on the returned handle.

IContainer asContainer(String path);

    Returns the handle for the workspace container (i.e. project or folder) with the given name.
    Throws exception if the path is not a valid workspace container path.
    This function does not test whether the container exists. To test that,
    call the exists() method on the returned handle.

IFile asFile(String path);

    Returns the handle for the workspace file with the given name.
    Throws exception if the path is not a valid workspace file path.
    This function does not test whether the file exists. To test that,
    call the exists() method on the returned handle.


StringUtils

Represents the Apache Commons StringUtils class, which contains over a hundred
utility functions for manipulating strings.
See http://commons.apache.org/lang/api/org/apache/commons/lang/StringUtils.html

StringUtils methods

  boolean isEmpty(String s);
  boolean isBlank(String s);
  String capitalize(String s);
  String upperCase(String s);
  String lowerCase(String s);
  boolean startsWith(String s, String suffix);
  boolean endsWith(String s, String prefix);
  String[] split(String s);
  String join(String[] strings);
  etc.


CollectionUtils

Represents the Apache Commons CollectionUtils class, which contains some useful
functions for manipulating collections (like lists). Functions include computing
set union, intersection, and difference.
http://commons.apache.org/collections/apidocs/org/apache/commons/collections/CollectionUtils.html

CollectionUtils methods

Collection union(Collection a, Collection b);
Collection intersection(Collection a, Collection b);
Collection subtract(Collection a, Collection b);


IDEUtils

Provides entry points into various aspects of the IDE. This includes access to
the Eclipse workspace (projects, folders, files), and the NED index. The former
is documented in the Eclipse Platform help; documentation for the latter can
be found in the sources (Javadoc).
See http://help.eclipse.org/galileo/index.jsp?topic=/org.eclipse.platform.doc.isv/guide/resInt.htm
and http://help.eclipse.org/galileo/topic/org.eclipse.platform.doc.isv/reference/api/org/eclipse/core/resources/IWorkspaceRoot.html

IDEUtils methods

IWorkspaceRoot getWorkspaceRoot();
NEDResources getNEDResources();
MsgResources getMsgResources();


LangUtils

Provides a collection of Java language related utility functions.

LangUtils methods

boolean hasMethod(Object object, String methodName);

    Returns true if the object has a method with the given name.
    Method args and return type are not taken into account.
    Search is performed on the object's class and all super classes.

boolean hasField(Object object, String fieldName);

    Returns true if the object has a field with the given name.
    Field type is not taken into account. Search is performed on
    the object's class and all super classes.

boolean instanceOf(Object object, String classOrInterfaceName);

    Returns true if the given object is instance of (subclasses from or
    implements) the given class or interface. To simplify usage, the class
    or interface name is accepted both with and without the package name.

String toString(Object object);

    Produces a user-friendly representation of the object. In case of
    collections (lists, maps, etc), the representation is JSON-like.

List<Object> newList();

    Creates and returns a new mutable List object (currently ArrayList).

Map<Object, Object> newMap();

    Creates and returns a new mutable Map object (currently HashMap).

Set<Object> newSet();

    Creates and returns a new mutable Set object (currently HashSet).



REFERENCES

An XSWT tuturial and documentation can be found at:
http://www.coconut-palm-software.com/the_new_visual_editor/doku.php?id=xswt:home

Documentation for the FreeMarker template language:
http://freemarker.org/docs/index.html



---

TODO document: supportedWizardTypes = project, simulation, nedfile, network 
  and in the concrete template it comes back as wizardType, which can be checked against. together with optionalFiles
  e.g. don't generate package.ned if it's a simple NED File wizard

TODO "New NED File" wizard milyen valtozokat definial alapbol!
        "wizardType" (=="nedfile)
        "newNedFileName"  -- the file name
        "nedPackageName" -- expected NED package for that folder
        "nedTypeName" -- derived from the file name     
        "bannerComment" -- with copyright notice, etc


TODO document glob patterns in verbatimFiles etc

TODO: optionalFiles removed! now all files, if they would be empty, are NOT saved, (existing file, if exists,
 is left there. If you want to generate an empty file, put some comment into it (most files formats allow
 comments), or use FileUtils.writeTextFile() [note: it exists???]. If you want to delete an existing file,
 here's how you do it: FileUtils.deleteFile()

TODO document <@setoutput file=newNedFileName?default("")/>   (ures: eredeti file; multiple setoutputs: concatenate)
TODO document FileChooser and ExternalFileChooser: custom widget: file selector (workspace / external file)
TODO NED type chooser, project chooser

TODO mention http://www.jsonlint.com/

TODO tutorial: XSWT, FTL, how to edit stuff; how to use Java classes (new widgets in XSWT,
JARs in the templates)

TODO pelda: "[] make new project dependent on this one" checkbox
TODO Util: hasMethod(), hasField(), instanceof(), newInstance()

TODO check: BeanWrapper cannot access inherited methods? (e.g. toString())

TODO: special markup in the template: "<?output="bubu.txt"> ... </?output>
to support creation of files with runtime-decided names.
TODO pelda: "Name of examples folder", "Name of source" folder


