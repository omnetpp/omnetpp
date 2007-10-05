package com.simulcraft.test.gui.access;

import org.eclipse.swt.widgets.Button;

import com.simulcraft.test.gui.core.InUIThread;

public class ButtonAccess extends ControlAccess
{
	public ButtonAccess(Button control) {
		super(control);
	}
	
	@Override
	public Button getControl() {
		return (Button)widget;
	}

	@InUIThread
	public void activateWithMouseClick() {
		assertEnabled();
		click();
	}
}
