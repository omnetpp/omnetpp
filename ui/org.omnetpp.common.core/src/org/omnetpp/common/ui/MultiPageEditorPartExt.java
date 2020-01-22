/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.MultiPageEditorPart;

/**
 * Multi-page editor with closable pages.
 *
 * @author Andras
 */
public abstract class MultiPageEditorPartExt extends MultiPageEditorPart {


    public void createPartControl(Composite parent) {
        super.createPartControl(parent);

        CTabFolder folder = getTabFolder();
        folder.addListener(SWT.MouseDown, (Event event) -> {
            switch (event.button) {
                case 2: { // middle click
                    CTabItem item = folder.getItem(new Point(event.x, event.y));
                    if (item != null)
                        item.dispose();
                }
                case 3: { // right click
                    // TODO: show context menu to close tabs other/left/right to the clicked item
                }
                default:
                    // no-op
            }
        });
    }
    /**
     * Adds a closable page to the multi-page editor. This is
     * a variant of
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

    public void addClosablePage(int index, IEditorPart editorPart, IEditorInput editorInput) throws PartInitException {
        // hack: add it in the normal way, then replace CTabItem with one with SWT.CLOSE set
        super.addPage(index, editorPart, editorInput);

        // second hack: super.addPage(index, editor, input); (called above) adds a Composite we don't need, remove it
        removeRootCompositeFromTab(index);

        Control control = getControl(index);
        CTabFolder ctabFolder = (CTabFolder) control.getParent();
        ctabFolder.getItem(index).dispose();
        CTabItem item = new CTabItem(ctabFolder, SWT.CLOSE, index);
        item.setData(editorPart);
        item.setControl(control);

        item.addDisposeListener(new DisposeListener() {
            public void widgetDisposed(DisposeEvent e) {
                CTabItem tabItem = (CTabItem)e.widget;

                Control client = tabItem.getControl();
                pageClosed(client);
                client.dispose();

                IEditorPart nestedEditor = (IEditorPart)tabItem.getData();
                if (nestedEditor != null)
                    nestedEditor.dispose();
            }
        });
    }

    private void removeRootCompositeFromTab(int index) {
        // unfortunately, when adding a closable page as an IEditorPart,
        // the MultiPageEditorPart adds a Composite to the parent,
        // and builds the inner editor within that Composite.
        // We don't need that intermediate container, in fact, it gets in the way.
        // So we get rid of it by putting the FormEditor itself in its place,
        // as a direct child of the CTabFolder (like all the fixed pages).
        Composite toDispose = (Composite)getControl(index); // this is the unnecessary container
        CTabFolder tabFolder = getTabFolder();
        Assert.isTrue(toDispose.getParent() == tabFolder);

        Assert.isTrue(toDispose.getChildren().length == 1);
        Control content = toDispose.getChildren()[0]; // this is the FormEditorPage

        content.setParent(tabFolder);
        setControl(index, content);

        Assert.isTrue(toDispose.getChildren().length == 0);
        toDispose.dispose();
    }

    public boolean isClosablePage(int page) {
        CTabItem item = ((CTabFolder)getControl(page).getParent()).getItem(page);
        return item.getShowClose();
    }

    public boolean isClosablePage(Control control) {
        if (control == null)
            return false;
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

    protected CTabFolder getTabFolder() {
        return (CTabFolder) getContainer();
    }

    /**
     * Called back when a closable page gets closed in the editor.
     * @param control  the page control (typically some composite)
     */
    protected abstract void pageClosed(Control control);
}
