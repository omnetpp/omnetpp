package com.simulcraft.test.gui.access;

import org.eclipse.swt.widgets.Label;

public class LabelAccess extends ControlAccess
{
	public LabelAccess(Label control) {
		super(control);
	}
	
    @Override
	public Label getControl() {
		return (Label)widget;
	}
}
