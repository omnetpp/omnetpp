package com.simulcraft.test.gui.access;

import com.simulcraft.test.gui.core.InBackgroundThread;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Table;

public class ContentAssistAccess extends TableAccess {

    public ContentAssistAccess(Table control) {
        super(control);
    }

    @InBackgroundThread
    public void chooseWithMouse(String label) {
        findTableItemByContent(label).reveal().doubleClick();
    }

    @InBackgroundThread
    public void chooseWithKeyboard(String label) {
        TableItemAccess item = findTableItemByContent(label);

        // press Down until we get there (wraps if we reach the bottom)
        while (getSelection().length!=1 || getSelection()[0].getWidget()!=item.getWidget()) {
            Access.sleep(0.1);
            pressKey(SWT.ARROW_DOWN);  // usually not visible in the animation because the text is the focus control!
        }
        pressEnter();
    }
}
