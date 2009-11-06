package org.omnetpp.common.wizard;

import org.eclipse.swt.widgets.Control;

/**
 * For extending classes to get to work with SWTDataUtil.getValueFromControl()/
 * writeValueIntoControl() methods.
 * 
 * @author Andras
 */
public interface IWidgetAdapter {
	boolean supportsControl(Control control);
	Object getValueFromControl(Control control);
	void writeValueIntoControl(Control control, Object value);
}
