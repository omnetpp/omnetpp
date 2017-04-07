/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.wizard;

import java.util.Collection;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.edit.command.CommandParameter;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.omnetpp.common.util.StatusUtil;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.forms.IScaveObjectEditForm;
import org.omnetpp.scave.editors.forms.ScaveObjectEditFormFactory;

/**
 * Two-page wizard for creating new scave objects.
 *
 * First page offers the types of the children can be added to the
 * specified parent.
 *
 * Second page contains a form to edit the new object.
 *
 * @author tomi
 */
public class NewScaveObjectWizard extends Wizard {

    private ScaveEditor editor;
    private EditingDomain domain;
    private EObject parent;
    private CommandParameter[] childDescriptors;
    private CommandParameter newChildDescriptor;

    private TypeSelectionWizardPage typeSelectionPage;
    private EditFieldsWizardPage editFieldsPage;

    public NewScaveObjectWizard(ScaveEditor editor, EObject parent) {
        this.editor = editor;
        this.domain = editor.getEditingDomain();
        this.parent = parent;
        setWindowTitle("New Object");
        setHelpAvailable(false);
        setNeedsProgressMonitor(false);
        Collection<?> descriptors = domain.getNewChildDescriptors(parent, null);
        childDescriptors = (CommandParameter[])descriptors.toArray(new CommandParameter[descriptors.size()]);
        Assert.isTrue(childDescriptors.length > 0);
    }

    public NewScaveObjectWizard(ScaveEditor editor, EObject parent, int index, EObject element) {
        this.editor = editor;
        this.domain = editor.getEditingDomain();
        this.parent = parent;
        setWindowTitle("New Object");
        setHelpAvailable(false);
        setNeedsProgressMonitor(false);

        Collection<?> descriptors = domain.getNewChildDescriptors(parent, null);
        childDescriptors = (CommandParameter[])descriptors.toArray(new CommandParameter[descriptors.size()]);
        Assert.isTrue(childDescriptors.length > 0);

        newChildDescriptor = new CommandParameter(parent, null, element, index);
    }

    /**
     * This is result of the wizard.
     */
    public CommandParameter getNewChildDescriptor() {
        return newChildDescriptor;
    }

    private void setNewChildDescriptor(CommandParameter newChildDescriptor) {
        if (this.newChildDescriptor != newChildDescriptor) {
            this.newChildDescriptor = newChildDescriptor;
            editFieldsPage.clearControl();
        }
    }

    @Override
    public void addPages() {
        if (newChildDescriptor == null) {
            if (childDescriptors.length > 1) {
                typeSelectionPage = new TypeSelectionWizardPage("Select type", childDescriptors);
                addPage(typeSelectionPage);
            }
            else if (childDescriptors.length == 1) {
                newChildDescriptor = childDescriptors[0];
            }
        }

        editFieldsPage = new EditFieldsWizardPage("Set Attributes");
        addPage(editFieldsPage);
    }

    /**
     * Delay the creation of controls on the edit page.
     */
    @Override
    public void createPageControls(Composite pageContainer) {
        if (typeSelectionPage != null)
            typeSelectionPage.createControl(pageContainer);
        else
            editFieldsPage.createControl(pageContainer);
    }

    @Override
    public boolean performCancel() {
        if (newChildDescriptor != null) {
            newChildDescriptor = null;
        }
        return true;
    }

    @Override
    public boolean performFinish() {
        if (newChildDescriptor != null) {
            editFieldsPage.setNewChildFeatures();
            return true;
        }
        return false;
    }

    /**
     * This is the first page of the wizard.
     *
     * The user can select the type of the new object here.
     *
     */
    class TypeSelectionWizardPage extends WizardPage {

        private CommandParameter[] childrenDescriptors;

        protected TypeSelectionWizardPage(String pageName, CommandParameter[] childrenDescriptors) {
            super(pageName);
            setTitle("Select Type");
            setDescription("Select type of object to be created");
            setPageComplete(false);

            // set descriptors of new children
            this.childrenDescriptors = childrenDescriptors;
        }

