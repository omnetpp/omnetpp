package org.omnetpp.inifile.editor.form;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Table;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Table based field editor for editing textual config entries
 * in all sections. Presents a table with a Section and a Value
 * columns, Value being editable.
 * 
 * @author Andras
 */
public class TextTableFieldEditor extends TableFieldEditor {

	public TextTableFieldEditor(Composite parent, ConfigurationEntry entry, IInifileDocument inifile, String labelText) {
		super(parent, entry, inifile, labelText);
	}

	protected TableViewer createTableViewer(Composite parent) {
		// add table
		Table table = new Table(parent, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION);
		table.setLayoutData(new GridData(320, table.getHeaderHeight()+4*table.getItemHeight()+2));
		table.setLinesVisible(true);
		table.setHeaderVisible(true);
		addTableColumn(table, "Section", 100);
		addTableColumn(table, "Value", 200);

		// set up tableViewer, label provider
		final TableViewer tableViewer = new TableViewer(table);
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
		
		// set up cell editor for value column
		//XXX set up content assist. See: addCellEditors() method, ChangeParametersControl.class in JDT; see also ParametersPage
		tableViewer.setColumnProperties(new String[] {"section", "value"});
		tableViewer.setCellEditors(new CellEditor[] {null, new TableTextCellEditor(tableViewer,1)});
		tableViewer.setCellModifier(new ICellModifier() {
			public boolean canModify(Object element, String property) {
				return property.equals("value");
			}

			public Object getValue(Object element, String property) {
				Assert.isTrue(property.equals("value"));
				String section = (String) element;
				return getValueFromFile(section);
			}

			public void modify(Object element, String property, Object value) {
			    if (element instanceof Item)
			    	element = ((Item) element).getData(); // workaround, see super's comment
				Assert.isTrue(property.equals("value"));
				String section = (String) element;
				setValueInFile(section, (String) value);
				tableViewer.refresh();
			}
		});
		
		return tableViewer; 
	}
	
}
