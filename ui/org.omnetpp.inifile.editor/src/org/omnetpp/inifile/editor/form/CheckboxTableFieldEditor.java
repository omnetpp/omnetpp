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
import org.omnetpp.common.util.StringUtils;
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

	public void setSectionColumnTitle(String text) {
	    if (entry.isPerObject())  // otherwise there're no table headers
	        tableViewer.getTable().getColumn(1).setText(text);
	}

	public void setObjectColumnTitle(String text) {
        if (entry.isPerObject())  // otherwise there're no table headers
            tableViewer.getTable().getColumn(0).setText(text);
	}

	@Override
	protected TableViewer createTableViewer(Composite parent) {
		// set up table viewer and its label provider
		Table table = new Table(parent, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION | SWT.CHECK);
        int height;
		if (!entry.isPerObject()) {
		    height = 2*table.getItemHeight(); // for some reason, 2* results in 3-line table...
		}
		else {
		    table.setLinesVisible(true);
		    table.setHeaderVisible(true);
		    addTableColumn(table, "Object", 100);
		    addTableColumn(table, "Section", 100);
		    height = table.getHeaderHeight() + 3*table.getItemHeight();
		}
        table.setLayoutData(new GridData(220, height)); 

		CheckboxTableViewer tableViewer = new CheckboxTableViewer(table);

        tableViewer.setLabelProvider(new TableLabelProvider() {
			@Override
			public String getColumnText(Object element, int columnIndex) {
                SectionKey sectionKey = (SectionKey) element;
                if (!entry.isPerObject())
                    switch (columnIndex) {
                        case 0: return "in section ["+sectionKey.section+"]";
                        default: throw new IllegalArgumentException();
                    }
                else
                    switch (columnIndex) {
                        case 0: return StringUtils.removeEnd(sectionKey.key, "."+entry.getKey());
                        case 1: return "["+sectionKey.section+"]";
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
