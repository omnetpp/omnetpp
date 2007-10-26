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

    @Override
	public Combo getControl() {
		return (Combo)widget;
	}
    
    @InUIThread
    public boolean isEditable() {
    	return (getControl().getStyle() & SWT.READ_ONLY) == 0;
    }
	
    @InUIThread
	public String[] getComboItems() {
	    return getControl().getItems();
	}
	
	@InUIThread
	public String getTextContent() {
		return getControl().getText();
	}

	public void assertEditable() {
		Assert.assertTrue("combo is readonly", isEditable());
	}

    @InUIThread
    public void assertTextContent(String regex) {
        Assert.assertTrue("combo content does not match "+regex, getTextContent().matches(regex));
    }

    @InUIThread
    public void typeOver(String content) {
        assertEnabled();
        assertEditable();
        assertHasFocus();
        Combo combo = getControl();
        pressKey('a', SWT.CONTROL);
        if (combo.getText().length() > 0 && content.equals(""))
            pressKey(SWT.DEL);
        else
            typeIn(content);
    }
    
    @NotInUIThread
    public void clickAndTypeOver(String content) {
        click();
        typeOver(content);
    }

    @NotInUIThread
    public void selectItem(String content) {
        click();
        
        if (isEditable()) {
        	typeOver("");
        	pressKey(SWT.ARROW_DOWN);
        }
        else
        	pressKey(SWT.HOME);

        int index = findString(getComboItems(), content);
        for (int i=0; i<index; i++)
            pressKey(SWT.ARROW_DOWN);

        if (!isEditable())
            pressEnter();
        
        assertTextContent(content);
    }
    
    @InUIThread
    public void assertContainsItem(String regex) {
        Assert.assertTrue("combo does not contain item: " + regex, findString(getComboItems(), regex) != -1);
    }

    @InUIThread
    public void assertNotContainsItem(String regex) {
        Assert.assertTrue("combo contains item: " + regex, findString(getComboItems(), regex) == -1);
    }

}
