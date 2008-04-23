package org.omnetpp.inifile.editor.form;

import java.util.ArrayList;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Table;
import org.omnetpp.common.contentassist.ContentAssistUtil;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.contentassist.InifileParamKeyContentProposalProvider;
import org.omnetpp.inifile.editor.contentassist.InifileValueContentProposalProvider;
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
		    addTableColumn(table, "Object", 150);
		    addTableColumn(table, "Section", 150);
		    height = table.getHeaderHeight() + 6*table.getItemHeight();
		}
        table.setLayoutData(new GridData(305, height)); 

		final CheckboxTableViewer tableViewer = new CheckboxTableViewer(table);
   
		// set up label provider
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

        // checkbox behavior
		tableViewer.addCheckStateListener(new ICheckStateListener() {
			public void checkStateChanged(CheckStateChangedEvent event) {
                SectionKey sectionKey = (SectionKey) event.getElement();
				String value = event.getChecked() ? "true" : "false";
				setValueInFile(sectionKey.section, sectionKey.key, value);
			}
		});

		if (entry.isPerObject()) {
		    // set up cell editor for object column
		    tableViewer.setColumnProperties(new String[] {"object", "value"});
		    final TableTextCellEditor[] cellEditors = new TableTextCellEditor[] {new TableTextCellEditor(tableViewer,1), null};
		    tableViewer.setCellEditors(cellEditors);

		    tableViewer.setCellModifier(new ICellModifier() {
		        public boolean canModify(Object element, String property) {
		            return property.equals("object");
		        }

		        public Object getValue(Object element, String property) {
		            Assert.isTrue(property.equals("object"));
		            SectionKey sectionKey = (SectionKey) element;
		            return StringUtils.removeEnd(sectionKey.key, "."+entry.getKey());
		        }

		        public void modify(Object element, String property, Object value) {
                    Assert.isTrue(property.equals("object"));
		            if (element instanceof Item)
		                element = ((Item) element).getData(); // workaround, see super's comment
		            SectionKey sectionKey = (SectionKey) element;
		            renameKeyInInifile(sectionKey.section, sectionKey.key, value+"."+entry.getKey());
		            tableViewer.refresh();
		        }
		    });

		    // content assist for the Object column
		    //TODO see TextTableFieldEditor and InifileParamKeyContentProposalProvider
		}
		
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
