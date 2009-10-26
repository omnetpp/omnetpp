TODO: mire szolgal; kinek kellene ilyet csinalni es mi celbol...



Project templates are read from the templates/project folder of OMNeT++ projects.

In that folder, every subfolder that contains a template.properties file is
treated as a project template. (Other folders are ignored.)

Unless specified otherwise, all files and subdirectories in a template folder
are considered template files, and will be copied over into the new project
after template substitution, preserving file names and the directory structure.

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
  nontemplates  Comma-separated list of non-template files or folders; those files
                won't get copied over to the new project. Wildcards are NOT accepted.
                The template.properties file and custom wizard page files
                automatically count as nontemplates, so they don't have to be listed.
  optionalFiles Comma-separated list of files that should be suppressed (i.e. not
                created) if they would be empty after template processing.
                Wildcards are NOT accepted.
  sourceFolders
                Comma-separated list of C++ source folders to be created and
                configured. By default, none.
  nedSourceFolders
                Comma-separated list of NED source folders to be created and
                configured. By default, none.
  makemakeOptions
                Comma-separated list of items in the syntax "folder:options";
                it sets opp_makemake options for the given folders. No default.

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

The property file takes precedence over values in the XSWT file.

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

An XSWT tuturial and documentation can be found at:
http://www.coconut-palm-software.com/the_new_visual_editor/doku.php?id=xswt:home

Documentation for the FreeMarker template language:
http://freemarker.org/docs/index.html

---
TODO decide: maybe template files should have the ".ftl" extension, so that
they get edited as FreeMarker template and not as NED/INI/C++ files?

TODO: special markup in the template: "<?redirect-to="bubu.txt"> ... </?redirect>
to support creation of files with runtime-decided names.

TODO: check if XSWT 2.0 could be brought to work? Because it changed the file
format a bit, and we want future OMNET++ versions to be backward compatible!!!
