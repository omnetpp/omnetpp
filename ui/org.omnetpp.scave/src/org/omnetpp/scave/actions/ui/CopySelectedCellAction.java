/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.ui;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.TextTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.common.Debug;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.DataTable;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.ui.BrowseDataPage;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.model2.AndFilter;

/**
 * Copy the value in the selected cell in the active filtered data panel
 * to the clipboard.
 *
 * @author andras
 */
public class CopySelectedCellAction extends AbstractScaveAction
{
    public CopySelectedCellAction() {
        setText("Copy Cell");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_TOOL_COPY));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        BrowseDataPage browseDataPage = scaveEditor.getBrowseDataPage();
        if (browseDataPage != scaveEditor.getActiveEditorPage())
            scaveEditor.showPage(browseDataPage);

        FilteredDataPanel panel = browseDataPage.getActivePanel();
        if (!(panel.getDataControl() instanceof DataTable))
            return;
        DataTable table = (DataTable)panel.getDataControl();
        String cellContent = table.getSelectedCell();

        // copy to clipboard
        Clipboard clipboard = new Clipboard(Display.getCurrent());
        clipboard.setContents(new Object[] {cellContent}, new Transfer[] {TextTransfer.getInstance()});
        clipboard.dispose();
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return editor.getBrowseDataPage().getActivePanel().getDataControl().getItemCount() != 0;
    }
}
