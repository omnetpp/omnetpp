= Dummy

== Writing Wizards

=== Motivation

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

Custom wizards are read from the `templates/` folder of OMNeT++ projects.
Wizards are implemented by mixing a templating engine (for generating
the output files) and a GUI description language (for custom wizard pages to
gather user input for the file generation). Because of the use of a 
templating engine, we'll also refer to custom wizards as "content templates".

In the `templates/` folder, every subfolder that contains a `template.properties`
file is treated as a content template. (Other folders are ignored.) Every
content template folder may contain several types of files: 

* The `template.properties` file contains general information about the wizard.
  Defines the initial value for the template variables, and specifies the 
  custom wizard pages to be shown when the wizard is run. 
* `*.ftl` files are template files that will be copied (without the `.ftl` extension)
  into the target folder after template variable substitution. 
* `*.xswt` files describe custom wizard pages to gather user input.  
* `*.fti` (template include) files are included by `*.ftl` 
  files. This can be used to factor out common parts from the template files.
*  `*.jar` files can be used to extend the wizard's functionality 
  with dynamically loaded Java code.
* All other files are regarded as files that have to be copied into the
  target folder verbatim when the wizard runs. The wizard folder may contain
  subdirectories which may also contain files of the above type (except
  `template.properties` of course).


The IDE offers several OMNeT++ related wizard dialogs: New OMNeT++ Project,
New NED File, New Simple Module, and so on. Every content template can
contribute to one or more of those wizard dialogs.

=== An Example Wizard

==== Configuring the Wizard

The first step when creating a wizard is to create a new folder
under the `templates` directory of the project. A file named `template.properties` 
must be present in the newly created directory. The file is used to configure 
the wizard. 

  templateName = New Test Wizard
  templateDescription = Generate an exampmle
  templateCategory = Test Wizards
  supportedWizardTypes = simulation, network

Specify at least the name, type and category for your wizard. Category is used to
specify how the wizards will be visually grouped. Wizard type specifies, in which
New ... Wizard  your new wizard will appear. You can specify one or more of the
following types: `project, simulation, nedfile, inifile, msgfile, wizard, 
simplemodue, compoundmodule, network, import, export` 

You can decide now what data you would like to gather from the user. Define 
template variables and their values as key-value pairs:

  nodeName = Dummy
  networkSize = 6
 
The wizard should have a custom page, where the user can specify the value of the above variables.

  page.1.file = parameters.xswt
  page.1.title = Network parameters 
 
We will use the file `parameters.xswt` to define the layout and the content of the new wizard page.

NOTE: There are numerous other configuration keys that can be used in `template.properties`. See the
"Configuration Keys" section for an exhaustive list of options. 
 
==== Creating a new Wizard Page

Files with `.xswt` extension (Wizard Page definitions) are used to define the UI and
add new wizard pages to gather user input for the template generation. In the previous
section we have specified that the file called `parameters.xswt` will contain the new
wizard page definition. We will add a 'spinner' control to specify the size of our network 
and a 'text' control to specify the node type:

  <xswt xmlns:x="http://sweet_swt.sf.net/xswt">
	  <import xmlns="http://sweet_swt.sf.net/xswt">
	    <!-- Import all widgets and utility classes  -->
	    <package name="java.lang"/>
	    <package name="org.eclipse.swt.widgets" />
	    <package name="org.eclipse.swt.graphics" />
	    <package name="org.eclipse.swt.layout" />
	    <package name="org.eclipse.swt.custom" />
	    <package name="org.omnetpp.common.wizard.support" />
	    <package name="org.omnetpp.ned.editor.wizards.support" />
	    <package name="org.omnetpp.cdt.wizard.support" />
	  </import>
	  <layout x:class="GridLayout" numColumns="2"/>
	  <x:children>
	    <label text="Number nodes in the network:"/>
	    <spinner x:id="networkSize" minimum="2" x:style="BORDER"/>
	    <label text="Type of Nodes:"/>
	    <text x:id="nodeName" x:style="BORDER"/>
	  </x:children>
  </xswt>

The above page will have two columns. The first column contains a label,
while the second contains an editable control.
Please note the `x:id="varName"` attributes in the spinner and text control 
definitions. We can bind a template variable to a control using the `id` attribute.
When the user finishes with the wizard page, the content of the controls will be 
automatically copied to the specified template variables.   


==== Creating Templated Files

When the template is used, the contents of the template folder (and subfolders)
will be copied over into the new project preserving the directory structure,
with the exception of `template.properties` and other known special-purpose files.
(It is also possible to specify other files and folders to ignore during copying.)

When the wizard is being used, a pool of variables is kept by the wizard dialog.
These variables are initialized from the `key = value` lines in the 
`template.properties` files; they can get displayed and/or edited
on custom wizard pages; and eventually they get substituted into `*.ftl` files
(using the $\{varname} syntax).

Some variables have special meaning and are interpreted by the wizard dialog
(e.g. the `nedSrcFolders` variable determines which folders get denoted as
NED Source Folders by the New OMNeT++ Project wizard). Variables can be used
to generate output file names, can be used as input file names, and can serve
as input and working variables for arbitrarily complex algorithms programmed
in the template (`*.ftl`) files.

Create a file named `untitled.ned.flt`. 

	<@setoutput path=targetFileName?default("")/>
	${bannerComment}
	
	<#if nedPackageName!="">package ${nedPackageName};</#if>
	
	network ${targetTypeName}
	{
		node[${networkSize}] : ${nodeName}
	}

The file name for the output will be the value of the `targetFileName` variable.
The rest of template variables will be substituted into the template. 

Specific wizard dialogs will also define extra variables for use in the
templates, e.g. the wizard type that creates a simulation with all required files
 will put the `simulationName` variable into the context. To see all defined variables, 
check the Appendix.

TIP: The "New Wizard" wizard in the IDE provides you with more than a handful of
     working examples, useful utilities for writing wizards, sample code for
     accessing various features, and so on. The aim of these wizards is to get you
     productive in the shortest time possible.


=== Wizard Types

The wizard will set the `wizardType` template variable when it executes,
so template code can check under which wizard type it runs (using `<#if>..</#if>`), and
act accordingly. This feature allows one to create templates that 
can be used for multiple wizard types. 

