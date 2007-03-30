package org.omnetpp.inifile.editor.form;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * For editing module parameters.
 * 
 * @author Andras
 */
//XXX for now it only edits the [Parameters] section, should be extended to run configs as well
public class ParametersPage extends FormPage {
	private static final String DEFAULT_SECTION = "Parameters";
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

		// set up tableViewer, content and label providers
		final TableViewer tableViewer = new TableViewer(table);
		tableViewer.setContentProvider(new ArrayContentProvider());
		tableViewer.setLabelProvider(new ITableLabelProvider() {
			public Image getColumnImage(Object element, int columnIndex) {
				return null;
			}

			public String getColumnText(Object element, int columnIndex) {
				String key = (String) element;
				switch (columnIndex) {
					case 0: return key;
					case 1: return getInifileDocument().getValue(DEFAULT_SECTION, key); 
					case 2: return getInifileDocument().getComment(DEFAULT_SECTION, key);
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

		//XXX set up content assist. See: addCellEditors() method, ChangeParametersControl.class in JDT
		//XXX prefix comments with "#" after editing?
		tableViewer.setColumnProperties(new String[] {"key", "value", "comment"});
		final CellEditor editors[] = new CellEditor[3];
		editors[0] = new TableTextCellEditor(tableViewer, 0);
		editors[1] = new TableTextCellEditor(tableViewer, 1);
		editors[2] = new TableTextCellEditor(tableViewer, 2);
		tableViewer.setCellEditors(editors);
		tableViewer.setCellModifier(new ICellModifier() {
			public boolean canModify(Object element, String property) {
				return true;
			}

			public Object getValue(Object element, String property) {
				String key = (String) element;
				if (property.equals("key"))
					return key;
				else if (property.equals("value"))
					return nullToEmpty(getInifileDocument().getValue(DEFAULT_SECTION, key)); 
				else if (property.equals("comment"))
					return nullToEmpty(getInifileDocument().getComment(DEFAULT_SECTION, key));
				else
					return "-";
			}

			public void modify(Object element, String property, Object value) {
			    if (element instanceof Item)
			    	element = ((Item) element).getData(); // workaround, see super's comment
				String key = (String) element;
				if (property.equals("key")) {
					getInifileDocument().changeKey(DEFAULT_SECTION, key, (String)value);
					reread(); // tableViewer.refresh() not enough, because input consists of keys
				}
				else if (property.equals("value")) {
					getInifileDocument().setValue(DEFAULT_SECTION, key, (String)value);
					tableViewer.refresh(); // if performance gets critical: refresh only if changed
				}
				else if (property.equals("comment")) {
					if (value.equals("")) value = null; // no comment == null
					getInifileDocument().setComment(DEFAULT_SECTION, key, (String)value);
					tableViewer.refresh();// if performance gets critical: refresh only if changed
				}
			}
		});
		
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
		//Button editButton = createButton(buttonGroup, "Edit...");
		Button removeButton = createButton(buttonGroup, "Remove");
		//new Label(buttonGroup, SWT.NONE);
		Button upButton = createButton(buttonGroup, "Up");
		Button downButton = createButton(buttonGroup, "Down");
		
		addButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				IStructuredSelection sel = (IStructuredSelection) tableViewer.getSelection();
				String beforeKey = sel.isEmpty() ? null : (String) sel.getFirstElement();
				String newKey = "**.newKey";
				getInifileDocument().addEntry(DEFAULT_SECTION, newKey, "", null, beforeKey);
				reread();
				//XXX key must be validated (in InifileDocument). if it causes parse error, the whole table goes away! 
				tableViewer.editElement(newKey, 0);
			}
		});

		removeButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				IStructuredSelection sel = (IStructuredSelection) tableViewer.getSelection();
				for (Object o : sel.toArray()) {
					String key = (String) o;
					getInifileDocument().removeKey(DEFAULT_SECTION, key);
				}
				reread();
			}
		});

		upButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				IStructuredSelection sel = (IStructuredSelection) tableViewer.getSelection();
				for (Object o : sel.toArray()) {
					String key = (String) o;
					//XXX getInifileDocument().moveKey(DEFAULT_SECTION, key);
				}
				reread();
			}
		});

		downButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				IStructuredSelection sel = (IStructuredSelection) tableViewer.getSelection();
				Object[] array = sel.toArray();
				ArrayUtils.reverse(array);  // we must iterate in reverse order
				for (Object o : array) {
					String key = (String) o;
					//XXX getInifileDocument().moveKey(DEFAULT_SECTION, key);
				}
				reread();
			}
		});

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
		tableViewer.setInput(doc.getKeys(DEFAULT_SECTION)); //XXX or empty array if there's no such section
		tableViewer.refresh();
	}

	@Override
	public void commit() {
		//XXX todo
	}
}
