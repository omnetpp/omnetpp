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
import org.omnetpp.ide.properties.DocumentationGeneratorPropertyPage;

public class GeneratorConfigurationDialog
    extends TitleAreaDialog
{
    private IProject project;
    
    private List<IProject> projects = new ArrayList<IProject>();

    private CheckboxTableViewer selectedProjects;

    private List<DocumentationGenerator> generators;

    private Button generateDoxy;

    private Button generateNedTypeFigures;

    private Button generateInheritanceDiagrams;

    private Button generateUsageDiagrams;

    private Button generateSourceContent;

    private GeneratorConfiguration configuration;

    private Text outputDirectoryPath;

    private Button browseButton;

    private Button insideProjectsButton;

    private Button separateDirecttoryButton;

    public GeneratorConfigurationDialog(Shell parentShell, IProject project, GeneratorConfiguration configuration) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.RESIZE);

        this.project = project;
        this.configuration = configuration;
    }
    
    @Override
    protected Control createDialogArea(Composite parent) {
        setHelpAvailable(false);
        setTitle("Please configure documentation generation options");
        setMessage("Documentation will be generated for all declared types in the selected projects");

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
                projects.add(project);
                selectedProjects.add(project);
            }
        }
        
        if (project != null) {
            selectedProjects.setChecked(project, true);
            selectedProjects.setSelection(new StructuredSelection(project));
        }

        createProjectListButtons(container);
    }

    private void createProjectListButtons(Composite container) {
        Composite buttons = new Composite(container, SWT.NONE);
        buttons.setLayout(new GridLayout(1, true));
        buttons.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false, 1, 1));

        Button button = new Button(buttons, SWT.PUSH);
        button.setText("Select referenced");
        button.addSelectionListener(new SelectionAdapter() {
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
        
        button = new Button(buttons, SWT.PUSH);
        button.setText("Select All");
        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                selectedProjects.setCheckedElements(projects.toArray());
            }
        });
        
        button = new Button(buttons, SWT.PUSH);
        button.setText("Deselect All");
        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                selectedProjects.setCheckedElements(new IProject[0]);
            }
        });
    }

    private void createContentOptions(Composite container) {
        Group group = new Group(container, SWT.NONE);
        group.setLayout(new RowLayout(SWT.VERTICAL));
        group.setText("Content options");
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

        generateDoxy = new Button(group, SWT.CHECK);
        generateDoxy.setSelection(configuration.generateDoxy);
        generateDoxy.setText("Generate Doxygen C++ documentation");

        generateNedTypeFigures = new Button(group, SWT.CHECK);
        generateNedTypeFigures.setSelection(configuration.generateNedTypeFigures);
        generateNedTypeFigures.setText("Generate NED type figures");

        generateInheritanceDiagrams = new Button(group, SWT.CHECK);
        generateInheritanceDiagrams.setSelection(configuration.generateInheritanceDiagrams);
        generateInheritanceDiagrams.setText("Generate inheritance diagrams");

        generateUsageDiagrams = new Button(group, SWT.CHECK);
        generateUsageDiagrams.setSelection(configuration.generateUsageDiagrams);
        generateUsageDiagrams.setText("Generate usage diagrams");

        generateSourceContent = new Button(group, SWT.CHECK);
        generateSourceContent.setSelection(configuration.generateSourceContent);
        generateSourceContent.setText("Generate type source content");
    }

    private void createOutputOptions(Composite container) {
        Group group = new Group(container, SWT.NONE);
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false, 2, 1));
        group.setLayout(new GridLayout(3, false));
        group.setText("Output options");

        insideProjectsButton = new Button(group, SWT.RADIO);
        insideProjectsButton.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false, 3, 1));
        insideProjectsButton.setText("inside projects");

        separateDirecttoryButton = new Button(group, SWT.RADIO);
        separateDirecttoryButton.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 1, 1));
        separateDirecttoryButton.setText("separate directory");
        
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

        separateDirecttoryButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                updateBrowseButton();
            }
        });

        insideProjectsButton.setSelection(configuration.outputDirectoryPath == null);
        separateDirecttoryButton.setSelection(configuration.outputDirectoryPath != null);
        updateBrowseButton();
    }

    private void updateBrowseButton() {
        outputDirectoryPath.setEnabled(separateDirecttoryButton.getSelection());
        browseButton.setEnabled(separateDirecttoryButton.getSelection());
    }

    @Override
    protected void configureShell(Shell newShell) {
        newShell.setText("Configure Documentation Generation");
        super.configureShell(newShell);
    }
    
    @Override
    protected void okPressed() {
        generators =  new ArrayList<DocumentationGenerator>();

        configuration.generateDoxy = generateDoxy.getSelection();
        configuration.generateNedTypeFigures = generateNedTypeFigures.getSelection();
        configuration.generateUsageDiagrams = generateUsageDiagrams.getSelection();
        configuration.generateInheritanceDiagrams = generateInheritanceDiagrams.getSelection();
        configuration.generateSourceContent = generateSourceContent.getSelection();
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

        super.okPressed();
    }
    
    public List<DocumentationGenerator> getConfiguredDocumentationGenerators() {
        return generators;
    }
}
