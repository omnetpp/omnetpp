package org.omnetpp.inifile.editor.form;

import java.util.ArrayList;

import org.eclipse.core.resources.IMarker;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;


/**
 * Checkbox-table based field editor, for editing boolean config entries
 * in all sections.
 * 
 * @author Andras
 */
public class CheckboxTableFieldEditor extends TableFieldEditor {

	public CheckboxTableFieldEditor(Composite parent, ConfigKey entry, IInifileDocument inifile, FormPage formPage, String labelText) {
		super(parent, entry, inifile, formPage, labelText);
	}

	@Override
	protected TableViewer createTableViewer(Composite parent) {
		// set up table viewer and its label provider
		Table table = new Table(parent, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION | SWT.CHECK);
		table.setLayoutData(new GridData(200, 2*table.getItemHeight())); // for some reason, 2* results in 3-line table...
		CheckboxTableViewer tableViewer = new CheckboxTableViewer(table);

		tableViewer.setLabelProvider(new TableLabelProvider() {
			@Override
			public String getColumnText(Object element, int columnIndex) {
				String section = (String) element;
				switch (columnIndex) {
					case 0: return "in section ["+section+"]";
					default: throw new IllegalArgumentException(); 
				}
			}
			@Override
			public Image getColumnImage(Object element, int columnIndex) {
				if (columnIndex == 0) {
					String section = (String) element;
					IMarker[] markers = InifileUtils.getProblemMarkersFor(section, entry.getKey(), inifile);
					return getProblemImage(markers, true);
				}
				return null;
			}
		});
		
		tableViewer.addCheckStateListener(new ICheckStateListener() {
			public void checkStateChanged(CheckStateChangedEvent event) {
				String section = (String) event.getElement();
				String value = event.getChecked() ? "true" : "false";
				setValueInFile(section, value); 
			}
		});
		
 		return tableViewer; 
	}

	@Override
	public void reread() {
		super.reread();

		// (Checkbox)TableViewer cannot read the checked state via a label provider-like 
		// interface, so we have to set it explicitly here...
		ArrayList<String> list = new ArrayList<String>();
		for (String section : (String[]) tableViewer.getInput())
			if (InifileUtils.parseAsBool(inifile.getValue(section, entry.getKey())))
				list.add(section);
		((CheckboxTableViewer)tableViewer).setCheckedElements(list.toArray());
	}

}
