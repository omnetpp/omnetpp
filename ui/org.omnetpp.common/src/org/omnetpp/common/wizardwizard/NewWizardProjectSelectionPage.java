package org.omnetpp.common.wizardwizard;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.TemplateBasedWizard;


public class NewWizardProjectSelectionPage extends WizardPage {
    private TableViewer table;
    private Text text;

    public NewWizardProjectSelectionPage(String pageName) {
        super(pageName);
        setTitle("New Wizard");
        setDescription("Contribute a new wizard into one of the New... dialogs");
    }

    public void createControl(Composite parent) {
        Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(1, false));

        Label label1 = new Label(composite, SWT.NONE);
        label1.setText("Select the project that will hold the wizard files:");

        table = new TableViewer(composite, SWT.BORDER|SWT.V_SCROLL);
        GridData gridData;
        table.getTable().setLayoutData(gridData = new GridData(SWT.FILL, SWT.FILL, true, true));
        gridData.heightHint = 100;
        table.setContentProvider(new ArrayContentProvider());
        table.setLabelProvider(new WorkbenchLabelProvider());
        table.setInput(ProjectUtils.getOmnetppProjects());
        table.setComparator(new ResourceComparator(ResourceComparator.NAME));

        Label label2 = new Label(composite, SWT.NONE);
        label2.setText("Enter wizard name:");

        text = new Text(composite, SWT.BORDER);
        text.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        text.setFocus();

        table.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                tableSelectionChanged();
            }});
        table.getTable().addMouseListener(new MouseAdapter() {
            public void mouseUp(MouseEvent e) {
                text.setFocus();
            }});
        text.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                textModified();
            }});
        setControl(composite);
    }

    protected void textModified() {
        validate();
    }

    protected void tableSelectionChanged() {
        validate();
    }

    protected void validate() {
        setMessage(null);
        setErrorMessage(null);
        Object sel = ((IStructuredSelection)table.getSelection()).getFirstElement();
        if (sel == null) {
            setMessage("Choose project");
            setPageComplete(false);
            return;
        }
        String wizardName = text.getText();
        if (wizardName.isEmpty()) {
            setMessage("Enter name for wizard");
            setPageComplete(false);
            return;
        }
        if (!wizardName.equals(StringUtils.makeValidIdentifier(wizardName))) {
            setErrorMessage("Wizard name contains illegal characters");
            setPageComplete(false);
            return;
        }
        IProject project = (IProject)sel;
        IFolder folder = project.getFolder(new Path(TemplateBasedWizard.TEMPLATES_FOLDER_NAME + "/" + wizardName));
        if (folder.exists()) {
            setErrorMessage("Project already contains a wizard with that name");
            setPageComplete(false);
            return;
        }
        setPageComplete(true);
    }

    public String getWizardName() {
        return text.getText();
    }

    public IProject getProject() {
        Object selection = ((IStructuredSelection)table.getSelection()).getFirstElement();
        return (IProject) selection;
    }
    
    public IContainer getWizardFolder() {
        IProject project = getProject();
        return project.getFolder(new Path(TemplateBasedWizard.TEMPLATES_FOLDER_NAME + "/" + getWizardName()));
    }
}
