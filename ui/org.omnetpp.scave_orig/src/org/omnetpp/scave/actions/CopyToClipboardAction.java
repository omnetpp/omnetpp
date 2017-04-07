/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.concurrent.Callable;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.engine.ResultFileManager;

/**
 * ...
 */
public class CopyToClipboardAction extends AbstractScaveAction {
    public CopyToClipboardAction() {
        setText("Copy to Clipboard");
        setToolTipText("Copy Data to Clipboard"); //TODO  in various formats!!!!
    }

    @Override
    protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
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
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return editor.getBrowseDataPage().getActivePanel() != null;
    }
}
