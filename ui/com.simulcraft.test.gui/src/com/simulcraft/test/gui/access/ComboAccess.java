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
	public String[] getComboItems() {
	    return getCombo().getItems();
	}
	
	@InUIThread
	public String getTextContent() {
		return getCombo().getText();
	}

	public void assertEditable() {
		Assert.assertTrue("combo is readonly", (getCombo().getStyle() & SWT.READ_ONLY) == 0);
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
        Combo combo = getCombo();
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
        pressKey(SWT.HOME);

        int index = findString(getComboItems(), content);
        for (int i=0; i<index; i++)
            pressKey(SWT.ARROW_DOWN);
        pressKey(SWT.CR);
        
        assertTextContent(content);
    }
    

}
