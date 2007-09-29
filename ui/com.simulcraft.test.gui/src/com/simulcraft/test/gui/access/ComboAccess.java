package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Combo;

import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.core.NotInUIThread;

public class ComboAccess extends ControlAccess
{
	public ComboAccess(Combo combo) {
		super(combo);
	}

	public Combo getCombo() {
		return (Combo)widget;
	}
	
	@InUIThread
	public String getTextContent() {
		return getCombo().getText();
	}

	public void assertEditable() {
		Assert.assertTrue("combo is readonly", (getCombo().getStyle() & SWT.READ_ONLY) == 0);
	}
	
    @InUIThread
    public void typeOver(String content) {
        assertEnabled();
        assertEditable();
        assertHasFocus();
        pressKey('a', SWT.CONTROL);
        typeIn(content);
    }
    
    @NotInUIThread
    public void clickAndTypeOver(String content) {
        click();
        typeOver(content);
    }
}
