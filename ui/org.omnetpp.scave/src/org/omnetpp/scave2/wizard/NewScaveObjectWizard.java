package org.omnetpp.scave2.wizard;

import java.util.Arrays;
import java.util.Collection;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.edit.command.CommandParameter;
import org.eclipse.emf.edit.command.CreateChildCommand;
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
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.ui.IScaveObjectEditForm;
import org.omnetpp.scave2.editors.ui.ScaveObjectEditFormFactory;

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
// XXX: The new node is temporarily added to the parent, because
//      edit forms do not work if the edited object is not added to the analysis.
//      The new node is removed when the dialog is closed.
public class NewScaveObjectWizard extends Wizard {
	
	private ScaveEditor editor;
	private EditingDomain domain;
	private EObject parent;
	private CommandParameter newChildDescriptor;
	
	// command to add the new node temporarily (see class comment above)
	private Command addNewChildCommand;
	
	private TypeSelectionWizardPage typeSelectionPage;
	private EditFieldsWizardPage editFieldsPage;
	
	public NewScaveObjectWizard(ScaveEditor editor, EObject parent) {
		this.editor = editor;
		this.domain = editor.getEditingDomain();
		this.parent = parent;
		setWindowTitle("Create new item");
		setHelpAvailable(false);
		setNeedsProgressMonitor(false);
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
			
			// remove previous child, add new child
			removeNewChild();
			addNewChildCommand = CreateChildCommand.create(domain, parent, newChildDescriptor, Arrays.asList(parent));
			if (addNewChildCommand.canExecute())
				addNewChildCommand.execute();
		}
	}
	
	@Override
	public void addPages() {
		typeSelectionPage = new TypeSelectionWizardPage("Select type");
		addPage(typeSelectionPage);
		
		editFieldsPage = new EditFieldsWizardPage("Set attributes");
		addPage(editFieldsPage);
	}

	/**
     * Delay the creation of controls on the edit page.
     */
	@Override
	public void createPageControls(Composite pageContainer) {
    	typeSelectionPage.createControl(pageContainer);
    }
	
	@Override
	public boolean performCancel() {
		removeNewChild();
		if (newChildDescriptor != null) {
			newChildDescriptor = null;
		}
		return true;
	}

	@Override
	public boolean performFinish() {
		removeNewChild();
		if (newChildDescriptor != null) {
			editFieldsPage.setNewChildFeatures();
			return true;
		}
		return false;
	}
	
	private void removeNewChild() {
		try {
			if (addNewChildCommand != null && addNewChildCommand.canUndo())
				addNewChildCommand.undo();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * This is the first page of the wizard.
	 * 
	 * The user can select the type of the new object here.
	 *
	 */
	class TypeSelectionWizardPage extends WizardPage {
		
		private CommandParameter[] childrenDescriptors;

		protected TypeSelectionWizardPage(String pageName) {
			super(pageName);
			setTitle("Select type");
			setDescription("Select the type of the object to be created");
			setPageComplete(false);
			
			// set descriptors of new children
			Collection descriptors = domain.getNewChildDescriptors(parent, null);
			childrenDescriptors = (CommandParameter[])descriptors.toArray(new CommandParameter[descriptors.size()]);
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
		
		protected EditFieldsWizardPage(String pageName) {
			super(pageName);
			setTitle("Edit fields");
			setDescription("");
		}
		
		/**
		 * Creates the controls of this page.
		 */
		public void createControl(Composite parentComposite) {
			Composite panel = new Composite(parentComposite, SWT.NONE);
			panel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
			ScaveObjectEditFormFactory factory = ScaveObjectEditFormFactory.instance();
			form = factory.createForm((EObject)newChildDescriptor.value, editor.getResultFileManager());
			form.populatePanel(panel);
			setControl(panel);
		}
		
		/**
		 * This method called when the type of the child has changed,
		 * so a new form has to be generated.
		 * It clears the control of this page, so <code>createControl()</code>
		 * is called again to generate it.
		 */
		void clearControl() {
			setControl(null);
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
