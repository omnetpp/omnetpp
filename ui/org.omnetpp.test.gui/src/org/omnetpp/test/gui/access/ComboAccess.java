package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Combo;
import org.omnetpp.test.gui.core.InUIThread;

public class ComboAccess extends ControlAccess<Combo>
{
	public ComboAccess(Combo combo) {
		super(combo);
	}

	public Combo getCombo() {
		return widget;
	}
	
	@InUIThread
	public String getTextContent() {
		return widget.getText();
	}

	public void assertEditable() {
		Assert.assertTrue("combo is readonly", (widget.getStyle() & SWT.READ_ONLY) == 0);
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
