package org.omnetpp.common.ui;

import org.eclipse.swt.SWT;

/**
 * Preferred values must be higher than the minimum values. SWT.DEFAULT means value is not provided. 
 * 
 * @author levy
 */
public class SizeConstraint {
	public int minimumWidth = SWT.DEFAULT;

	public int minimumHeight = SWT.DEFAULT;

	public int preferredWidth = SWT.DEFAULT;

	public int preferredHeight = SWT.DEFAULT;
	
	// may be extended with maximum later
}
