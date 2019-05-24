/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.concurrent.Callable;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.ui.BrowseDataPage;
import org.omnetpp.scave.engine.ResultFileManager;

/**
 * Copy data from the Browse Data page to the clipboard.
 */
public class CopyDataToClipboardAction extends AbstractScaveAction {
    public CopyDataToClipboardAction() {
        setText("Copy to Clipboard");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_TOOL_COPY));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) {
        final FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
        if (activePanel != null) {
            ResultFileManager.callWithReadLock(activePanel.getResultFileManager(), new Callable<Object>() {
                public Object call() throws Exception {
                    activePanel.getDataControl().copySelectionToClipboard();
                    return null;
                }
            });
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        BrowseDataPage browseDataPage = editor.getBrowseDataPage();
        return browseDataPage != null && browseDataPage.getActivePanel() != null;
    }
}
