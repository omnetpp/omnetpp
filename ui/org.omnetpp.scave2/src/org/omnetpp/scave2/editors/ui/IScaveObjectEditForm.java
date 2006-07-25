package org.omnetpp.scave2.editors.ui;

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
	int getFeatureCount();
	
	/**
	 * Returns the value of the control associated with the specified feature. 
	 */
	Object getValue(int featureIndex);
	
	/**
	 * Sets the value of the control associated with the specified feature. 
	 */
	void setValue(int featureIndex, Object value);

	/**
	 * Returns true if the value of the control changed since the last
	 * <code>setValue()</code> call.
	 */
	boolean isDirty(int featureIndex);
}
