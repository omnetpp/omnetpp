package org.omnetpp.scave.editors.forms;

import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.widgets.Composite;

/**
 * Common interface of the forms for editing scave objects.
 * 
 * EditDialog and NewScaveObjectWizard uses this interface.
 * ScaveObjectEditFormFactory can be used to create the
 * edit forms for scave objects.
 *
 * @author tomi
 */
public interface IScaveObjectEditForm {
	
	/**
	 * Key for accessing the selected object as a form parameter.
	 */
	final String PARAM_SELECTED_OBJECT = "SelectedObject";
	
	/**
	 * Returns the string displayed in the title are of the dialog.
	 */
	String getTitle();
	
	/**
	 * Returns the string displayed below the title.
	 */
	String getDescription();
	
	/**
	 * Adds the controls of the form to the panel.
	 */
	void populatePanel(Composite panel);

	/**
	 * Returns the features edited on this form.
	 */
	EStructuralFeature[] getFeatures();
	
	/**
	 * Returns the number of the features edited on this form.
	 */
	int getFeatureCount(); // TODO remove this method
	
	/**
	 * Returns the value of the control associated with the specified feature. 
	 */
	Object getValue(EStructuralFeature feature);
	
	/**
	 * Sets the value of the control associated with the specified feature. 
	 */
	void setValue(EStructuralFeature feature, Object value);
}