Create a file called `omnetpp.ini.ftl` and for our example, fill with:	

	<#if wizardType=="simulation">
	network = ${targetTypeName}
	</#if>

We need the INI file only if we are creating a simulation. If the 
current type is not 'simulation', an empty file will be generated
and it will not be copied to the destination folder. 
	
There are several types of wizards you can create. Each one has a different
goal:

===== New Project Wizards

Project wizards can create -- as their name suggest -- new projects. All necessary files
required by the new project can be included in the template. It is possible to adjust 
project properties to customize the new project. You can enable C++ code support, set 
source and NED folders. The files in the template folder will be directly copied to the
new project folder.   

===== New Simulation Wizards

A new simulation is basically a network definition plus an INI file 
describing the initial configurations and parameter values. These 
files are typically created in a separate directory that is selected 
in the wizard as the 'simulation folder'. 

===== New INI,MSG or NED File Wizards

New ... File Wizards generate only a single file. The filename can be accessed in the 
\${targetFileName} and the target folder as \${targetFolder}.  

===== Export / Import Wizards 

Export and import wizards work similarly like the other wizards, but the input 
and output files must be specified on custom wizard pages.

=== Configuration Keys

The `template.properties` file is a standard Java property file (`key = value` syntax).
That file can be used to supply a template name, a template description,
and various other options.

Recognized property file keys:

templateName::  
				The template's display name; defaults to the folder name.
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
                this template supports. If not specified, the wizard will support
                all known wizard types.
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

There are additional options for adding custom pages into the wizard, as described
in the next section.

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

TIP: Documentation for the FreeMarker template language can be found at:
     http://freemarker.org/docs/index.html

Several variables are predefined, such as `templateName, targetFolder, date,
author`; others like `targetFileName, targetTypeName, simulationFolderName,
nedPackageName`, etc. are defined only for certain wizard dialogs.
A full list of variables is provided in the Appendix.


==== Generating multiple files 

By default templates are processed and copied with the same name (chopping the .ftl
extension), but it is possible to redirect the output of the template to a different 
file using the `<@setoutput path=.../>` macro. The filename can contain slashes too, i.e.
one can write files in a different folder. If the folder does not exist, it will
be created.

