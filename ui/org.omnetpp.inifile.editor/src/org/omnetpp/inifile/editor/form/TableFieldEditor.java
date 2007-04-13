package org.omnetpp.inifile.editor.form;

import java.util.ArrayList;

import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Base class for field editors that allow the user edit a setting
 * with section granularity. 
 *  
 * @author Andras
 */
public abstract class TableFieldEditor extends FieldEditor {
	protected TableViewer tableViewer;  // input: section names as String[]
	protected Button addButton;
	protected Button removeButton;


	public TableFieldEditor(Composite parent, ConfigurationEntry entry, IInifileDocument inifile, String labelText) {
		super(parent, SWT.NONE, entry, inifile);

		GridLayout gridLayout = new GridLayout(1, false);
		gridLayout.marginTop = gridLayout.marginBottom = gridLayout.marginHeight = gridLayout.verticalSpacing = 0;
		gridLayout.marginHeight = 2;
		setLayout(gridLayout);

		createLabel(entry, labelText+":  (default: "+(entry.getDefaultValue()==null ? "none" : entry.getDefaultValue().toString())+")");
		createTableWithButtons(this);
		reread();
	}

	protected void createTableWithButtons(Composite parent) {
		Composite composite = new Composite(parent, SWT.NONE);
		composite.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
		composite.setLayout(new GridLayout(2, false));
		
		tableViewer = createTableViewer(composite);
		tableViewer.setContentProvider(new ArrayContentProvider());

		// add button group
		Composite buttonGroup = new Composite(composite, SWT.NONE);
		buttonGroup.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, true));

		GridLayout gridLayout = new GridLayout(1, false);
		gridLayout.marginTop = gridLayout.marginBottom = gridLayout.marginHeight = 0;
		buttonGroup.setLayout(gridLayout);
		addButton = createButton(buttonGroup, "Add...");
		removeButton = createButton(buttonGroup, "Remove");

		addButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				String section = askTargetSection();
				if (section != null) {
					setValueInFile(section, getDefaultValueFor(section));
					reread();
				}
			}
		});

		removeButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				IStructuredSelection selection = (IStructuredSelection) tableViewer.getSelection();
				for (Object o : selection.toArray()) {
					String section = (String) o;
					removeFromFile(section);
				}
				reread();
			}
		});
		
	}

	/**
	 * When creating a new entry: prompts the user for the section in which to insert it.
	 */
	protected String askTargetSection() {
		// collect section which not yet contain this key
		ArrayList<String> list = new ArrayList<String>();
		for (String section : inifile.getSectionNames())
			if (!inifile.containsKey(section, entry.getName()))
				list.add(section);

		// and pop up a chooser dialog
		ListDialog dialog = new ListDialog(getShell());
		dialog.setLabelProvider(new LabelProvider());
		dialog.setContentProvider(new ArrayContentProvider());
		dialog.setInput(list.toArray());
		dialog.setMessage("Select target section:");
		dialog.setTitle("New Configuration Key");
		if (dialog.open() == ListDialog.OK)
			return dialog.getResult()[0].toString();
		return null;
	}

	/**
	 * Generate default value for a new entry (in the given section).
	 */
	protected String getDefaultValueFor(String section) {
		if (entry.getDefaultValue() != null)
			return entry.getDefaultValue().toString();
		switch (entry.getType()) {
			case CFG_BOOL: return "true";
			case CFG_INT:  return "0";
			case CFG_DOUBLE: return "0.0";
			case CFG_FILENAME: return "";
			case CFG_FILENAMES: return "";
			case CFG_STRING: return "";
		}
		return "";
	}

	/**
	 * Create the table and add label provider. Content provider and input will be
	 * managed by the TableFieldEditor base class.
	 */
	abstract protected TableViewer createTableViewer(Composite composite);

	/**
	 * Utility method for subclasses, to add a table column. (Unused here.)
	 */
	protected TableColumn addTableColumn(Table table, String label, int width) {
		TableColumn column = new TableColumn(table, SWT.NONE);
		column.setText(label);
		column.setWidth(width);
		return column;
	}

	protected Button createButton(Composite buttonGroup, String label) {
		Button button = new Button(buttonGroup, SWT.PUSH);
		button.setText(label);
		button.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
		return button;
	}
	
	/**
	 * Does nothing.
	 */
	@Override
	public void commit() {
		// table cell editors will autocommit when losing the focus
	}

	/**
	 * Refresh table contents.
	 */
	@Override
	public void reread() {
		// find out in which sections this key occurs, and set it to the table as input
		ArrayList<String> list = new ArrayList<String>();
		for (String section : inifile.getSectionNames())
			if (inifile.containsKey(section, entry.getName()))
				list.add(section);
		tableViewer.setInput(list.toArray(new String[list.size()]));
	}

}
