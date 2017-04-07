/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

import java.util.concurrent.Callable;

import org.eclipse.jface.action.Action;
import org.omnetpp.scave.engine.ResultFileManager;


/**
 * Copies the selected rows to the system clipboard as tab separated values.
 */
public class CopySelectionToClipboardAction extends Action {

    private FilteredDataTabFolder tabfolder;

    public CopySelectionToClipboardAction(FilteredDataTabFolder tabfolder) {
        setText("Copy to Clipboard");
        setToolTipText("Copy Data to Clipboard");
        this.tabfolder = tabfolder;
    }

    @Override
    public void run() {
        if (tabfolder != null) {
            final FilteredDataPanel activePanel = tabfolder.getActivePanel();
            if (activePanel != null && activePanel.getResultFileManager() != null)
            {
                ResultFileManager.callWithReadLock(activePanel.getResultFileManager(), new Callable<Object>() {
                    public Object call() throws Exception {
                        activePanel.getDataControl().copySelectionToClipboard();
                        return null;
                    }
                });
            }
        }
    }
}
