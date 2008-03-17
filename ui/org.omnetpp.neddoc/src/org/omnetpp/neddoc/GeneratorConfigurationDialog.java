package org.omnetpp.neddoc;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.ide.preferences.OmnetppPreferencePage;
import org.omnetpp.ide.properties.DocumentationGeneratorPropertyPage;

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
    extends TitleAreaDialog
{
    private List<IProject> allProjects = new ArrayList<IProject>();
    private List<DocumentationGenerator> generators;
    private GeneratorConfiguration configuration;

    // Widgets
    private CheckboxTableViewer selectedProjects;

    private Button generateNedTypeFigures;
    private Button generateInheritanceDiagrams;
    private Button generateUsageDiagrams;
    private Button generateSourceContent;

    private Button generateDoxy;
    private Button doxySourceBrowser;

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
        setHelpAvailable(false);
        setTitle("Generate HTML documentation from NED, MSG, and C++ files");
        setMessage("Please select projects and documentation options below");

        Composite container = new Composite((Composite)super.createDialogArea(parent), SWT.NONE);
        container.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        container.setLayout(new GridLayout(2, false));

        createProjectList(container);
        createContentOptions(container);
        createOutputOptions(container);

        return container;
    }

    private void createProjectList(Composite container) {
        Label label = new Label(container, SWT.NONE);
        label.setText("Select projects:");
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false, 2, 1));

        selectedProjects = CheckboxTableViewer.newCheckList(container, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
        selectedProjects.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1));
        selectedProjects.setLabelProvider(new LabelProvider() {
            public String getText(Object element) {
                return ((IProject)element).getName();
            }
        });

        for (IProject project : ResourcesPlugin.getWorkspace().getRoot().getProjects()) {
            if (project.isAccessible()) {
                allProjects.add(project);
                selectedProjects.add(project);
            }
        }

        if (configuration.projects != null) {
            selectedProjects.setCheckedElements(configuration.projects);
            selectedProjects.setSelection(new StructuredSelection(configuration.projects));
        }

        createProjectListButtons(container);
    }

    private void createProjectListButtons(Composite container) {
        Composite buttons = new Composite(container, SWT.NONE);
        buttons.setLayout(new GridLayout(1, true));
        buttons.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));

        Button selectReferencedButton = new Button(buttons, SWT.PUSH);
        selectReferencedButton.setText("Select referenced");
        selectReferencedButton.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
        selectReferencedButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                ISelection selection = selectedProjects.getSelection();

                if (selection instanceof IStructuredSelection) {
                    IStructuredSelection structuredSelection = (IStructuredSelection)selection;
                    Set<IProject> referencedProjects = new HashSet<IProject>();

                    IProject selectedProject = (IProject)structuredSelection.getFirstElement();
                    addReferencedProjects(selectedProject, referencedProjects);

                    // keep old selection
                    for (Object project : selectedProjects.getCheckedElements())
                        referencedProjects.add((IProject)project);

                    selectedProjects.setCheckedElements(referencedProjects.toArray());
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
            }
        });

        Button deselectAllButton = new Button(buttons, SWT.PUSH);
        deselectAllButton.setText("Deselect All");
        deselectAllButton.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
        deselectAllButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                selectedProjects.setCheckedElements(new IProject[0]);
            }
        });
    }

    private void createContentOptions(Composite container) {
        Group group = new Group(container, SWT.NONE);
        group.setLayout(new RowLayout(SWT.VERTICAL));
        group.setText("Generate:");
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

        boolean dotAvailable = OmnetppPreferencePage.isGraphvizDotAvailable();
        generateNedTypeFigures = createCheckbox(group, "Network diagrams", configuration.generateNedTypeFigures && dotAvailable);
        generateInheritanceDiagrams = createCheckbox(group, "Inheritance diagrams", configuration.generateInheritanceDiagrams && dotAvailable);
        generateUsageDiagrams = createCheckbox(group, "Usage diagrams", configuration.generateUsageDiagrams && dotAvailable);
        generateNedTypeFigures.setEnabled(dotAvailable);
        generateInheritanceDiagrams.setEnabled(dotAvailable);
        generateUsageDiagrams.setEnabled(dotAvailable);

        generateSourceContent = createCheckbox(group, "Source listings", configuration.generateSourceContent);

        boolean doxygenAvailable = OmnetppPreferencePage.isDoxygenAvailable();
        generateDoxy = createCheckbox(group, "C++ documentation (using Doxygen)", configuration.generateDoxy && doxygenAvailable);
        generateDoxy.setEnabled(doxygenAvailable);

        Label label = new Label(group, SWT.NONE);
        label.setText("   Note: Doxygen configuration file locations can be configured in the Project Properties dialog");

        doxySourceBrowser = createCheckbox(group, "Doxygen source browser", configuration.doxySourceBrowser);
        doxySourceBrowser.setEnabled(doxygenAvailable);

        label = new Label(group, SWT.NONE);
        label.setText("   Note: other Doxygen options can be configured in the Doxygen configuration file");
    }

    private Button createCheckbox(Composite parent, String text, boolean initialSelection) {
        Button checbox = new Button(parent, SWT.CHECK);
        checbox.setSelection(initialSelection);
        checbox.setText(text);
        return checbox;
    }

    private void createOutputOptions(Composite container) {
        Group group = new Group(container, SWT.NONE);
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false, 2, 1));
        group.setLayout(new GridLayout(3, false));
        group.setText("Target folders:");

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
        newShell.setText("Documentation Generation");
        super.configureShell(newShell);
    }

    /**
     * Creates the list of preconfigured documentation generators.
     */
    @Override
    protected void okPressed() {
        generators =  new ArrayList<DocumentationGenerator>();

        configuration.generateNedTypeFigures = generateNedTypeFigures.getSelection();
        configuration.generateUsageDiagrams = generateUsageDiagrams.getSelection();
        configuration.generateInheritanceDiagrams = generateInheritanceDiagrams.getSelection();
        configuration.generateSourceContent = generateSourceContent.getSelection();

        configuration.generateDoxy = generateDoxy.getSelection();
        configuration.doxySourceBrowser = doxySourceBrowser.getSelection();

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
            }
            catch (CoreException e) {
                throw new RuntimeException(e);
            }

            generator.setConfiguration(configuration);
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
