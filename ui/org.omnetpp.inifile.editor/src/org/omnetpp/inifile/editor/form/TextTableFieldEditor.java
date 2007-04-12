package org.omnetpp.inifile.editor.form;

import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;

public class TextTableFieldEditor extends TableFieldEditor {

	public TextTableFieldEditor(Composite parent, ConfigurationEntry entry, IInifileDocument inifile, String labelText) {
		super(parent, entry, inifile, labelText);
	}

	protected TableViewer createTableViewer(Composite parent) {
		// add table
		Table table = new Table(parent, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION);
		table.setLayoutData(new GridData(320, 80));
		table.setLinesVisible(true);
		table.setHeaderVisible(true);
		addTableColumn(table, "Section", 100);
		addTableColumn(table, "Value", 200);

		// set up tableViewer, label provider
		TableViewer tableViewer = new TableViewer(table);
		tableViewer.setLabelProvider(new TableLabelProvider() {
			public String getColumnText(Object element, int columnIndex) {
				String section = (String) element;
				switch (columnIndex) {
					case 0: return "["+section+"]";
					case 1: return getValueFromFile(section);
					default: throw new IllegalArgumentException();
				}
			}
		});
		return tableViewer; 
	}
	
}
