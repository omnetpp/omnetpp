/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.neddoc.properties;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.QualifiedName;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.dialogs.PropertyPage;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.OmnetppDirs;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.ProcessUtils;

/**
 * This is a property page displayed in project properties to set the project specific parameters for documentation generation.
 * The property values are stored on the project as persistent properties.
 * The parameters are the following project relative paths:
 *  - doxygen configuration file
 *  - generated doxygen documentation
 *  - generated neddoc documentation
 *
 * @author levy
 */
public class DocumentationGeneratorPropertyPage
    extends PropertyPage
{
    public static final String DEFAULT_DOXY_PATH = "doc/doxy";
    public static final String DEFAULT_DOXY_CONFIG_FILE_PATH = "doxy.cfg";
    public static final String DEFAULT_NEDDOC_PATH = "doc/neddoc";

    public static QualifiedName DOXY_PATH_QNAME = new QualifiedName("DocumentationGenerator", "DoxyPath");
    public static QualifiedName DOXY_CONFIG_FILE_PATH_QNAME = new QualifiedName("DocumentationGenerator", "DoxyConfigFilePath");
    public static QualifiedName NEDDOC_PATH_QNAME = new QualifiedName("DocumentationGenerator", "NeddocPath");
    public static QualifiedName CUSTOM_CSS_QNAME = new QualifiedName("DocumentationGenerator", "CustomCss");

    private Text doxyPath;
    private Text doxyConfigFilePath;
    private Text neddocPath;
    private Text customCssPath;

    public DocumentationGeneratorPropertyPage() {
        super();
    }

    @Override
    protected Control createContents(Composite parent) {
        final IProject project = getProject();

        Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(1, false));

        Group group = new Group(composite, SWT.NONE);
        group.setText("Project relative paths");
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        group.setLayout(new GridLayout(3, false));

        doxyPath = addTextAndBrowse(group, "Generated Doxygen documentation:", true);
        neddocPath = addTextAndBrowse(group, "Generated NED documentation:", true);

        try {
            setText(doxyPath, getDoxyPath(project));
            setText(neddocPath, getNeddocPath(project));
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }

        group = new Group(composite, SWT.NONE);
        group.setText("Doxygen");
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        group.setLayout(new GridLayout(3, false));

        doxyConfigFilePath = addTextAndBrowse(group, "Configuration file path:", false);

        IPreferenceStore store = CommonPlugin.getConfigurationPreferenceStore();
        final String doxyExecutablePath = store.getString(IConstants.PREF_DOXYGEN_EXECUTABLE);

        Button button = new Button(group, SWT.PUSH);
        button.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false, 3, 1));
        button.setText("Generate default");
        button.setEnabled(doxyExecutablePath != null && new File(ProcessUtils.lookupExecutable(doxyExecutablePath)).exists());
        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                String relativePath = doxyConfigFilePath.getText();
                if (relativePath == null || relativePath.equals("")) {
                    relativePath = "doxy.cfg";
                    doxyConfigFilePath.setText(relativePath);
                }
                String fileName = project.getFile(relativePath).getLocation().toOSString();

                if (new File(fileName).exists())
                    if (!MessageDialog.openConfirm(getShell(), "Confirm overwrite", "Do you wan to overwrite the Doxygen configuration file: " + fileName + "?"))
                       return;

                try {
                    generateDefaultDoxyConfigurationFile(project, doxyExecutablePath, fileName);
                }
                catch (Exception x) {
                    MessageDialog.openError(null, "Documentation Generation",
                            "Error generating the Doxygen configuration file: " + fileName + " using Doxygen: " + doxyExecutablePath);
                }

            }
        });

        try {
            setText(doxyConfigFilePath, getDoxyConfigFilePath(project));
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }

        Label label = new Label(group, SWT.WRAP);
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 3, 1));
        label.setText("Note: for referencing external doxygen documentation see the TAGFILES option\n");

        group = new Group(composite, SWT.NONE);
        group.setText("HTML");
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        group.setLayout(new GridLayout(3, false));

        customCssPath = addTextAndBrowse(group, "Custom CSS path:", false);

        try {
            setText(customCssPath, getCustomCssPath(project));
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }

        return composite;
    }

    private void setText(Text text, String value) {
        if (value != null)
            text.setText(value);
    }

    private Text addTextAndBrowse(Composite composite, String labelText, final boolean folder) {
        Label label = new Label(composite, SWT.NONE);
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 1, 1));
        label.setText(labelText);

        final Text text = new Text(composite, SWT.BORDER);
        text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 1, 1));

        Button button = new Button(composite, SWT.PUSH);
        button.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 1, 1));
        button.setText("Browse...");
        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                IProject project = getProject();

                ElementTreeSelectionDialog dialog = new ElementTreeSelectionDialog(getShell(), new WorkbenchLabelProvider(),
                    new WorkbenchContentProvider() {
                        @Override
                        public Object[] getChildren(Object element) {
                            Object[] children = super.getChildren(element);

                            if (folder) {
                                ArrayList<Object> filteredChildren = new ArrayList<Object>();
                                for (Object child : children)
                                   if (child instanceof IContainer)
                                       filteredChildren.add(child);
                                return filteredChildren.toArray();
                            }
                            else
                                return children;
                        }
                });
                dialog.setAllowMultiple(false);
                dialog.setTitle("Select a " + (folder ? "Folder" : "File"));
                dialog.setInput(project);
                dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));

                if (dialog.open() == IDialogConstants.OK_ID && dialog.getFirstResult() instanceof IResource) {
                    String result = ((IResource)dialog.getFirstResult()).getFullPath().toString();
                    text.setText(result.replaceFirst("^/" + project.getName() + "/", ""));
                }
            }
        });

        return text;
    }

    @Override
    protected void performDefaults() {
        doxyPath.setText(DEFAULT_DOXY_PATH);
        doxyConfigFilePath.setText(DEFAULT_DOXY_CONFIG_FILE_PATH);
        neddocPath.setText(DEFAULT_NEDDOC_PATH);
    }

    @Override
    public boolean performOk() {
        try {
            IProject project = getProject();

            project.setPersistentProperty(DOXY_PATH_QNAME, doxyPath.getText());
            project.setPersistentProperty(DOXY_CONFIG_FILE_PATH_QNAME, doxyConfigFilePath.getText());
            project.setPersistentProperty(NEDDOC_PATH_QNAME, neddocPath.getText());
            project.setPersistentProperty(CUSTOM_CSS_QNAME, customCssPath.getText());

            return true;
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }
    }

    private IProject getProject() {
        return (IProject)getElement().getAdapter(IProject.class);
    }

    public static String getDoxyPath(IProject project) throws CoreException {
        String value = project.getPersistentProperty(DOXY_PATH_QNAME);

        if (value == null)
            return DEFAULT_DOXY_PATH;
        else
            return value;
    }

    public static String getDoxyConfigFilePath(IProject project) throws CoreException {
        String value = project.getPersistentProperty(DOXY_CONFIG_FILE_PATH_QNAME);

        if (value == null)
            return DEFAULT_DOXY_CONFIG_FILE_PATH;
        else
            return value;
    }

    public static String getNeddocPath(IProject project) throws CoreException {
        String value = project.getPersistentProperty(NEDDOC_PATH_QNAME);

        if (value == null)
            return DEFAULT_NEDDOC_PATH;
        else
            return value;
    }

    public static String getCustomCssPath(IProject project) throws CoreException {
        return project.getPersistentProperty(CUSTOM_CSS_QNAME);
    }

    public static String replaceDoxygenConfigurationEntry(String content, String key, String value) {
        key = key.replace("\\", "\\\\");
        value = value.replace("\\", "\\\\");
        return content.replaceAll("(?m)^\\s*" + key +"\\s*=.*?$", key + "=" + value);
    }

    public static void generateDefaultDoxyConfigurationFile(final IProject project, final String doxyExecutablePath, String fileName) 
            throws IOException {
        ProcessUtils.exec(doxyExecutablePath, new String[] {"-g", fileName}, project.getLocation().toString(), null);

        String content = FileUtils.readTextFile(fileName, null);
        // AUTO means will be set when generating the documentation based on project settings
        content = replaceDoxygenConfigurationEntry(content, "PROJECT_NAME", project.getName());
        content = replaceDoxygenConfigurationEntry(content, "OUTPUT_DIRECTORY", "AUTO");
        content = replaceDoxygenConfigurationEntry(content, "FULL_PATH_NAMES", "NO");
        content = replaceDoxygenConfigurationEntry(content, "DETAILS_AT_TOP", "YES");
        content = replaceDoxygenConfigurationEntry(content, "JAVADOC_AUTOBRIEF", "YES");
        content = replaceDoxygenConfigurationEntry(content, "EXTRACT_ALL", "YES");
        content = replaceDoxygenConfigurationEntry(content, "EXTRACT_PRIVATE", "YES");
        content = replaceDoxygenConfigurationEntry(content, "QUIET", "YES");
        content = replaceDoxygenConfigurationEntry(content, "RECURSIVE", "YES");
        content = replaceDoxygenConfigurationEntry(content, "EXCLUDE_PATTERNS", "*_m.cc *_n.cc *.d");
        content = replaceDoxygenConfigurationEntry(content, "INLINE_SOURCES", "YES");
        content = replaceDoxygenConfigurationEntry(content, "REFERENCES_RELATION", "NO");
        content = replaceDoxygenConfigurationEntry(content, "VERBATIM_HEADERS", "NO");
        content = replaceDoxygenConfigurationEntry(content, "HTML_OUTPUT", ".");
        content = replaceDoxygenConfigurationEntry(content, "GENERATE_TREEVIEW", "NO");
        content = replaceDoxygenConfigurationEntry(content, "GENERATE_LATEX", "NO");
        content = replaceDoxygenConfigurationEntry(content, "GENERATE_TAGFILE", "AUTO");
        content = replaceDoxygenConfigurationEntry(content, "TEMPLATE_RELATIONS", "YES");
        content = replaceDoxygenConfigurationEntry(content, "TAGFILES", OmnetppDirs.getOmnetppRootDir() + "/doc/api/opptags.xml=" + OmnetppDirs.getOmnetppRootDir() + "/doc/api");
        FileUtils.writeTextFile(fileName, content, null);
    }
}