        /**
         * Creates the controls of this page.
         */
        public void createControl(Composite parentComposite) {
            Group radioGroup = new Group(parentComposite, SWT.NONE);
            radioGroup.setText("Type");
            radioGroup.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
            radioGroup.setLayout(new GridLayout());
            for (int i = 0; i < childrenDescriptors.length; ++i) {
                final Button radio = new Button(radioGroup, SWT.RADIO);
                EClass eClass = ((EObject)childrenDescriptors[i].value).eClass();
                radio.setText(eClass.getName());
                radio.setVisible(true);
                radio.setLayoutData(new GridData());
                final int index = i;
                radio.addSelectionListener(new SelectionAdapter() {
                    public void widgetSelected(SelectionEvent e) {
                        handleSelectionChanged(radio, index);
                    }
                });
            }
            setControl(radioGroup);
        }

        /**
         * This method called when the selection changed.
         * It creates a new object with the selected type.
         */
        private void handleSelectionChanged(Button radio, int index) {
            if (radio.getSelection())
                setNewChildDescriptor(childrenDescriptors[index]);
            setPageComplete(radio.getSelection());
        }

    }

    /**
     * This is the second page of the wizard.
     *
     * The user can set the properties of the new child here.
     * It contains the same controls as the Edit page of child.
     */
    class EditFieldsWizardPage extends WizardPage {

        IScaveObjectEditForm form;
        IScaveObjectEditForm.Listener listener;

        protected EditFieldsWizardPage(String pageName) {
            super(pageName);
            setPageComplete(false);
            listener = new IScaveObjectEditForm.Listener() {
                public void editFormChanged(IScaveObjectEditForm form) {
                    updateButtonsAndErrorMessage();
                }
            };
        }

        /**
         * Creates the controls of this page.
         */
        public void createControl(Composite parentComposite) {
            ScaveObjectEditFormFactory factory = ScaveObjectEditFormFactory.instance();
            form = factory.createForm(newChildDescriptor.getEValue(),
                                        parent, newChildDescriptor.getEStructuralFeature(), newChildDescriptor.getIndex(),
                                        null, editor.getResultFileManager());
            setTitle(form.getTitle());
            setDescription(form.getDescription());

            Composite panel = new Composite(parentComposite, SWT.NONE);
            GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
            panel.setLayoutData(gridData);
            form.populatePanel(panel);
            setControl(panel);

            form.addChangeListener(listener);
            updateButtonsAndErrorMessage();
        }

        /**
         * This method called when the type of the child has changed,
         * so a new form has to be generated.
         * It clears the control of this page, so <code>createControl()</code>
         * is called again to generate it.
         */
        void clearControl() {
            if (form != null)
                form.removeChangeListener(listener);
            setControl(null);
            setPageComplete(false);
        }

        /**
         * Update button state and error message.
         * Called when a form field changes.
         */
        void updateButtonsAndErrorMessage() {
            IStatus status = form.validate();
            setPageComplete(!status.matches(IStatus.ERROR));
            setErrorMessage(status);
        }

        private void setErrorMessage(IStatus status) {
            String message = null;
            if (status.matches(IStatus.ERROR)) {
                message = "There are errors:\n\t";
                IStatus error = StatusUtil.getFirstDescendantWithSeverity(status, IStatus.ERROR);
                Assert.isNotNull(error);
                message += status.isMultiStatus() ? status.getMessage() + ": " : "";
                message += error.getMessage();
            }
            else if (status.matches(IStatus.WARNING)) {
                message = "There are warnings:\n\t";
                IStatus warning = StatusUtil.getFirstDescendantWithSeverity(status, IStatus.WARNING);
                Assert.isNotNull(warning);
                message += status.isMultiStatus() ? status.getMessage() + ": " : "";
                message += warning.getMessage();
            }
            setErrorMessage(message);
        }

        /**
         * Copies the values on the form to the created object.
         * Called when the Finish button pressed.
         */
        void setNewChildFeatures() {
            EObject newChild = (EObject)newChildDescriptor.value;
            EStructuralFeature[] features = form.getFeatures();
            for (int i = 0; i < features.length; ++i) {
                EStructuralFeature feature = features[i];
                newChild.eSet(feature, form.getValue(feature));
            }
        }
    }
}
