/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.part.MultiPageEditorPart;

/**
 * Multi-page editor with closable pages.
 *
 * @author Andras
 */
public abstract class MultiPageEditorPartExt extends MultiPageEditorPart {

    /**
     * Adds a closable page to the multi-page editor. This is a variant of
     * addPage(int index, Control control).
     */
    public void addClosablePage(int index, Control control) {
        // hack: add it in the normal way, then replace CTabItem with one with SWT.CLOSE set
        super.addPage(index, control);
        CTabFolder ctabFolder = (CTabFolder) control.getParent();
        ctabFolder.getItem(index).dispose();
        CTabItem item = new CTabItem(ctabFolder, SWT.CLOSE, index);
        item.setControl(control);

        // CTabItem does not dispose the page contents widget, only calls
        // setVisible(false) on it; so we have to do it here
        item.addDisposeListener(new DisposeListener() {
            public void widgetDisposed(DisposeEvent e) {
                Control client = ((CTabItem)e.widget).getControl();
                pageClosed(client);
                client.dispose();
            }
        });

    }

    public boolean isClosablePage(Control control) {
        CTabFolder ctabFolder = (CTabFolder) control.getParent();
        for (int i = 0; i < ctabFolder.getItemCount(); i++) {
            CTabItem item = ctabFolder.getItem(i);
            if (item.getControl() == control)
                return item.getShowClose();
        }
        throw new RuntimeException("control is not an editor page: " + control.toString());
    }
    
    /**
     * Closes the page of the multi-page editor page which holds
     * the given control. The request is ignored if the control is
     * not found among the pages.
     */
    public void removePage(Control control) {
        int index = findPage(control);
        if (index!=-1)
            removePage(index);
    }

    /**
     * Returns the index of the given multi-page editor page. Returns -1 if not found.
     * @param control  the page control (typically some composite)
     */
    public int findPage(Control control) {
        for (int i=0; i<getPageCount(); i++)
            if (getControl(i)==control)
                return i;
        return -1;
    }

    /**
     * Called back when a closable page gets closed in the editor.
     * @param control  the page control (typically some composite)
     */
    protected abstract void pageClosed(Control control);
}
