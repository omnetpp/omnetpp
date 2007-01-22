package org.omnetpp.common.displaymodel;

import org.omnetpp.common.displaymodel.IDisplayString.Prop;


public interface IDisplayStringChangeListener {

	/**
	 * Notifies that a display string change has occured. Can be used to pass the modified
	 * display string back to the model
	 * @param changedProp The property that changed or NULL if it cannot be identified
	 * @param newValue The new value of the property (if can be determined)
	 * @param oldValue The old value of the property (if can be determined)
	 */
	void propertyChanged(IDisplayString source, Prop changedProp, Object newValue, Object oldValue);

}