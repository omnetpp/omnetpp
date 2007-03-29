package org.omnetpp.inifile.editor.form;

import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * For editing module parameters.
 * 
 * @author Andras
 */
//XXX for now it only edits the [Parameters] section, should be extended to run configs as well
//XXX hint for in-place table editing: ICellModifier, TableViewer.setCellEditors(), TableViewer.setCellModifier()
// see: ChangeParametersControl.class in JDT, addCellEditors() and ParametersCellModifier inner class
//	and:
//		final TableTextCellEditor editors[]= new TableTextCellEditor[PROPERTIES.length];
//		editors[TYPE_PROP]= new TableTextCellEditor(fTableViewer, TYPE_PROP);
//		editors[NEWNAME_PROP]= new TableTextCellEditor(fTableViewer, NEWNAME_PROP);
//		editors[DEFAULT_PROP]= new TableTextCellEditor(fTableViewer, DEFAULT_PROP);
//      [...]
//		fTableViewer.setCellEditors(editors);
//		fTableViewer.setCellModifier(new ParametersCellModifier());
//

public class ParametersPage extends FormPage {
	private TableViewer tableViewer;
	
	public ParametersPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, inifileEditor);

		// layout: 2x2 grid: (label, dummy) / (table, buttonGroup)
		setLayout(new GridLayout(2,false));

		Label label = new Label(this, SWT.NONE);
		label.setText("Parameter Assignments");
		new Label(this, SWT.NONE); // dummy, for row1col2

		tableViewer = createAndConfigureTable();
		tableViewer.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		
		Composite buttonGroup = createButtons();
		buttonGroup.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, true));
		
		reread();
	}

	private TableViewer createAndConfigureTable() {
		Table table = new Table(this, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION | SWT.VIRTUAL);
		table.setLinesVisible(true);
		table.setHeaderVisible(true);
		addTableColumn(table, "Key", 200);
		addTableColumn(table, "Value", 150);
		addTableColumn(table, "Comment", 200);

		TableViewer tableViewer = new TableViewer(table);
		tableViewer.setContentProvider(new ArrayContentProvider());
		
		tableViewer.setLabelProvider(new ITableLabelProvider() {
			public Image getColumnImage(Object element, int columnIndex) {
				return null;
			}

			public String getColumnText(Object element, int columnIndex) {
				String key = (String) element;
				switch (columnIndex) {
					case 0: return key;
					case 1: return getInifileDocument().getValue("Parameters", key); 
					case 2: return getInifileDocument().getComment("Parameters", key);
					default: throw new IllegalArgumentException();
				}
			}

			public void addListener(ILabelProviderListener listener) {
			}

			public void dispose() {
			}

			public boolean isLabelProperty(Object element, String property) {
				return true;
			}

			public void removeListener(ILabelProviderListener listener) {
			}
		});

		//XXX make a copy of JDT's TableTextCellEditor, and use that! see example in ChangeParametersControl in JDT
		final TextCellEditor editors[] = new TextCellEditor[3];
		editors[0] = new TextCellEditor(tableViewer.getTable(), SWT.NONE);
		editors[1] = new TextCellEditor(tableViewer.getTable(), SWT.NONE);
		editors[2] = new TextCellEditor(tableViewer.getTable(), SWT.NONE);
		tableViewer.setCellEditors(editors);
		tableViewer.setColumnProperties(new String[] {"key", "value", "comment"});
		tableViewer.setCellModifier(new ICellModifier() {
			public boolean canModify(Object element, String property) {
				return true;
			}

			public Object getValue(Object element, String property) {
				String key = (String) element;
				if (property.equals("key"))
					return key;
				else if (property.equals("value"))
					return nullToEmpty(getInifileDocument().getValue("Parameters", key)); 
				else if (property.equals("comment"))
					return nullToEmpty(getInifileDocument().getComment("Parameters", key));
				else
					return "-";
			}

			public void modify(Object element, String property, Object value) {
			    if (element instanceof Item)
			    	element = ((Item) element).getData(); // workaround, see super's comment
				String key = (String) element;
				String stringValue = (String) value;
				if (property.equals("key"))
					; //FIXME implement changeKey(section, oldKey, newKey) in IInifileDocument
				else if (property.equals("value"))
					getInifileDocument().setValue("Parameters", key, stringValue); 
				else if (property.equals("comment"))
					getInifileDocument().setComment("Parameters", key, stringValue);
			}
		});
		//XXX arrange updating labelProvider after cell editor has committed
		
		return tableViewer;
	}

	private static String nullToEmpty(String text) {
		return text == null ? "" : text;
	}
	
	private TableColumn addTableColumn(Table table, String label, int width) {
		TableColumn column = new TableColumn(table, SWT.NONE);
		column.setText(label);
		column.setWidth(width);
		return column;
	}

	private Composite createButtons() {
		Composite buttonGroup = new Composite(this, SWT.NONE);
		buttonGroup.setLayout(new GridLayout(1,false));
		
		Button addButton = createButton(buttonGroup, "Add...");
		Button editButton = createButton(buttonGroup, "Edit...");
		Button removeButton = createButton(buttonGroup, "Remove");
		Button upButton = createButton(buttonGroup, "Up");
		Button downButton = createButton(buttonGroup, "Down");
		return buttonGroup;
	}

	private Button createButton(Composite buttonGroup, String label) {
		Button button = new Button(buttonGroup, SWT.PUSH);
		button.setText(label);
		button.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
		return button;
	}

	@Override
	public void reread() {
		//XXX use keys like: "section/key"
		IInifileDocument doc = getInifileDocument();
		//XXX get only dotted keys! if (key.contains("."))...
		//TODO should introduce rule: parameter refs must contain a dot; to check this in C++ code as well
		tableViewer.setInput(doc.getKeys("Parameters")); //XXX or empty array if there's no such section
		tableViewer.refresh();
	}

	@Override
	public void commit() {
		//XXX todo
	}
}
