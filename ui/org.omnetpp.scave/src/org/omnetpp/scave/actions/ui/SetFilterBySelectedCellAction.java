/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.ui;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.datatable.IDataControl;
import org.omnetpp.scave.editors.ui.BrowseDataPage;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.model2.FilterUtil;

/**
 * Sets the filter of a filtered data panel.
 * The filter is determined by the selected cell of the panel's control.
 *
 * @author andras
 */
public class SetFilterBySelectedCellAction extends AbstractScaveAction
{
    public SetFilterBySelectedCellAction() {
        setText("Set Cell Value As Filter");
        setDescription("Sets the filter according to the clicked cell.");
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        BrowseDataPage browseDataPage = scaveEditor.getBrowseDataPage();
        if (browseDataPage != scaveEditor.getActiveEditorPage())
            scaveEditor.showPage(browseDataPage);

        FilteredDataPanel panel = browseDataPage.getActivePanel();
        ResultFileManager.runWithReadLock(panel.getResultFileManager(), () -> {
            IDataControl control = panel.getDataControl();
            ResultItem item = control.getSelectedItem();
            if (item != null && control.getSelectedField() != null) {
                ResultItemField field = new ResultItemField(control.getSelectedField());
                String fieldName = field.getName();
                String fieldValue = field.getFieldValue(item);

                if (fieldName != null && fieldValue != null) {
                    FilterUtil filter = new FilterUtil();
                    filter.setField(fieldName, fieldValue);
                    panel.setFilter(filter.getFilterPattern());
                }
            }
        });
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        if (!(selection instanceof IDListSelection))
            return false;
        IDList idList = ((IDListSelection)selection).getIDList();
        return idList.size() == 1;
    }
}
