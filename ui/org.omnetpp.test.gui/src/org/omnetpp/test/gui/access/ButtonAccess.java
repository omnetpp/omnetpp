package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Button;
import org.omnetpp.test.gui.InUIThread;

public class ButtonAccess extends ControlAccess<Button>
{
	public ButtonAccess(Button control) {
		super(control);
	}

	@InUIThread
	public void activateWithMouseClick() {
		assertEnabled();
		click();
	}
}
