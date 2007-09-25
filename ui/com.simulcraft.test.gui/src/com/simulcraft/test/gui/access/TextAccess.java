package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Text;

import com.simulcraft.test.gui.core.InUIThread;

public class TextAccess extends ControlAccess
{
	public TextAccess(Text text) {
		super(text);
	}

	public Text getText() {
		return (Text)widget;
	}
	
	@InUIThread
	public String getTextContent() {
		return getText().getText();
	}

	public void assertEditable() {
		Assert.assertTrue("text control is readonly", (widget.getStyle() & SWT.READ_ONLY) == 0);
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
