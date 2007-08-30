package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Label;

public class LabelAccess extends ControlAccess
{
	public LabelAccess(Label control) {
		super(control);
	}
	
	public Label getLabel() {
		return (Label)widget;
	}
}
