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

    @InUIThread
	public void assertEditable() {
		Assert.assertTrue("text control is readonly", (widget.getStyle() & SWT.READ_ONLY) == 0);
	}

    @InUIThread
    public void typeOver(String content) {
        assertEnabled();
        assertEditable();
        assertHasFocus();
        Text text = getText();
        if (text.getSelectionCount() != text.getText().length())
            pressKey('a', SWT.CONTROL);
        typeIn(content);
    }
	
	public void clickAndTypeOver(String content) {
		click();
		typeOver(content);
	}
}
