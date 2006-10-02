package org.omnetpp.scave2.editors.ui;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * This is the edit dialog for scave model objects.
 * 
 * It receives an object and optionally a set of features to be edited
 * (defaults to all editable features).
 * It responses with the changed values.
 * 
 * @author tomi
 */
public class EditDialog extends TitleAreaDialog {

	private ScaveEditor editor;
	private EObject object;
	private EStructuralFeature[] features;
	private IScaveObjectEditForm form;
	private Object[] values;
	
	
	public EditDialog(
			Shell parentShell,
			EObject object,
			ScaveEditor editor) {
		this(parentShell, object, null, editor);
	}
	
	public EditDialog(
			Shell parentShell,
			EObject object,
			EStructuralFeature[] features,
			ScaveEditor editor) {
		super(parentShell);
		setShellStyle(getShellStyle() | SWT.RESIZE);
		this.editor = editor;
		this.object = object;
		this.features = features;
		this.form = createForm(object, features, editor.getResultFileManager());
	}
	
	
	
	public EStructuralFeature[] getFeatures() {
		return form.getFeatures();
	}
	
	public static EStructuralFeature[] getEditableFeatures(EObject object, ScaveEditor editor) {
		IScaveObjectEditForm form = createForm(object, null, editor.getResultFileManager());
		return form.getFeatures();
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
	protected Control createDialogArea(Composite parent) {
		Composite composite = (Composite)super.createDialogArea(parent);
		
		Composite panel = new Composite(composite, SWT.NONE);
		panel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		
		setTitle(form.getTitle());
		setMessage(form.getDescription());
		form.populatePanel(panel);
		features = form.getFeatures();
		for (int i = 0; i < features.length; ++i)
			form.setValue(features[i], object.eGet(features[i]));
		return composite;
	}
	
	@Override
	protected void okPressed() {
		applyChanges();
		super.okPressed();
	}
	
	private void applyChanges() {
		if (features != null) {
			values = new Object[features.length];
			for (int i = 0; i < values.length; ++i) {
				values[i] = form.getValue(features[i]);
			}
		}
	}
	
	private static IScaveObjectEditForm createForm(EObject object, EStructuralFeature[] features, ResultFileManager manager) {
		return features == null ?
					ScaveObjectEditFormFactory.instance().createForm(object, manager) :
					ScaveObjectEditFormFactory.instance().createForm(object, features);
	}
}
