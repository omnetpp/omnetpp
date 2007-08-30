package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Combo;
import org.omnetpp.test.gui.core.InUIThread;

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
	public void clickAndType(String text) {
		assertEnabled();
		assertEditable();
		click(); // focus
		pressKey(SWT.HOME);
		pressKey(SWT.END, SWT.SHIFT);  // select all
		typeIn(text); // typing will replace content
	}
}
