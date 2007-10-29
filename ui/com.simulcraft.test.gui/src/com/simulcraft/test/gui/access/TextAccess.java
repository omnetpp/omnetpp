package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Text;

import com.simulcraft.test.gui.core.UIStep;

public class TextAccess extends ControlAccess
{
	public TextAccess(Text text) {
		super(text);
	}

    @Override
	public Text getControl() {
		return (Text)widget;
	}
	
	@UIStep
	public String getTextContent() {
		return getControl().getText();
	}

    @UIStep
	public void assertEditable() {
		Assert.assertTrue("text control is readonly", (widget.getStyle() & SWT.READ_ONLY) == 0);
	}

    @UIStep
    public void assertTextContent(String regex) {
        Assert.assertTrue("text control content does not match "+regex, getTextContent().matches(regex));
    }

    
    @UIStep
    public void typeOver(String content) {
        assertEnabled();
        assertEditable();
        assertHasFocus();
        Text text = getControl();
        if (text.getSelectionCount() != text.getText().length()) {
            pressKey('a', SWT.CONTROL); // -- doesn't appear to work everywhere, e.g. in the inifile editor form pages
//            pressKey(SWT.END);
//            pressKey(SWT.HOME, SWT.SHIFT);
        }
        if (text.getText().length() > 0 && content.equals(""))
            pressKey(SWT.DEL);
        else
            typeIn(content);
    }
	
	public void clickAndTypeOver(String content) {
		click();
		typeOver(content);
	}
}
