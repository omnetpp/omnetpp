/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.neddoc;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.ui.SWTFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ide.preferences.OmnetppPreferencePage;
import org.omnetpp.neddoc.properties.DocumentationGeneratorPropertyPage;

/**
 * This is the documentation generator configuration dialog displayed before
 * generating documentation for an OMNeT++ project. The dialog shows
 * the list of open projects to allow generating documentation for multiple
 * projects at once. In addition, it allows to control what is included
 * in the documentation: doxygen, NED type figures, inheritance and usage
 * diagrams, etc.
 *
 * The documentation generators are configured to put the result either
 * under the project preconfigured by the user using the project properties
 * page or to a separate directory given in this dialog.
 *
 * @author levy
 */
public class GeneratorConfigurationDialog
    extends Dialog
{
    private List<IProject> allProjects = new ArrayList<IProject>();
    private List<DocumentationGenerator> generators;
    private GeneratorConfiguration configuration;

    // Widgets
    private CheckboxTableViewer selectedProjects;

    private Button generateNedTypeFigures;
    private Button generatePerTypeInheritanceDiagrams;
    private Button generatePerTypeUsageDiagrams;
    private Button generateFullInheritanceDiagrams;
    private Button generateFullUsageDiagrams;
    private Button generateSourceContent;
    private Button generateMsgDefinitions;
    private Button generateFileListings;
    private Button enableAutomaticHyperlinking;

    private Button generateDoxy;
    private Button doxySourceBrowser;

    private Text excludedDirs;
    private Text extensionFilePath;
    private Text outputDirectoryPath;

    private Button browseButton;
    private Button insideProjectsButton;
    private Button separateDirectoryButton;

    public GeneratorConfigurationDialog(Shell parentShell, GeneratorConfiguration configuration) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.RESIZE);

        this.configuration = configuration;
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        Composite container = new Composite((Composite)super.createDialogArea(parent), SWT.NONE);
        container.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        container.setLayout(new GridLayout(2, false));

        createProjectList(container);
        createContentOptions(container);
        createOutputOptions(container);

        return container;
    }

    private void createProjectList(Composite container) {
        Group group = SWTFactory.createGroup(container, "Inputs", 2, 2, SWTFactory.GRAB_AND_FILL_HORIZONTAL);

        selectedProjects = CheckboxTableViewer.newCheckList(group, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
        selectedProjects.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1));
        selectedProjects.setLabelProvider(new LabelProvider() {
            public String getText(Object element) {
                return ((IProject)element).getName();
            }
        });

        selectedProjects.addCheckStateListener(new ICheckStateListener() {
            public void checkStateChanged(CheckStateChangedEvent event) {
                setOkButtonEnabled();
            }
        });

        for (IProject project : ProjectUtils.getOmnetppProjectsSafely(getShell())) {
            if (project.isAccessible()) {
                allProjects.add(project);
                selectedProjects.add(project);
            }
        }

        if (configuration.projects != null) {
            selectedProjects.setCheckedElements(configuration.projects);
            selectedProjects.setSelection(new StructuredSelection(configuration.projects));
            setOkButtonEnabled();
        }

        createProjectListButtons(group);

        Composite files = SWTFactory.createComposite(group, 2, 2, SWTFactory.GRAB_AND_FILL_HORIZONTAL);

        SWTFactory.createLabel(files, "Exclude directories:", 1);
        excludedDirs = SWTFactory.createText(files, SWT.BORDER, 1);
        excludedDirs.setMessage("example: /inet/samples, /*/examples, **/test");
        if (configuration.excludedDirs != null)
            excludedDirs.setText(configuration.excludedDirs);

        SWTFactory.createLabel(files, "Doc fragments file:", 1);
        extensionFilePath = SWTFactory.createText(files, SWT.BORDER, 1);
        extensionFilePath.setMessage("example: neddoc-fragments.xml (in each project)");
        if (configuration.extensionFilePath != null)
            extensionFilePath.setText(configuration.extensionFilePath);

    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        super.createButtonsForButtonBar(parent);
        setOkButtonEnabled();
    }

    private void setOkButtonEnabled() {
        Button button = getButton(IDialogConstants.OK_ID);

        if (button != null)
            button.setEnabled(selectedProjects.getCheckedElements().length != 0);
    }

    private void createProjectListButtons(Composite container) {
        Composite buttons = new Composite(container, SWT.NONE);
        buttons.setLayout(new GridLayout(1, true));
        buttons.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));

        Button selectReferencedButton = new Button(buttons, SWT.PUSH);
        selectReferencedButton.setText("Select Referenced");
        selectReferencedButton.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
        selectReferencedButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                ISelection selection = selectedProjects.getSelection();

                if (selection instanceof IStructuredSelection) {
                    IStructuredSelection structuredSelection = (IStructuredSelection)selection;
                    Set<IProject> referencedProjects = new HashSet<IProject>();

                    IProject selectedProject = (IProject)structuredSelection.getFirstElement();
                    if (selectedProject != null) {
                        addReferencedProjects(selectedProject, referencedProjects);

                        // keep old selection
                        for (Object project : selectedProjects.getCheckedElements())
                            referencedProjects.add((IProject)project);

                        selectedProjects.setCheckedElements(referencedProjects.toArray());
                        setOkButtonEnabled();
                    }
                }
            }

            private void addReferencedProjects(IProject project, Set<IProject> dependents) {
                try {
                    dependents.add(project);

                    for (IProject referencedProject : project.getReferencedProjects())
                        addReferencedProjects(referencedProject, dependents);
                }
                catch (CoreException e) {
                    throw new RuntimeException(e);
                }
            }
        });

        Button selectAllButton = new Button(buttons, SWT.PUSH);
        selectAllButton.setText("Select All");
        selectAllButton.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
        selectAllButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                selectedProjects.setCheckedElements(allProjects.toArray());
                setOkButtonEnabled();
            }
        });

        Button deselectAllButton = new Button(buttons, SWT.PUSH);
        deselectAllButton.setText("Deselect All");
        deselectAllButton.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
        deselectAllButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                selectedProjects.setCheckedElements(new IProject[0]);
                setOkButtonEnabled();
            }
        });
    }

    private void createContentOptions(Composite container) {
        Group group = new Group(container, SWT.NONE);
        group.setLayout(new GridLayout(2, true));
        group.setText("Generate");
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

        boolean dotAvailable = OmnetppPreferencePage.isGraphvizDotAvailable();

        generateNedTypeFigures = createCheckbox(group, "Network diagrams", configuration.generateNedTypeFigures && dotAvailable);
        generatePerTypeInheritanceDiagrams = createCheckbox(group, "Per-type inheritance diagrams*", configuration.generatePerTypeInheritanceDiagrams && dotAvailable);
        generatePerTypeInheritanceDiagrams.setEnabled(dotAvailable);
        generateMsgDefinitions = createCheckbox(group, "Message Definitions", configuration.generateMsgDefinitions);
        generatePerTypeUsageDiagrams = createCheckbox(group, "Per-type usage diagrams*", configuration.generatePerTypeUsageDiagrams && dotAvailable);
        generatePerTypeUsageDiagrams.setEnabled(dotAvailable);
        generateSourceContent = createCheckbox(group, "Source on NED and MSG type pages", configuration.generateSourceListings);
        generateFullInheritanceDiagrams = createCheckbox(group, "Full inheritance diagrams*", configuration.generateFullInheritanceDiagrams && dotAvailable);
        generateFullInheritanceDiagrams.setEnabled(dotAvailable);
        generateFileListings = createCheckbox(group, "NED and MSG source file listings", configuration.generateFileListings);
        generateFullUsageDiagrams = createCheckbox(group, "Full usage diagrams*", configuration.generateFullUsageDiagrams && dotAvailable);
        generateFullUsageDiagrams.setEnabled(dotAvailable);

        SWTFactory.createLabel(group, "", 1);
        Label label1 = new Label(group, SWT.NONE);
        label1.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 1, 1));
        label1.setText("   Note: Options marked * require Graphviz");


        enableAutomaticHyperlinking = createCheckbox(group, "Automatic hyperlinking of NED and message type names", configuration.automaticHyperlinking);
        enableAutomaticHyperlinking.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
        Label label = new Label(group, SWT.NONE);
        label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
        label.setText("   Note: when turned off, use the tilde notation for names to be hyperlinked: ~Sink, ~TCP.");

        boolean doxygenAvailable = OmnetppPreferencePage.isDoxygenAvailable();
        generateDoxy = createCheckbox(group, "C++ documentation (requires Doxygen)", configuration.generateDoxy && doxygenAvailable);
        generateDoxy.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
        generateDoxy.setEnabled(doxygenAvailable);
        label = new Label(group, SWT.NONE);
        label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
        label.setText("   Note: Doxygen configuration file locations can be configured in the Project Properties dialog");

        doxySourceBrowser = createCheckbox(group, "C++ source file listings", configuration.cppSourceListings);
        doxySourceBrowser.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
        doxySourceBrowser.setEnabled(doxygenAvailable);
        label = new Label(group, SWT.NONE);
        label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
        label.setText("   Note: other Doxygen options can be configured in the Doxygen configuration file");
    }

    private Button createCheckbox(Composite parent, String label, boolean initialSelection) {
        return SWTFactory.createCheckButton(parent, label, null, initialSelection, 1);
    }

    private void createOutputOptions(Composite container) {
        Group group = new Group(container, SWT.NONE);
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false, 2, 1));
        group.setLayout(new GridLayout(3, false));
        group.setText("Target folders");

        insideProjectsButton = new Button(group, SWT.RADIO);
        insideProjectsButton.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false, 3, 1));
        insideProjectsButton.setText("Inside each project (note: locations can be configured in the Project Properties dialog)");

        separateDirectoryButton = new Button(group, SWT.RADIO);
        separateDirectoryButton.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 1, 1));
        separateDirectoryButton.setText("A common directory:");

        outputDirectoryPath = new Text(group, SWT.BORDER);
        if (configuration.outputDirectoryPath != null)
            outputDirectoryPath.setText(configuration.outputDirectoryPath);
        outputDirectoryPath.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 1, 1));

        browseButton = new Button(group, SWT.PUSH);
        browseButton.setText("Browse...");
        browseButton.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 1, 1));
        browseButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                DirectoryDialog dialog = new DirectoryDialog(getShell());
                String fileName = dialog.open();

                if (fileName != null)
                    outputDirectoryPath.setText(fileName);
            }
        });

        insideProjectsButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                updateBrowseButton();
            }
        });

        separateDirectoryButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                updateBrowseButton();
            }
        });

        insideProjectsButton.setSelection(configuration.outputDirectoryPath == null);
        separateDirectoryButton.setSelection(configuration.outputDirectoryPath != null);
        updateBrowseButton();
    }

    private void updateBrowseButton() {
        outputDirectoryPath.setEnabled(separateDirectoryButton.getSelection());
        browseButton.setEnabled(separateDirectoryButton.getSelection());
    }

    @Override
    protected void configureShell(Shell newShell) {
        newShell.setText("NED/C++ Documentation Generation");
        super.configureShell(newShell);
    }

    /**
     * Creates the list of preconfigured documentation generators.
     */
    @Override
    protected void okPressed() {
        generators =  new ArrayList<DocumentationGenerator>();

        configuration.generateNedTypeFigures = generateNedTypeFigures.getSelection();
        configuration.generatePerTypeUsageDiagrams = generatePerTypeUsageDiagrams.getSelection();
        configuration.generatePerTypeInheritanceDiagrams = generatePerTypeInheritanceDiagrams.getSelection();
        configuration.generateFullUsageDiagrams = generateFullUsageDiagrams.getSelection();
        configuration.generateFullInheritanceDiagrams = generateFullInheritanceDiagrams.getSelection();
        configuration.generateSourceListings = generateSourceContent.getSelection();
        configuration.generateFileListings = generateFileListings.getSelection();
        configuration.generateMsgDefinitions = generateMsgDefinitions.getSelection();
        configuration.automaticHyperlinking = enableAutomaticHyperlinking.getSelection();
        configuration.excludedDirs = excludedDirs.getText();
        configuration.extensionFilePath = extensionFilePath.getText();

        configuration.generateDoxy = generateDoxy.getSelection();
        configuration.cppSourceListings = doxySourceBrowser.getSelection();

        Object[] selectedElements = selectedProjects.getCheckedElements();
        configuration.projects = new IProject[selectedElements.length];
        System.arraycopy(selectedElements, 0, configuration.projects, 0, selectedElements.length);

        configuration.outputDirectoryPath = outputDirectoryPath.getText().equals("") || insideProjectsButton.getSelection() ? null : outputDirectoryPath.getText();

        for (Object element : selectedProjects.getCheckedElements()) {
            IProject project = (IProject)element;
            DocumentationGenerator generator = new DocumentationGenerator(project);

            if (configuration.outputDirectoryPath != null)
                generator.setDocumentationRootPath(new Path(configuration.outputDirectoryPath).append(project.getName()));
            else
                generator.setDocumentationRootPath(project.getLocation());

            try {
                generator.setRootRelativeDoxyPath(new Path(DocumentationGeneratorPropertyPage.getDoxyPath(project)));
                generator.setRootRelativeNeddocPath(new Path(DocumentationGeneratorPropertyPage.getNeddocPath(project)));
                generator.setAbsoluteDoxyConfigFilePath(project.getFile(DocumentationGeneratorPropertyPage.getDoxyConfigFilePath(project)).getLocation());

                String customCssPath = DocumentationGeneratorPropertyPage.getCustomCssPath(project);
                if (!StringUtils.isEmpty(customCssPath))
                    generator.setCustomCssPath(project.getFile(DocumentationGeneratorPropertyPage.getCustomCssPath(project)).getLocation());
            }
            catch (CoreException e) {
                throw new RuntimeException(e);
            }

            generator.setGenerateNedTypeFigures(configuration.generateNedTypeFigures);
            generator.setGeneratePerTypeUsageDiagrams(configuration.generatePerTypeUsageDiagrams);
            generator.setGenerateFullUsageDiagrams(configuration.generateFullUsageDiagrams);
            generator.setGeneratePerTypeInheritanceDiagrams(configuration.generatePerTypeInheritanceDiagrams);
            generator.setGenerateFullInheritanceDiagrams(configuration.generateFullInheritanceDiagrams);
            generator.setGenerateSourceListings(configuration.generateSourceListings);
            generator.setGenerateFileListings(configuration.generateFileListings);
            generator.setGenerateMsgDefinitions(configuration.generateMsgDefinitions);
            generator.setAutomaticHyperlinking(configuration.automaticHyperlinking);
            generator.setGenerateDoxy(configuration.generateDoxy);
            generator.setGenerateCppSourceListings(configuration.cppSourceListings);
            generator.setExcludedDirs(configuration.excludedDirs);

            if (StringUtils.isNotBlank(configuration.extensionFilePath))
                generator.setExtensionFilePath(new Path(configuration.extensionFilePath));

            generators.add(generator);
        }

        NeddocPlugin.getDefault().storeGeneratorConfiguration(configuration);

        super.okPressed();
    }

    /**
     * Returns the list of preconfigured documentation generators. This method is intended to be called when the dialog result is OK.
     */
    public List<DocumentationGenerator> getConfiguredDocumentationGenerators() {
        return generators;
    }
}
