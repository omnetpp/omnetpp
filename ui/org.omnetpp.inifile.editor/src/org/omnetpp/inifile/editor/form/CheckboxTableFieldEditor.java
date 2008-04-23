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
import org.omnetpp.inifile.editor.model.SectionKey;


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
                SectionKey sectionKey = (SectionKey) element;
				switch (columnIndex) {
					case 0: return "in section ["+sectionKey.section+"]";
					default: throw new IllegalArgumentException();
				}
			}
			@Override
			public Image getColumnImage(Object element, int columnIndex) {
				if (columnIndex == 0) {
	                SectionKey sectionKey = (SectionKey) element;
					IMarker[] markers = InifileUtils.getProblemMarkersFor(sectionKey.section, sectionKey.key, inifile);
					return getProblemImage(markers, true);
				}
				return null;
			}
		});

		tableViewer.addCheckStateListener(new ICheckStateListener() {
			public void checkStateChanged(CheckStateChangedEvent event) {
                SectionKey sectionKey = (SectionKey) event.getElement();
				String value = event.getChecked() ? "true" : "false";
				setValueInFile(sectionKey.section, sectionKey.key, value);
			}
		});

 		return tableViewer;
	}

	@Override
	public void reread() {
		super.reread();

		// (Checkbox)TableViewer cannot read the checked state via a label provider-like
		// interface, so we have to set it explicitly here...
		ArrayList<SectionKey> list = new ArrayList<SectionKey>();
		for (SectionKey sectionKey : (SectionKey[]) tableViewer.getInput())
			if (InifileUtils.parseAsBool(inifile.getValue(sectionKey.section, sectionKey.key)))
				list.add(sectionKey);
		((CheckboxTableViewer)tableViewer).setCheckedElements(list.toArray());
	}

}
