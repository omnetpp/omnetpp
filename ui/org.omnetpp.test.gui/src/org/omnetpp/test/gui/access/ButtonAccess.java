package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Button;

public class ButtonAccess extends ControlAccess<Button>
{
	public ButtonAccess(Button control) {
		super(control);
	}

	public void activateWithMouseClick() {
		assertEnabled();
		clickControl(LEFT_MOUSE_BUTTON);
	}
}
