
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
  license               license identifier for the @license NED property
  bannercomment         banner comment for source files; includes license text

Other variables may come from custom template pages, see later.

The template.properties file is a standard Java property file (key=value syntax).
That file can be used to supply a template name, a template description,
and various other options.

Recognized property file keys:

  name          Template's display name; defaults to the folder name.
                This is the name that appears in the wizard's template selection
                page.
  description   Description of the template. This may appear as a tooltip
                or in a description pane in the wizard.
  category      Template category is used for organizing the templates into a
                tree in the wizard's template selection page.
  image         Name of the icon that appears with the name in the wizard's
                template selection page.
  dependent     True or false; defaults to true. If true, the template's project
                will be added to the referenced projects list of the new project.
                be marked as make project as dependent on this one
  nontemplates  Space-separated list of non-template files or folders; those files
                won't get copied over to the new project. Wildcards are NOT accepted.
                The template.properties file and custom wizard page files
                automatically count as nontemplates, so they don't have to be listed.
  optionalfiles Space-separated list of files that should be suppressed (i.e. not
                created) if they would be empty after template processing.
                Wildcards are NOT accepted.

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

TODO all fields in the property file should be available in the templates too!

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

