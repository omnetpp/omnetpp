/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.autoimport;

import java.io.File;

import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ComboViewer;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;


/**
 * Offer creating a project in the given directory or one of its ancestors,
 * based on user's choice (combo box).
 *
 * @author Andras
 */
public class CreateProjectDialog extends TitleAreaDialog {
    // dialog config
    private String message;
    private IProjectDescription[] descriptions;

    // widgets
    private Text projectNameText;
    private ComboViewer combo;
    private Button okButton;

    // result
    private IProjectDescription descriptionResult;

    private boolean insideValidation;

    public CreateProjectDialog(Shell parentShell, String message, IProjectDescription[] descriptions) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.RESIZE);
        this.descriptions = descriptions;
        this.message = message;
    }

    @Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        shell.setText("Create Project");
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        setTitle("Create Project");
        setMessage(message);

        Composite dialogArea = (Composite) super.createDialogArea(parent);

        Composite composite = new Composite(dialogArea, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(2,false));

        createLabel(composite, "Project root:", parent.getFont());
        combo = new ComboViewer(composite, SWT.BORDER | SWT.READ_ONLY);
        combo.getCombo().setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        createLabel(composite, "Name:", parent.getFont());
        projectNameText = new Text(composite, SWT.BORDER);
        projectNameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // create input
        IProjectDescription firstExistingDescription = null;
        for (IProjectDescription description : descriptions)
            if (firstExistingDescription==null && existsOnDisk(description))
                firstExistingDescription = description;

        if (firstExistingDescription != null) {
            Label l = createLabel(composite, "* Project description file (.project) found on disk\n", parent.getFont());
            ((GridData)l.getLayoutData()).horizontalSpan = 2;
        }

        // fill combo
        combo.setContentProvider(new ArrayContentProvider());
        combo.setLabelProvider(new LabelProvider() {
            @Override
            @SuppressWarnings("deprecation")
            public String getText(Object element) {
                IProjectDescription desc = (IProjectDescription)element;
                String location = desc.getLocation().toOSString();
                return existsOnDisk(desc) ? location + " *" : location;
            }
        });
        combo.setInput(descriptions);

        IProjectDescription initialSelection = firstExistingDescription;
        if (initialSelection == null)
            initialSelection = descriptions[0];
        combo.setSelection(new StructuredSelection(initialSelection));

        // set up validation on content changes
        combo.getCombo().addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                comboSelectionChanged();
                validateDialogContents();
            }
        });
        projectNameText.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                validateDialogContents();
            }
        });

        comboSelectionChanged();  // initialize project name

        // note: do initial validation when OK button is already created, from createButtonsForButtonBar()

        // focus on first field
        combo.getCombo().setFocus();

        return composite;
    }

    @SuppressWarnings("deprecation")
    private boolean existsOnDisk(IProjectDescription desc) {
        return new File(desc.getLocation().append(IProjectDescription.DESCRIPTION_FILE_NAME).toOSString()).exists();
    }

    protected static Label createLabel(Composite parent, String text, Font font) {
        Label label = new Label(parent, SWT.WRAP);
        label.setText(text);
        label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false));
        label.setFont(font);
        return label;
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        // create OK and Cancel buttons by default
        okButton = createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);

        // do this here because setting the text will set enablement on the OK button
        validateDialogContents();
    }

    protected void comboSelectionChanged() {
        IProjectDescription description = getComboSelection();

        projectNameText.setText(description.getName());
        projectNameText.selectAll();
    }

    protected void validateDialogContents() {
        // prevent notification loops
        if (insideValidation)
            return;
        insideValidation = true;

        boolean hasError = false;

        IWorkspace workspace = ResourcesPlugin.getWorkspace();

        String projectName = projectNameText.getText();
        IStatus status = workspace.validateName(projectName, IResource.PROJECT);
        if (status.getSeverity() == IStatus.ERROR) {
            setErrorMessage(status.getMessage());
            hasError = true;
        }
        else if (workspace.getRoot().getProject(projectName).exists()) {
            setErrorMessage("A project named '" + projectName + "' already exists.");
            hasError = true;
        }
        else {
            // modify name in the project description
            IProjectDescription description = getComboSelection();
            description.setName(projectName);
        }

        if (!hasError)
            setErrorMessage(null);

        if (okButton != null)  // it is initially null
            okButton.setEnabled(!hasError);

        insideValidation = false;
    }

    protected IProjectDescription getComboSelection() {
        IStructuredSelection selection = (IStructuredSelection)combo.getSelection();
        IProjectDescription description = (IProjectDescription) selection.getFirstElement();
        return description;
    }

    @Override
    protected void okPressed() {
        // save dialog state into variables, so that client can retrieve them after the dialog was disposed
        descriptionResult = getComboSelection();
        super.okPressed();
    }

    public IProjectDescription getResult() {
        return descriptionResult;
    }
}
