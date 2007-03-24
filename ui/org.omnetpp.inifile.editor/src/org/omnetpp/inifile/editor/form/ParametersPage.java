package org.omnetpp.inifile.editor.form;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.inifile.editor.editors.InifileEditor;

/**
 * For editing module parameters.
 * 
 * @author Andras
 */
public class ParametersPage extends FormPage {
	Table table;
	
	public ParametersPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, inifileEditor);
		setLayout(new GridLayout(1,false));
		
		table = new Table(this, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION);
		table.setLinesVisible(true);
		table.setHeaderVisible(true);
		table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		new TableColumn(table, SWT.NONE).setText("Key");
		new TableColumn(table, SWT.NONE).setText("Value");
		new TableColumn(table, SWT.NONE).setText("Comment");
	}
	
	@Override
	public void reread() {
		//XXX todo
	}

	@Override
	public void commit() {
		//XXX todo
	}
}
