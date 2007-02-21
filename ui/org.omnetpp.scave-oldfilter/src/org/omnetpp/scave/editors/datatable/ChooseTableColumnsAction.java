package org.omnetpp.scave.editors.datatable;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.widgets.Event;
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
//XXX original code, may be deleted		
//		String[] columns = table.getColumnNames();
//		boolean[] initialSelection = new boolean[columns.length];
//		for (int i = 0; i < columns.length; ++i)
//			initialSelection[i] = table.isColumnVisible(i);
//		CheckboxSelectionDialog dialog = new CheckboxSelectionDialog(
//				table.getShell(), 
//				"Select Columns", 
//				"Select which columns should be visible in the table:", 
//				columns, initialSelection);
//		if (dialog.open() == Window.OK) {
//			boolean[] selection = dialog.getSelection();
//			for (int i = 0; i < selection.length; ++i)
//				table.setColumnVisible(i, selection[i]);
//		}

		// create dialog with the column names
		ListSelectionDialog dialog = new ListSelectionDialog(
			 table.getShell(),
			 table.getColumnNames(),
			 new ArrayContentProvider(),
			 new LabelProvider(), // plain toString()
			 "Select which columns should be visible in the table:");
		dialog.setTitle("Select Columns");

		// calculate initial selection
		String[] columns = table.getColumnNames();
		ArrayList<String> initialSelection = new ArrayList<String>();
		for (int i = 0; i < columns.length; ++i)
			if (table.isColumnVisible(i))
				initialSelection.add(columns[i]);
		dialog.setInitialSelections(initialSelection.toArray());

		// execute dialog and store result
		if (dialog.open() == Dialog.OK) {
			List result = Arrays.asList(dialog.getResult());
		    for (int i = 0; i < columns.length; ++i)
		    	table.setColumnVisible(i, result.contains(columns[i]));
		}
	}
}
