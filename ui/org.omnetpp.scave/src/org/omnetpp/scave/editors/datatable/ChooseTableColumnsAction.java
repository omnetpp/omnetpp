package org.omnetpp.scave.editors.datatable;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.ui.dialogs.ListSelectionDialog;


/**
 * Brings up the "Choose Columns" dialog for a data table.
 */
public class ChooseTableColumnsAction extends Action {

	private DataTable table;
	
	public ChooseTableColumnsAction(DataTable table) {
		setText("Choose table columns...");
		setToolTipText("Choose table columns");
		this.table = table;
	}
	
	@Override
	public void run() {
		chooseTableColumns(table);
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
