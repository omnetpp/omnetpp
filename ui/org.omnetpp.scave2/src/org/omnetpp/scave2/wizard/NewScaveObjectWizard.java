package org.omnetpp.scave2.wizard;

import java.util.Collection;

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
import org.omnetpp.scave.model.ScaveModelFactory;
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
public class NewScaveObjectWizard extends Wizard {
	
	private EObject parent;
	private EditingDomain domain;
	private EObject newScaveObject;
	
	private TypeSelectionWizardPage typeSelectionPage;
	private EditFieldsWizardPage editFieldsPage;
	
	public NewScaveObjectWizard(EditingDomain domain, EObject parent) {
		this.domain = domain;
		this.parent = parent;
		setWindowTitle("Create new item");
		setHelpAvailable(false);
		setNeedsProgressMonitor(false);
	}
	
	public EObject getNewScaveObject() {
		return newScaveObject;
	}
	
	private void createNewScaveObject(EClass type) {
		if (newScaveObject == null || !newScaveObject.eClass().equals(type)) {
			newScaveObject = ScaveModelFactory.eINSTANCE.create(type);
			editFieldsPage.clearControl();
		}
	}
	
	@Override
	public void addPages() {
		EClass[] types = getNewChildTypes(parent);
		typeSelectionPage = new TypeSelectionWizardPage("Select type", types);
		addPage(typeSelectionPage);
		
		editFieldsPage = new EditFieldsWizardPage("Set attributes");
		addPage(editFieldsPage);
	}

	private EClass[] getNewChildTypes(EObject parent) {
		Collection descriptors = domain.getNewChildDescriptors(parent, null);
		EClass[] types = new EClass[descriptors.size()];
		int i = 0;
		for (Object descriptor : descriptors) {
			types[i++] = ((EObject)((CommandParameter)descriptor).value).eClass();
		}
		return types;
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
		newScaveObject = null;
		return true;
	}

	@Override
	public boolean performFinish() {
		if (newScaveObject != null) {
			EStructuralFeature[] features = editFieldsPage.getFeatures();
			for (int i = 0; i < features.length; ++i) {
				newScaveObject.eSet(features[i], editFieldsPage.getValue(i));
			}
		}
		return newScaveObject != null;
	}
	
	
	class TypeSelectionWizardPage extends WizardPage {
		
		private EClass[] types;

		protected TypeSelectionWizardPage(String pageName, EClass[] types) {
			super(pageName);
			setTitle("Select type");
			setDescription("Select the type of the object to be created");
			setPageComplete(false);
			this.types = types;
		}
		

		public void createControl(Composite parentComposite) {
			Composite radioPanel = new Composite(parentComposite, SWT.NONE);
			radioPanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
			radioPanel.setLayout(new GridLayout());
			for (int i = 0; i < types.length; ++i) {
				final Button radio = new Button(radioPanel, SWT.RADIO);
				final int index = i;
				radio.setText(types[i].getName());
				radio.setVisible(true);
				radio.setLayoutData(new GridData());
				radio.addSelectionListener(new SelectionAdapter() {
					public void widgetSelected(SelectionEvent e) {
						setNewNodeType(radio, index);
					}
				});
			}
			setControl(radioPanel);
		}
		
		private void setNewNodeType(Button radio, int index) {
			NewScaveObjectWizard wizard = (NewScaveObjectWizard)getWizard();
			EClass selectedType = radio.getSelection() ? types[index] : null;
			if (selectedType != null)
				wizard.createNewScaveObject(selectedType);
			setPageComplete(selectedType != null);
		}
	}
	
	class EditFieldsWizardPage extends WizardPage {
		
		IScaveObjectEditForm form;
		
		protected EditFieldsWizardPage(String pageName) {
			super(pageName);
			setTitle("Edit fields");
			setDescription("");
		}
		
		public void createControl(Composite parentComposite) {
			Composite panel = new Composite(parentComposite, SWT.NONE);
			panel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
			form = ScaveObjectEditFormFactory.instance().createForm(getNewScaveObject());
			form.populatePanel(panel);
			setControl(panel);
		}
		
		public EStructuralFeature[] getFeatures() {
			return form.getFeatures();
		}
		
		public Object getValue(int index) {
			return form.getValue(index);
		}
		
		public void clearControl() {
			setControl(null);
		}
	}
}
