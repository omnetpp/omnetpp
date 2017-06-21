/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

import java.util.Arrays;
import java.util.List;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.ui.dialogs.ListSelectionDialog;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;


/**
 * Brings up the "Choose Columns" dialog for a data table.
 */
public class ChooseTableColumnsAction extends Action {
    private DataTable dataTable;

    public ChooseTableColumnsAction(DataTable table) {
        setText("Choose Table Columns...");
        setToolTipText("Choose Table Columns");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_CHOOSETABLECOLUMN));
        this.dataTable = table;
    }

    public void setDataTable(DataTable dataTable) {
        this.dataTable = dataTable;
    }

    public DataTable getDataTable() {
        return dataTable;
    }

    @Override
    public void run() {
        chooseTableColumns(dataTable);
    }

    protected void chooseTableColumns(DataTable table) {
        // create dialog with the column names
        ListSelectionDialog dialog = new ListSelectionDialog(
             table.getShell(),
             table.getAllColumnNames(),
             new ArrayContentProvider(),
             new LabelProvider(), // plain toString()
             "Select which columns should be visible in the table:");
        dialog.setTitle("Select Columns");

        // calculate initial selection
        dialog.setInitialSelections(table.getVisibleColumnNames());

        // execute dialog and store result
        if (dialog.open() == Dialog.OK) {
            List<Object> selectedObjects = Arrays.asList(dialog.getResult());
            String [] selectedColumnNames = selectedObjects.toArray(new String[selectedObjects.size()]);
            table.setVisibleColumns(selectedColumnNames);
        }
    }
}
