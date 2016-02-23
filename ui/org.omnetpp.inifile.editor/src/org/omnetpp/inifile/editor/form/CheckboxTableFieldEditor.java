/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import java.util.ArrayList;
import java.util.Map;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.ConfigOption;
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

    public CheckboxTableFieldEditor(Composite parent, ConfigOption entry, IInifileDocument inifile, FormPage formPage, String labelText, Map<String,Object> hints) {
        super(parent, entry, inifile, formPage, labelText, hints);
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
            addTableColumn(table, getStringHint(HINT_OBJECT_COL_TITLE, "Object"), 150);
            addTableColumn(table, getStringHint(HINT_SECTION_COL_TITLE, "Section"), 150);
            height = table.getHeaderHeight() + 6*table.getItemHeight();
        }
        int width = 5;
        for (TableColumn column : table.getColumns())
            width += column.getWidth();
        table.setLayoutData(new GridData(Math.max(305, width), height));

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
                        case 0: return StringUtils.removeEnd(sectionKey.key, "."+entry.getName());
                        case 1: return "["+sectionKey.section+"]";
                        default: throw new IllegalArgumentException();
                    }
            }
            @Override
            public Image getColumnImage(Object element, int columnIndex) {
                if (columnIndex == 0) {
                    SectionKey sectionKey = (SectionKey) element;
                    IMarker[] markers = InifileUtils.getProblemMarkersFor(sectionKey.section, sectionKey.key, inifile);
                    return getProblemImage(markers, true, true);
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
                    return StringUtils.removeEnd(sectionKey.key, "."+entry.getName());
                }

                public void modify(Object element, String property, Object value) {
                    Assert.isTrue(property.equals("object"));
                    if (element instanceof Item)
                        element = ((Item) element).getData(); // workaround, see super's comment
                    SectionKey sectionKey = (SectionKey) element;
                    renameKeyInInifile(sectionKey.section, sectionKey.key, value+"."+entry.getName());
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
