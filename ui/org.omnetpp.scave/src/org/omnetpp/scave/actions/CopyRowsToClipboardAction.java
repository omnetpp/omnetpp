/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.common.ui.TimeTriggeredProgressMonitorDialog2;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.ui.BrowseDataPage;

/**
 * Copy rows from the active panel of the Browse Data page to the clipboard.
 */
public class CopyRowsToClipboardAction extends AbstractScaveAction {
    public CopyRowsToClipboardAction() {
        setText("Copy Rows");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_COPYDATA));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        final FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
        if (activePanel != null) {
            TimeTriggeredProgressMonitorDialog2.runWithDialogInUIThread("Copying to clipboard",
                    (monitor) -> activePanel.getDataControl().copyRowsToClipboard(monitor));
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        BrowseDataPage browseDataPage = editor.getBrowseDataPage();
        return browseDataPage != null && browseDataPage.getActivePanel() != null;
    }
}
