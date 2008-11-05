package org.omnetpp.scave.editors.ui;

import java.util.Map;

import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.forms.IScaveObjectEditForm;
import org.omnetpp.scave.editors.forms.ScaveObjectEditFormFactory;
import org.omnetpp.scave.engine.ResultFileManager;

/**
 * This is the edit dialog for scave model objects.
 * 
 * It receives an object and optionally a set of features to be edited
 * (defaults to all editable features).
 * It replies with the changed values.
 * 
 * @author tomi
 */
public class EditDialog extends TitleAreaDialog {
	
	private ScaveEditor editor;
	private EObject object;
	private EStructuralFeature[] features; //XXX not really used, remove on the long term?
	private IScaveObjectEditForm form;
	private Object[] values;
	
	
	/**
	 * Creates the dialog. The form in the dialog will be chosen based on the object type.
	 * The form can be customized via the formParameters, like which page of the 
	 * chart dialog should be displayed by default.
	 *    
	 * @param parentShell    the parent shell
	 * @param object         object to be edited
	 * @param editor         the editor 
	 * @param formParameters key-value pairs understood by the form; may be null
	 */
	public EditDialog(Shell parentShell, EObject object, ScaveEditor editor, Map<String,Object> formParameters) {
		this(parentShell, object, null, editor, formParameters);
	}
	
	public EditDialog(Shell parentShell, EObject object, EStructuralFeature[] features, ScaveEditor editor, Map<String,Object> formParameters) {
		super(parentShell);
		setShellStyle(getShellStyle() | SWT.RESIZE);
		this.editor = editor;
		this.object = object;
		this.features = features;
		this.form = createForm(object, features, editor.getResultFileManager(), formParameters);
	}
	
	@Override
	protected IDialogSettings getDialogBoundsSettings() {
	    return UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName());
	}
	
	public EStructuralFeature[] getFeatures() {
		return form.getFeatures();
	}
	
	public static EStructuralFeature[] getEditableFeatures(EObject object, ScaveEditor editor) {
		try {
			IScaveObjectEditForm form = createForm(object, null, editor.getResultFileManager(), null);
			return form.getFeatures();
		}
		catch (IllegalArgumentException e) {
			return new EStructuralFeature[0];
		}
	}
	
	public Object getValue(int index) {
		return values[index];
	}
	
	@Override
	protected void configureShell(Shell newShell) {
		super.configureShell(newShell);
		newShell.setText("Edit " + object.eClass().getName());
	}

	@Override
	protected void buttonPressed(int buttonId) {
		if (IDialogConstants.APPLY_ID == buttonId)
			applyPressed();
		else
			super.buttonPressed(buttonId);
	}

	@Override
	protected void createButtonsForButtonBar(Composite parent) {
		createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
		createButton(parent, IDialogConstants.APPLY_ID, IDialogConstants.APPLY_LABEL, false);
		createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
	}

	@Override
	protected Control createDialogArea(Composite parent) {
		Composite composite = (Composite)super.createDialogArea(parent);
		
		Composite panel = new Composite(composite, SWT.NONE);
		panel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		
		setTitle(form.getTitle());
		setMessage(form.getDescription());
		form.populatePanel(panel);
		features = form.getFeatures();
		for (int i = 0; i < features.length; ++i) {
			if (object.eIsSet(features[i]))
				form.setValue(features[i], object.eGet(features[i]));
		}
		return composite;
	}
	
	@Override
	protected void okPressed() {
		applyChanges();
		super.okPressed();
	}
	
	protected void applyPressed() {
		applyChanges();
	}
	
	private void applyChanges() {
		if (features == null)
			return;
		
		values = new Object[features.length];
		for (int i = 0; i < values.length; ++i) {
			values[i] = form.getValue(features[i]);
		}
		
		CompoundCommand command = new CompoundCommand("Edit");
		for (int i = 0; i < features.length; ++i) {
			Object oldValue = object.eGet(features[i]);
			Object newValue = getValue(i);
			boolean isDirty = oldValue == null && newValue != null ||
							  oldValue != null && !oldValue.equals(newValue);
			if (isDirty) {
				command.append(SetCommand.create(
					editor.getEditingDomain(),
					object,
					features[i],
					newValue));
			}
		}
		editor.executeCommand(command);
	}
	
	private static IScaveObjectEditForm createForm(EObject object, EStructuralFeature[] features, ResultFileManager manager, Map<String,Object> formParameters) {
		//XXX remove features[] parameter! 
		return ScaveObjectEditFormFactory.instance().createForm(object, formParameters, manager);
	}
}
