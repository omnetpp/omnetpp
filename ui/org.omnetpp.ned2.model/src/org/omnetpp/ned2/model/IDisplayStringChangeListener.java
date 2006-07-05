package org.omnetpp.ned2.model;

import org.omnetpp.common.displaymodel.IDisplayString.Prop;

public interface IDisplayStringChangeListener {

	/**
	 * Notifies that a display string change has occured. Can be used to pass the modified
	 * display string back to the model
	 * @param changedProp The property that changed or NULL if it cannot be identified
	 */
	void propertyChanged(Prop changedProp);

}