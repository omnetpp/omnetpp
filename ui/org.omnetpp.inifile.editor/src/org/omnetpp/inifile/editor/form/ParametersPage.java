package org.omnetpp.inifile.editor.form;

import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
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
public class ParametersPage extends FormPage {
	private TableViewer tableViewer;
	
	public ParametersPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, inifileEditor);

		// layout: 2x2 grid: (label, dummy) / (table, buttonGroup)
		setLayout(new GridLayout(2,false));
		Label label = new Label(this, SWT.NONE);
		label.setText("Parameter Assignments");
		new Label(this, SWT.NONE); // dummy
		Table table = new Table(this, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION | SWT.VIRTUAL);
		table.setLinesVisible(true);
		table.setHeaderVisible(true);
		table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		addTableColumn(table, "Key", 200);
		addTableColumn(table, "Value", 150);
		addTableColumn(table, "Comment", 200);
		tableViewer = new TableViewer(table);
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
				//XXX
			}

			public void dispose() {
			}

			public boolean isLabelProperty(Object element, String property) {
				return true;
			}

			public void removeListener(ILabelProviderListener listener) {
				//XXX
			}
		});

		Composite buttonGroup = new Composite(this, SWT.NONE);
		buttonGroup.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, true));
		buttonGroup.setLayout(new GridLayout(1,false));
		
		Button addButton = createButton(buttonGroup, "Add...");
		Button editButton = createButton(buttonGroup, "Edit...");
		Button removeButton = createButton(buttonGroup, "Remove");
		Button upButton = createButton(buttonGroup, "Up");
		Button downButton = createButton(buttonGroup, "Down");
		
		reread();
	}

	private TableColumn addTableColumn(Table table, String label, int width) {
		TableColumn column = new TableColumn(table, SWT.NONE);
		column.setText(label);
		column.setWidth(width);
		return column;
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
