package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;

import com.simulcraft.test.gui.core.UIStep;

public class ButtonAccess extends ControlAccess
{
	public ButtonAccess(Button control) {
		super(control);
	}
	
	@Override
	public Button getControl() {
		return (Button)widget;
	}

	@UIStep
	public void selectWithMouseClick() {
		assertEnabled();
		click();
	}
	
	@UIStep
	public void assertIsCheckbox() {
		Assert.assertTrue("Checkbox expected", (getControl().getStyle() & SWT.CHECK) != 0);
	}
	
    @UIStep
	public void ensureSelection(boolean selected) {
		assertIsCheckbox();
		Button button = getControl();
		if (button.getSelection() != selected) {
			assertEnabled();
			click();
		}
	}
}