If filename is empty, the directive restores output to the original file name (the template's name).
This also works if a folder name is given and only the file name is missing
(`<@setoutput path="src/">`): then it will write the file with the original name
but into the specified folder.

NOTE: If there are multiple setoutput's with the same file name within a template,
the content will be concatenated. Concatenation only works within one template;
if you have multiple templates writing into the same file, they will overwrite
each other's content, and it is undefined which one will win.

==== Conditional file creation

Empty and blank files (ie. those containing only white space) will not be created,
i.e. processing will skip writing it without any question, warning or error.
This allows you to easily create conditional files. This also means that you
cannot create empty files this way. However, this "limitation" is easy to overcome
as most file formats (ned, c++, ini, xml, etc) have a comment syntax, so you
can just write a file that contains only a comment ("// file intentionally left blank").
Alternatively, you can create an empty file using the FileUtils Java utility class
(`<@do FileUtils.createFile("empty.txt", "")!/>`). 
   
NOTE: Although blank files are not created, the template engine will not delete an
      existing file that happens to be already there with the same name.

TIP: A typical usage: `<@setoutput path=fileName?default("")/>`, which means that if
     the fileName variable undefined (absent), use "" as file name, i.e. save to
     the original file name.


=== Custom Wizard Pages

The following properties can be used to define custom pages in the wizard. `<i>` is
an integer page ID (starting with 1); their ordering defines the order of wizard 
pages.

page.<i>.file::
                The name of the XSWT file that describes the wizard page layout.
page.<i>.class::
                In addition to XSWT files, custom Java pages may also be defined 
                in Java code. This can be useful when the wizard page would be 
                too complex to describe with XSWT, would need to have significant 
                active behavior, or simply the wizard page code already exists 
                in Java form. See below for further discussion about custom pages.
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
                `${` and `}` marks: the string you enter will be substituted into
                "`${( <condition> )?string}`" string (will replace "`<condition>`"),
                and evaluate as such. An example will be provided later.

All property file entries are available as template variables too. Also, most
property values may refer to other property values or template variables, using
the $\{name} syntax.

NOTE: Custom wizard pages are defined in XSWT (http://xswt.sourceforge.net,
      http://www.coconut-palm-software.com/the_new_visual_editor/doku.php?id=xswt:home).

NOTE: Currently we use XSWT 1.1.2. Newer XSWT integration builds from
      http://www.coconut-palm-software.com/~xswt/integration/plugins/
      did not work out well.


=== Conditional Custom Wizard Pages

Now that templating and XSWT were covered, we can revisit how one can use page
conditions. Consider the following practical example:

Suppose a wizard for wireless networks. On the first page of the wizard there
is a "[] configure routing" checkbox with the ID "wantRouting". If this
checkbox gets selected, you want to display a second page where the user can
select a routing protocol, then further configuration pages depending on
the particular routing protocol the user chose. To achieve this, you would
add the following lines to template.properties:

  # page with the "wantRouting" checkbox
  page.1.title = General
  # page with the "protocol" combobox
  page.2.title = Choose Routing Protocol
  page.3.title = AODV Options
  page.4.title = DSDV Options

  page.2.condition = wantRouting
  page.3.condition = wantRouting && protocol=="AODV"
  page.4.condition = wantRouting && protocol=="DSDV"

You get the idea. This allows the creation of conditional pages. 


==== Standard SWT widgets

TIP: The SWT controls are documented on the Eclipse web site. See:
http://help.eclipse.org/galileo/topic/org.eclipse.platform.doc.isv/reference/api/org/eclipse/swt/widgets/package-summary.html

It is possible to bind template variables to a specific control by using the `x:id` 
attribute.
  <text x:id="templateVariableName" />
 
This is the way how template variables are bound to the controls (R=read, W=write):

Button::        This SWT class represents buttons, checkboxes and radio
                buttons, depending on its style attribute (`SWT.PUSH,
                SWT.CHECK, SWT.RADIO`).
                * W: the string "true" selects the checkbox/radiobutton,
                   everything else clears it.
                * R: returns a Boolean.

Combo, CCombo:: Represent a combo box and a custom combo box. It can be
                made read-only (with the `SWT.READ_ONLY` style); a read-only
                combo allows list selection but no manual editing.
                The list items can be specified from XSWT. Variables only
                work with the textedit part (cannot add/remove list items).
                * W: the string value gets written into the combo. If the combo
                   is read-only and contains no such item, nothing happens.
                * R: returns the currently selected item as string.

DateTime::      A widget for editing date/time.
                * W: accepts a string in the following format: "yyyy-mm-dd hh:mm:ss".
                   If the string is not in the right format, an error occurs.
                * R: returns a string in the same format, "yyyy-mm-dd hh:mm:ss".

Label::         Label widget (not interactive).
                * W: sets the label to the string
                * R: returns the label

List::          A listbox widget that allows selection of one or more items,
                depending on the style attribute (`SWT.SINGLE` or `SWT.MULTI`).
                List items can be specified from XSWT. Template variables only
                work with the selection (cannot add/remove list items).
                * W: accepts a string with comma-separated items, and selects the
                   corresponding item(s) in the listbox. Items that are not
                   among the listbox items are ignored.
                * R: Returns a string array object (String[]) that can be
                   iterated over in the template.

Scale::         A graphical widget for selecting a numeric value.
                * W: accepts strings with an integer value. Non-numeric strings
                   will cause an error (a message dialog will be displayed).
                * R: returns an Integer which can be used in arithmetic
                   expressions in the template.

Slider::        A scrollbar-like widget for selecting a positive numeric value.
                Handled in a similar way as Scale.

Spinner::       Similar to a textedit, but contains little up and down arrows,
                and can be used to input an integer number.
                Handled in a similar way as Scale.

StyledText::    A textedit widget which allows displaying and editing of
                styled text. Handled similarly to Text.

Text::          A textedit widget. It can be single-line or multi-line,
                depending on the style attribute (`SWT.SINGLE, SWT.MULTI`).
                * W: accepts a (potentially multi-line) string.
                * R: returns the edited text as a string.

==== Custom widgets

HttpLink::      A control containing a text and a hyperlink between `<a></a>` tags. An URL can 
                be specified to be opened in an external browser.
                * W: accepts a string with the target URL. 
                * R: returns the target URL as string.
                * ATTR: text : the textual content of the control `<a></a>` denotes the link inside.
                * ATTR: URL : the target URL where the control points to 

InfoLink::      A control for which displays a text with embedded link(s), and clicking
                on a link will display a hover text in a window. The hover text can be given 
                the with setHoverText method (i.e. the hoverText XSWT attribute), or bound 
                to a template variable (using the x:id XSWT attribute).
                * W: accepts a string with the hover text for the control. 
                * R: returns the hover text as string.
                * ATTR: text : the content of the control
                * ATTR: hoverText : the html formatted text displayed in the hover control 
                * ATTR: hoverMinimumWidth : the minimal width for the hover control
                * ATTR: hoverMinimumHeight : the minimal height for the hover control
                * ATTR: hoverPreferredWidth : the preferred width for the hover control
                * ATTR: hoverPreferredHeight : the preferred height for the hover control

FileLink::      A control for displaying the name of a resource as a link. When clicked, it shows
                the resource (opens Project Explorer and focuses it to the resource).
                * W: accepts a string with the workspace path of the resource to be shown. 
                * R: returns the full workspace path of the resource.
                * ATTR: resourcePath : the full workspace path of the file  

FileChooser::   A control for selecting a file from the workspace. Implemented as a Composite with 
                a single-line Text and a Browse button.
                * W: accepts a string with the workspace file name. 
                * R: returns the name of the selected file as a string from the workspace.
                * ATTR: fileName : the full workspace path of the selected file.

ExternalFileChooser::
                A control for selecting a file from the filesystem. Implemented as a Composite with 
                a single-line Text and a Browse button.
                * W: accepts a string with the full file name. 
                * R: returns the name of the selected file as a string from the filesystem.
                * ATTR: fileName : the full filesystem path of the selected file.

GateChooser::   A control for selecting a gate of a NED module type. If the module
                exists, it lets the user select one of its gates from a combo;
                if it doesn't, it lets the user enter a gate name.
                * W: accepts strings with a gate name. 
                * R: returns the name of the selected gate as a string.
                * ATTR: gateName : the name of the selected gate
                * ATTR: nedTypeName : the NED type whose gates should be enumerated.
                * ATTR: gateFilter : type filter for the enumerated gates. Expects a
                  binary OR (|) of the following values: `GateChooser.INPUT, 
                  GateChooser.OUTPUT, GateChooser.INOUT, GateChooser.VECTOR, 
                  GateChooser.SCALAR`.

  <gateChooser x:id="gateName" 
    nedTypeName="${nodeType}" 
    gateFilter="GateChooser.INOUT|GateChooser.VECTOR"/>
                
NedTypeChooser:: 
				A control for selecting a NED module type. An existing type name can be selected
				or a new one can be entered.
                * W: accepts strings with a ned type name. 
                * R: returns the name of the selected ned type as a string.
                * ATTR: nedName : the NED module type as a string
                * ATTR: acceptedTypes : filter for the enumeration of types. Expects a
                  binary OR (|) of the following values: `NedTypeChooser.MODULE, 
                NedTypeChooser.SIMPLE_MODULE, NedTypeChooser.COMPOUND_MODULE, 
                NedTypeChooser.MODULEINTERFACE, NedTypeChooser.CHANNEL, 
                NedTypeChooser.CHANNELINTERFACE, NedTypeChooser.NETWORK`.

  <nedTypeChooser x:id="channelType" 
       acceptedTypes="NedTypeChooser.CHANNEL"/>

NOTE: Table and tree widgets are currently not supported in a useful way, the main
reason being that SWT Tables and Trees are not editable by default.

Some non-interactive widgets which cannot be connected to template variables
but are useful in forms as structuring elements: 
Composite:: Used to group two or more controls into a single one.

  <composite>
    <layoutData x:class="GridData" horizontalSpan="2"/>
    <layout x:class="GridLayout"/>
    <x:children>
      <button x:id="dynamic" text="Dynamic" x:style="RADIO"/>
      <button x:id="static" text="Static" x:style="RADIO"/>
    </x:children>
  </composite>

Group::  Used to group the controls with a visual heading.

  <group text="Heading text">
    <x:children>
       <label text="Control 1"/>
       <label text="Control 2"/>
    </x:children>
  </group>

TabFolder/TabItem:: Can be used to group the controls into separate pages.

  <tabFolder>
    <x:children>
      <composite x:id="page1"></composite>
      <composite x:id="page2"></composite>
      <tabItem text="Tab 1" control="page1"/>
      <tabItem text="Tab 2" control="page2"/>
    </x:children>
  </tabFolder>

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
text widget with `id="numNodes"` and a checkbox with `id="generateTraffic"`, then
the following lines

  numNodes = 100
  generateTraffic = true

will fill in the text widget with "100" and select the checkbox. Widgets that
do not have such lines in the property file will be left alone.

Compound data structures (arrays, maps, and any combination of them) can be
specified in the JSON syntax (http://json.org). They can be iterated over
in the templates, and can be used as input/output for custom compound widgets.
Examples:

  apps = ["ftp", "smtp", "news"]
  layers = {"datalink":"ieee80211", "network":"ip", "transport":["tcp","udp"]}

If you get an error complaining about syntax errors in a JSON entry, the
http://jsonlint.com website can help you locate the problem.

The property file takes precedence over values in the XSWT file.

==== XSWT Form Creation

Entities and attributes in an XSWT file are directly mapped to the corresponding
SWT controls and their properties. For example the `<styledText>` tag is mapped 
to the StyledText SWT control. Similarly the 'text' attribute is mapped to the 
`text` property of the StyledText control. It is also possible to call any public 
method on the control by embedding a "call" as a child tag:

  <styledtext text="Hello world!">
  	  <setFocus/>
  </styledText>

Constants in controls declared `public final` can be used in an XSWT file by appending
the java class name before them:

  <gateChooser gateFilter="GateChooser.INOUT|GateChooser.VECTOR"/>

Constants in the SWT class do not need the `SWT.` prefix. You can use:

  <button x:style="RADIO"/>

Children can be added to a compound control inside the `<x:children></x:children>` tags.

  <group text="Hello">
    <x:children>
      <label text="Message 1" />
      <label text="Message 2" />
    </x:children>
  </group> 

Layout data can also be added as a new tag inside a control element:

  <text x:id="numServers" x:style="BORDER">
    <layoutData x:class="GridData" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
  </text>

TIP: An XSWT tutorial and documentation can be found at:
http://www.coconut-palm-software.com/the_new_visual_editor/doku.php?id=xswt:home


==== Conditional Controls

It is also possible to create controls conditionally.
To overcome the limitation that XSWT page descriptions are completely static,
XSWT files undergo FreeMarker template processing before giving them to the
XSWT engine for instantiation. This template processing occurs right before
the page gets displayed, so data entered on previous pages can also be
used as input for generating XSWT source. This feature can be useful to make
conditional widgets (i.e. using `<#if>` to make part of the page appear only
when a certain option has been activated on earlier pages); to create a
previously unknown number of widgets (using a `<#list>..</#list>` loop);
to populate combo boxes, listboxes or other widgets with options; and more.
If the user navigates in the wizard back and forth several times (using the
Next and Back buttons), the contents of wizard pages are always re-created
with using the current values of template variables just before getting
displayed, so they will always be up to date.
    

=== Extending the Wizards in Java

==== Creating Wizard Pages

Defining a wizard page in Java requires that you install the Eclipse PDE 
(Plug-in Development Environment), and that you have some Eclipse development skills.

The `template.properties` key for denoting a Java-based wizard page is `page.<NN>.class`,
and the value should be the fully qualified name of the Java class that implements
the wizard page. The requirements for the class are:
  - the class must be accessible to the class loader
  - the class must extend `org.omnetpp.common.wizard.ICustomWizardPage;`
  - the class must have a public constructor with the following argument list:
    (`String name, IContentTemplate creatorTemplate, String condition`)

==== Creating Custom Widgets

Since XSWT works via Java reflection, your own custom widgets can be used
the forms very much like normal SWT widgets. No glue or registration code
is needed, just add their package to the `<import>` tags at the top of the XSWT
file.

However, some Java code is needed so that the wizard knows how to write
template variables into your widgets and how to extract them after editing.
This functionality can be added via the `org.omnetpp.common.wizard.IWidgetAdapter`
interface. This interface must be implemented either by the widget class
itself, or by a class named `<widgetclass>Adapter` in the same package.
The interface has methods to tell whether the adapter supports a given widget,
to read the value out of the widget, and to write a value into the widget.

In addition to basic data types (`Boolean, Integer, Double, String, etc`),
it is possible to use compound data types as well, i.e. those composed of
the List and Map interfaces of the Java Collections API. The default values
can be given in the template.properties file in the JSON notation, and the
result can be used in the templates (iteration via `<#list>`, etc).

Jar files placed into the plugins/ subdirectory of an OMNeT++ project will be
loaded automatically, and will be available for all templates. Jar files in
that directory may be plain Java jars and Eclipse plug-in jars. (The latter
makes it also possible to contribute new functionality into the IDE via
various extension points, but this is outside the scope of this discussion
about wizards.)

Jar files placed in the folder of the template will
be loaded automatically when the template is used, and the classes in it will
be available for that template. Custom SWT widget classes can be imported and
used in XSWT forms, and other code can be used in the template files via the
bean wrapper (e.g. `${classes["org.example.SomeClass"].someStaticMethod(...)}`,
see the example wizards.) Like .xswt files and `template.properties`, jar files
are not copied over into the destination folder when the wizard executes.

==== Extending the Template Processing

If you are skilled in writing Eclipse plug-ins, there are ways you can extend
content templates. One is to contribute to the `org.omnetpp.common.wizard.templatecontributor` 
extension point, which lets you supply `IContentTemplateContributor` objects
that can extend the content template implementation in various ways. You can 
contribute new variables, functions or macros to the template context.


=== Common Pitfalls

 * Variables need to be defined. Referring to an undefined variable is an
   error in FreeMarker, i.e. it does not return empty string as in bash or
   in makefiles.

 * Default values should be given in template.properties! You need to resist
   the temptation to define them in the XSWT page by pre-filling the corresponding
   widget (e.g. `<text x:id="n" text="100">`). If you specify the value in a page,
   the assignment will not take effect if the user skips that page (i.e. clicks
   Finish earlier). That causes variable to remain undefined, resulting in a
   runtime error during template processing.

 * Type mismatch. Variables have types in FreeMarker, and one can get type conversion
   errors if the templates are not programmed carefully; for example, comparing a number
   and a string is a runtime error. Worse, widgets in wizard pages may implicitly
   perform type conversion. For example, a numHosts=100 line in template.properties
   defines a number, but if you have a `<text x:id="numHosts"/>` widget in the form,
   the variable will come back from it as a string. Even worse, whether the
   number->string conversion takes place will depend on whether the page gets
   displayed in the wizard session or not. Therefore, it is recommended that
   you explicitly convert numeric variables to numbers at the top of templates,
   for example like this: `<#assign numHosts = numHosts?number>`

 * For some reason, FreeMarker refuses to print boolean variables, i.e. `${isFoo}`
   results in a runtime error. The common workaround is to write
   `<#if isFoo>true<#else>false</#if>`; this can be shortened with our iif() function:
   `${isFoo, "true", "false"}`.

 * Many string operations are available both as builtin FreeMarker operators
   (varname?trim) and as Java methods via  FreeMarkers's BeanWrapper (`varname.trim()`).
   If you are mixing the two, it is possible that you'll start getting
   spurious errors for the Java method calls. In that case, simply change
   Java method calls to FreeMarker builtins, and all will be well.

 * Some Java functionality (the instanceof operator, `Class.newInstance()`, etc)
   cannot be accessed via BeanWrapper. If you hit such a limitation, check
   our LangUtils class that provides FreeMarker-callable static methods
   to plug these holes.

=== XSWT Tips and Tricks:

* How can I make a checkbox or radio button? `<checkbox>` and `<radio>` are not recognized in my XSWT files!

They are called `<button x:style="CHECK">` and `<button x:style="RADIO">` in SWT.

* My text fields, combo boxes etc look strange, what do I do wrong?

You usually want to add the BORDER option, like this: `<text x:style="BORDER">`

* How to make a long label wrap nicely?

You will find that specifying x:style="WRAP" is necessary, but not enough. It is
also needed that the label widget expands and fills the space horizontally:
      
      <label text="Some long text...." x:style="WRAP">
          <layoutData x:class="GridData" horizontalAlignment="FILL" 
             grabExcessHorizontalSpace="true"/>
      </label>

* How to set the focus?

Add `<setFocus/>` to the XML body of the desired widget.

* How to make widgets conditional on some previous input?

You can use `<#if>` and other FreeMarker directives in XSWT files. These
files undergo template processing each time the corresponding page
appears.

* How to carry forward data from a previous page to the next?

Use FreeMarker variables (`${varName}`) in the page.

* How can I fill a combo box with values that I'll only know at runtime?

You can generate the `<option>` children of the combo using FreeMarker
directives, e.g. `<#list>...</#list>`

* How can I have some more sophisticated user input than simple textedit fields and checkboxes?

You can implement custom SWT controls in Java, and use them in the
wizard pages. The custom controls may even be packaged into jar files
in the template's directory, i.e. you do not need to write a separate
Eclipse plug-in or something. Have a look at the source files of the
existing custom controls (`FileChooser, NedTypeChooser, InfoLink`, etc).

* How to dynamically enable/disable controls on a page, depending on other controls (i.e. a checkbox or radio button)

Currently you cannot. If you are desperate, you have the following options:
(1) put the dependent controls onto a separate page, which you can make
conditional; (2) write a custom CheckboxComposite control in Java
that features a checkbox, and enables/disables child controls
when the checkbox selection changes; (3) write the full custom wizard
page entirely in Java, and register it in `template.properties` with
page.xx.class= instead of page.xx.file; (4) implement scripting support
for XSWT 1.x and contribute the patch to us.

* In the Project wizard, how does it get decided which templates get offered if the "with C++ checkbox" gets selected or not selected on the first page?

If the C++ support checkbox is cleared, templates that require 
C++ support will not appear; when it is checked, there is no
such filtering. A template is regarded as one that requires C++ support 
if the template.properties file contains any of the following:
`sourceFolders=, makemakeOptions=, or requiresCPlusPlus=true`.

=== Using the IDE

==== Editing XSWT Files

Double-clicking on an XSWT file will open it in the XSWT editor. The editor
provides basic syntax highlighting. An extremely
useful feature of the IDE is the XSWT Preview, where you can preview
the form being edited (it updates when you save the file). The Preview should
open automatically when you open the XSWT file; if it does not (or you close it),
you can access it via the *Window|Show View...* menu item.

Some (custom) widgets may not appear in the Preview; this is because the
Preview does not load jar files from the projects.

==== Editing Template Files

The Freemarker Editor is opened when you double-click files with the *.ftl or
*.fti extension. (The latter stands for Freemarker Template Include, and it is
intended for template fragments that you include into other templates. Otherwise
the wizard ignores fti files, i.e. does not copy them into the new project or
folder.) The Freemarker Editor offers basic syntax highlight, validation
(error markers appear during editing if the template is not syntactically
correct), and basic content assist. Content assist can help you with directives
(`<#...>` syntax) and builtin operations (like `?number, ?size, ?default`, etc).
The content assist popup appears automatically when you type '<#' (actually
a closing '>' is also needed for the editor to recognize the tag), and
when you hit '?' within a directive or an interpolation (`${...}`).


=== Append. A: Predefined Template Variables

[cols="30%,^10%,^10%,^10%,^10%,^10%,^10%,^10%^"]
|==============
|variable name | project | simul. | msgfile | inifile | nedfile | wizard | export
|`addProjectReference`  | X |   |   |   |   |   |
|`author`  				| X | X | X | X | X | X | X
|`date`                 | X | X | X | X | X | X | X
|`licenseCode`    		| X | X | X | X | X | X | X
|`licenseText`    		| X | X | X | X | X | X | X
|`makemakeOptions`     	| X |   |   |   |   |   |
|`msgTypeName`     	    |   |   | X |   |   |   |
|`namespaceName`       	|   | X | X |   | X |   |
|`nedPackageName`       | X | X |   | X | X |   |
|`nedSourceFolders`    	| X |   |   |   |   |   |
|`newWizardName`     	|   |   |   |   |   | X |
|`newWizardProject`     |   |   |   |   |   | X |
|`projectName`    		| X | X | X | X | X | X |
|`PROJECTNAME`    		| X | X | X | X | X | X |
|`projectname`    		| X | X | X | X | X | X |
|`rawProjectName`    	| X | X | X | X | X | X | 
|`requiresCPlusPlus`    | X |   |   |   |   |   |
|`simulationFolderName` |   | X |   |   |   |   |
|`simulationName`    	|   | X |   |   |   |   |
|`sourceFolders`     	| X |   |   |   |   |   |
|`targetFileName`       |   |   | X | X | X |   |
|`targetFolder`    		| X | X | X | X | X | X |
|`targetMainFile`       | X | X | X | X | X |   |
|`targetTypeName`       | X | X | X | X | X |   |
|`templateCategory`    	| X | X | X | X | X | X | X
|`templateDescription`  | X | X | X | X | X | X | X
|`templateFolderName`   | X | X | X | X | X | X | X
|`templateFolderPath`   | X | X | X | X | X | X | X
|`templateName`    		| X | X | X | X | X | X | X
|`templateProject`    	| X | X | X | X | X | X | X
|`templateURL`    		| X | X | X | X | X | X | X
|`withCplusplusSupport` | X |   |   |   |   |   |
|`wizardType`    		| X | X | X | X | X | X | X
|`year` 				| X | X | X | X | X | X | X
|==============

==== General

In the following sections we describe the individual wizard types and their supported
template variables. Variables will be marked with one or more letter to show in 
which wizard types they are supported as shown on in the previous table  

* A: supported in all wizards 
* P: project
* S: simulation
* M: messagefile 
* I: inifile
* N: nedfile, simplemodule, compoundmodule, network 
* W: wizard

If the variable is marked as `R/O` (read-only), you should not modify its value
in your wizard pages. Modification of read-only values may result in inconsystent 
output files.


date (A)::  current date in yyyy-mm-dd format
year (A)::  year in yyyy format
author (A)::  user name ("user.name" Java system property)
licenseCode (A)::  license identifier for the @license NED property
licenseText (A)::  copyright notice for the given license
bannerComment (A)::  banner comment for source files; includes license text

==== Template information

wizardType (A):: the type of the wizard. Any of the following:
                 project, simulation, nedfile, inifile, messagefile,
                 simplemodule, compoundmodule, network, wizard
templateName (A):: name of the template
templateDescription (A):: template description
templateCategory (A):: template category
templateURL (A):: the URL, the template was loaded from (only for built-in and 
                  other URL-based wizards)


The following variables are only defined if the template was loaded from the workspace
(i.e. a project's `templates/` subdirectory):

templateFolderName (A)::  name of the folder (without path) in which the template files are
templateFolderPath (A)::  full workspace path of the folder in which the template files are
templateProject (A)::  name of the project that defines the template

==== File name related variables

targetFolder (A) (R/O):: the project or folder path in which the project will generate files.
   For project wizards, this holds the name of the project being created; for
   file wizards, it holds the name of the folder in which the file will be created;
   for simulation wizard, it holds the name of the folder where files will be created.
targetFileName (N,I,M) (R/O):: the name of the new file to be created 
targetTypeName (P,S,N,I,M):: a typename that can be used as the main 'type' for the resulting code.
                         (for P = `${projectName}`, for S = `${simulationName}`, for N,I,M = `${targetFileName}`)
targetMainFile (P,S,N,I,M):: a file name that can be used as the 'main' output file for the template 
                         (for P,S = `${targetTypeName}.ned`, for N,I,M = `${targetFileName}`)

==== Project name related variables

rawProjectName (A):: the project name, "as is"
projectName (A)::  sanitized project name with first letter capitalized
projectname (A)::  sanitized project name in all lowercase
PROJECTNAME (A)::  sanitized project name in all uppercase

Sanitization means making the name suitable as a NED or C/C++ identifier
(spaces, punctuation and other unfit chars replaced with underscore, etc.)

==== C++ project control

addProjectReference (P):: if true, make project as dependent on this one
sourceFolders (P):: source folders to be created and configured
makemakeOptions (P):: makemake options, as "folder1:options1,folder2:options2,..."
withCplusplusSupport (P):: whether the project supports C++ code compilation 
requiresCPlusPlus (P):: if true, the wizard requires the "support C++ option" during the project creation
namespaceName (S,N,M):: the namespace where C++ classes should be placed 

==== NED files and message files
 
nedSourceFolders (P):: NED source folders to be created and configured
nedPackageName (P,S,N,I):: (P: $\{projectname}, S,N,I:autocalculated)
msgTypeName (M):: the name of the message class

==== Variables specific to New Simulation wizards

simulationFolderName (S) (R/O):: the folder where the simulation will be created
simulationName (S) (R/O):: the name of the simulation

==== Variables for New Wizard generation

newWizardName (W) (R/O):: the name of the new wizard to be created
newWizardProject (W):: the project where the new wizard will be created

==== Miscellaneus

The variables below are just for advanced use only. They can be used to access
directly all known NED and message types, static classes for utility functions and 
the whole context used during template processing. 

creationContext (A):: The template evaluation context. Provided for low level access.
classes (A)::         Access to class static models. It is possible to access 
                      class static methods via this variable. 
nedResources (A):: all currently known NED types
msgResources (A):: all currently known message types

NOTE: In addition to the above variables, all keys found in the template.properties file
are added automatically to the context as a template variable.



=== Append. B: Functions, Classes and Macros available from Templates


	
	iif(condition, valueIfTrue, valueIfFalse)
	
Inline if. The FreeMarker language does not have a conditional operator
(like ?: of C/C++ ), but the iif() function can save you from the verbosity
of having to spell out <#if>..<#else>..</#if> everywhere such thing is needed.
Note that unlike in C/C++ the evaluation is not lazy, i.e. both the "then" and
the "else" expressions are always evaluated.

   <@do expression !>::
   
FreeMarker does not have a construct for calling a function and then discarding
the result. One could use <#assign dummy = expression>, but this, apart from
being ugly, will fail if the called (Java) function is void or returns null.
We recommend our small "@do" macro which takes one argument and does nothing,
and the exclamation mark (the FreeMarker default value operator) cures the
void/null problem.

The following Java classes are available during template processing:

  Math::		    java.lang.Math
  StringUtils::     org.apache.commons.lang.StringUtils
  CollectionUtils::	org.apache.commons.lang.CollectionUtils
  WordUtils::       org.apache.commons.lang.CollectionUtils
  FileUtils::       see below for documentation       	
  IDEUtils::        see below for documentation
  LangUtils::       see below for documentation

Example template code:

  The value of 10*cos(0.2) is ${10*Math.cos(0.2)}.

  Conditional output: ${iif(condition,"yes","no")}

  Create an empty file:
  <@do FileUtils.createFile("empty.txt", "")!/>
  
  The words of the sentence '${sentence}':
  <#list StringUtils.split(sentence) as word>
     ${word}
  </#list>

  Properties in the file ${filename}:
  <#assign props = FileUtils.readPropertyFile(filename)>
  <#list props?keys as key>
     ${key} ==> ${props.get(key)}
  </#list>

==== Math

Represents the Java Math class, which contains mathematical functions.
See http://java.sun.com/j2se/1.5.0/docs/api/java/lang/Math.html .

Math has the following methods:

  double cos(double x)

  double sin(double x)

  double pow(double x, double y)

etc.

==== StringUtils

Represents the Apache Commons StringUtils class, which contains over a hundred
utility functions for manipulating strings.
See http://commons.apache.org/lang/api/org/apache/commons/lang/StringUtils.html .

StringUtils has the following methods::

  boolean isEmpty(String s)

  boolean isBlank(String s)

  String capitalize(String s)

  String upperCase(String s)

  String lowerCase(String s)

  boolean startsWith(String s, String suffix)

  boolean endsWith(String s, String prefix)

  String[] split(String s)

  String join(String[] strings)

etc.

==== WordUtils

Represents the Apache Commons WordUtils class, which contains utility functions 
for manipulating strings as word sequences.
See http://commons.apache.org/lang/api/org/apache/commons/lang/WordUtils.html .

WordUtils has the following methods:

  String wrap(String str, int wrapLength)
  
  String capitalize(String str)
  
  String swapCase(String str)
  
etc.

==== CollectionUtils

Represents the Apache Commons CollectionUtils class, which contains some useful
functions for manipulating collections (like lists). Functions include computing
set union, intersection, and difference.
See http://commons.apache.org/collections/apidocs/org/apache/commons/collections/CollectionUtils.html

CollectionUtils has the following methods:

  Collection union(Collection a, Collection b)

  Collection intersection(Collection a, Collection b)

  Collection subtract(Collection a, Collection b)

etc.

==== FileUtils

Contains utility functions for reading files in the following formats: XML,
JSON, CSV, property file, and functions to read and return a text file
as a single string, as an array of lines, and as a an array of string arrays
(where string arrays were created by splitting each by whitespace).

There are two sets of functions, one works on files in the Eclipse workspace,
and the other files on "external" files, i.e. files in the file system.
Files are interpreted in the Java platform's default encoding (unless XML files,
which specify their own encoding.)

FileUtils has the following methods:

  org.w3c.dom.Document readXMLFile(String fileName)
  org.w3c.dom.Document readExternalXMLFile(String fileName)

Parses an XML file, and return the Document object of the resulting
DOM tree.

  Object readJSONFile(String fileName)
  Object readExternalJSONFile(String fileName)

Parses a JSON file. The result is a Boolean, Integer, Double, String,
List or Map, or any data structure composed of them. The JSON syntax is
documented at http://json.org; if you want to check whether a particular
text file corresponds to the JSON syntax, use http://jsonlint.com.

  String[][] readCSVFile(String fileName, boolean ignoreFirstLine,
                         boolean ignoreBlankLines, 
                         boolean ignoreCommentLines)
  String[][] readExternalCSVFile(String fileName, boolean ignoreFirstLine,
                         boolean ignoreBlankLines, 
                         boolean ignoreCommentLines)

Reads a CSV file. The result is an array of lines, where each line is
a string array. Additional method parameters control whether to discard the
first line of the file (which is usually a header line), whether to
ignore blank lines, and whether to ignore comment lines (those starting
with the # character). Comment lines are not part of the commonly accepted
CSV format, but they are supported here nevertheless, due to their
usefulness.

  Properties readPropertyFile(String fileName)
  Properties readExternalPropertyFile(String fileName)

Parses a Java property file ('key=value' lines) in the workspace.
The result is a Properties object, which is effectively a hash of
key-value pairs.

  String[][] readSpaceSeparatedTextFile(String fileName, 
                           boolean ignoreBlankLines, 
                           boolean ignoreCommentLines)
  String[][] readExternalSpaceSeparatedTextFile(String fileName, 
                           boolean ignoreBlankLines, 
                           boolean ignoreCommentLines)

Reads a text file, and return its contents, split by lines, and each line
split by whitespace. Additional method parameters control whether to ignore
blank lines and/or comment lines (those starting with the # character).
The result is an array of lines, where each line is a string array of the
items on the line.

  String[] readLineOrientedTextFile(String fileName)
  String[] readExternalLineOrientedTextFile(String fileName)

Reads a text file in the workspace, and returns its lines. Comment lines
(those starting with a hash mark, #) are discarded. The result is a
string array.

  String readTextFile(String fileName)
  String readExternalTextFile(String fileName)

Reads a text file, and return its contents unchanged as a single string.

  boolean isValidWorkspacePath(String path)

Returns true if the given string is syntactically a valid workspace path.

  boolean isValidWorkspaceFilePath(String path)

Returns true if the given string is syntactically a valid workspace file path.
This function does not check whether the file exists, or whether the given path
actually already points to a resource of a different type.

  IWorkspaceRoot getWorkspaceRoot()

Returns the workspace root object. The workspace contains the user's
projects.

  IProject asProject(String path)

Returns the handle for the workspace project with the given name.
Throws exception if the path is not a valid workspace project path.
This function does not test whether the project exists. To test that,
call the exists() method on the returned handle.

  IContainer asContainer(String path)

Returns the handle for the workspace container (i.e. project or folder) with the given name.
Throws exception if the path is not a valid workspace container path.
This function does not test whether the container exists. To test that,
call the exists() method on the returned handle.

  IFile asFile(String path)

Returns the handle for the workspace file with the given name.
Throws exception if the path is not a valid workspace file path.
This function does not test whether the file exists. To test that,
call the exists() method on the returned handle.

  IResource asResource(String pathName)

Returns the handle for the workspace project, folder or file with
the given name. If the resource does not exist and the path contains
more than one segment (i.e. it cannot be a project), it is returned as
a file handle if it has a file extension, and as a folder if it
does not.

  File asExternalFile(String path)

Returns a java.io.File object for the given path. The object can be used to
access operations provided by the File API, such as exists(), length(), etc.

  void copy(String path, String destPath, IProgressMonitor monitor)

Copies a workspace resource (file, folder or project) given with its path
to the destination path. For projects and folders, it copies recursively
(i.e. copies the whole folder tree). From the project root directory it
leaves out dot files, hidden files, and team private files.

   void copyURL(String url, String destFilePath, 
                         IProgressMonitor monitor)

Copies the file at the given URL to the given destination workspace file.

   String createTempFile(String content)

Writes the given string to a temporary file, and returns the path of the
temporary file in the file system. The file will be automatically deleted
when the IDE exits, but it can be also deleted earlier via deleteExternalFile().

   void createFile(String fileName, String content)

Creates a workspaces text file with the given contents, in the platform's default encoding.

   void createExternalFile(String fileName, String content)

Creates a text file in the file system with the given contents, in the platform's default encoding.

   void deleteFile(String fileName)

Deletes the given workspace file. It is OK to invoke it on a nonexistent file.

   void deleteExternalFile(String fileName)

Deletes the given file from the file system. It is OK to invoke it on a nonexistent file.

   void createDirectory(String fileName)

Creates a workspace folder. The parent must exist.

   void createExternalDirectory(String fileName)

Creates a directory in the file system. The parent must exist.

   void removeDirectory(String fileName)

Deletes a workspace folder. The folder must be empty. It is OK to invoke
it on a nonexistent folder.

   void removeExternalDirectory(String fileName)

Deletes a directory in the file system. The directory must be empty.
It is OK to invoke it on a nonexistent directory.

==== NedUtils

Provides utility methods to work with NED types and check their existence.

NedUtils has the following methods:

  boolean isVisibleType(String typeName, String inFolder)

Returns whether the given NED type is visible in the given folder.
If the type is a fully qualified name, it is recognized if it is
defined in the same project as the given folder, or in one of its
referenced projects; if the type is a simple name (without package),
it is recognized if it's in the NED package of the given folder.

  INEDTypeInfo getNedType(String typeName, String inFolder)

Like isVisibleNedType(), but actually returns the given NED type
if it was found; otherwise it returns null. Useful if you implement
a complex wizard page in Java.

==== IDEUtils

Provides entry points into various aspects of the IDE. This includes access to
the Eclipse workspace (projects, folders, files), and the NED index. The former
is documented in the Eclipse Platform help; documentation for the latter can
be found in the sources (Javadoc).
See http://help.eclipse.org/galileo/index.jsp?topic=/org.eclipse.platform.doc.isv/guide/resInt.htm
and http://help.eclipse.org/galileo/topic/org.eclipse.platform.doc.isv/reference/api/org/eclipse/core/resources/IWorkspaceRoot.html

IDEUtils has the following methods:

  boolean openConfirm(String title, String message, 
                              String detailsMessage)
  boolean openQuestion(String title, String message, 
                               String detailsMessage)
  boolean openError(String title, String message, 
                            String detailsMessage)
  boolean openWarning(String title, String message, 
                              String detailsMessage)
  boolean openInformation(String title, String message, 
                              String detailsMessage)

Opens a standard message dialog, with an closable details message.

==== LangUtils

Provides a collection of Java language related utility functions.

LangUtils shas the following methods:

  boolean hasMethod(Object object, String methodName)

Returns true if the object has a method with the given name.
Method args and return type are not taken into account.
Search is performed on the object's class and all super classes.

  boolean hasField(Object object, String fieldName)

Returns true if the object has a field with the given name.
Field type is not taken into account. Search is performed on
the object's class and all super classes.

  boolean instanceOf(Object object, String classOrInterfaceName)

Returns true if the given object is instance of (subclasses from or
implements) the given class or interface. To simplify usage, the class
or interface name is accepted both with and without the package name.

  String toString(Object object)

Produces a user-friendly representation of the object. In case of
collections (lists, maps, etc), the representation is JSON-like.

  List<Object> newList()

Creates and returns a new mutable List object (currently ArrayList).

  Map<Object, Object> newMap()

Creates and returns a new mutable Map object (currently HashMap).

  Set<Object> newSet()

Creates and returns a new mutable Set object (currently HashSet).

  Class<?> getClass(Object object)

Returns the class of the given object. Provided because BeanWrapper
seems to have a problem with the getClass() method.

==== ProcessUtils

Provides functionality to start external applications from the wizard, 

ProcessUtis has the following methods:

  ProcessResult exec(String command, String[] arguments, 
                             String workingDirectory, 
                             String standardInput, double timeout)

Executes the given command with the arguments as a separate process.
The standard input is fed into the spawn process and the output is read
until the process finishes or timeout occurs. The timeout value 0 means wait
infinitely long to finish the process. Arguments at the end of the argument
list are optional.

  String lookupExecutable(String name)

Finds the given executable in the path, and returns it with full path.
If not found, it returns the original string.


TODO document what are the variables nedResources and msgResources; write a few examples about their usage

TODO document glob patterns in verbatimFiles etc

TODO show how to write templates that are suitable for more than one wizard.

TODO check: BeanWrapper cannot access inherited methods? (e.g. toString())

TODO tutorial: XSWT, FTL, how to edit stuff; how to use Java classes (new widgets in XSWT,
JARs in the templates)

TODO add a little nutshell XSWT tutorial!

TODO also some minimal FreeMarker tutorial (variables are ${}, there is <#if>,
  <#list>; and there's lots more, see the manual and/or the provided examples

TODO document export/import wizards
