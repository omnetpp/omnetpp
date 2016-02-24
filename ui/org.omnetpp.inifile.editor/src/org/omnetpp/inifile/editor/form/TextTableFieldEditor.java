/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import java.util.Map;

import org.eclipse.core.resources.IMarker;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.contentassist.ContentAssistUtil;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.contentassist.InifileValueContentProposalProvider;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.SectionKey;

/**
 * Table based field editor for editing textual config entries
 * in all sections. Presents a table with a Section and a Value
 * columns, Value being editable. If the config entry is per-object,
 * there's also an Object column
 *
 * @author Andras
 */
//XXX fix up content assist!!!
//XXX todo: content assist for the "object" column too
public class TextTableFieldEditor extends TableFieldEditor {
    public TextTableFieldEditor(Composite parent, ConfigOption entry, IInifileDocument inifile, FormPage formPage, String labelText, Map<String,Object> hints) {
        super(parent, entry, inifile, formPage, labelText, hints);
    }

    public void setSectionColumnTitle(String text) {
        tableViewer.getTable().getColumn(0).setText(text);
    }

    public void setObjectColumnTitle(String text) {
        if (entry.isPerObject())  // otherwise there's no "Object" column
            tableViewer.getTable().getColumn(1).setText(text);
    }

    public void setValueColumnTitle(String text) {
        int valueColumnIndex = entry.isPerObject() ? 2 : 1;
        tableViewer.getTable().getColumn(valueColumnIndex).setText(text);
    }

    @Override
    protected TableViewer createTableViewer(Composite parent) {
        // add table
        Table table = new Table(parent, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION);
        table.setLinesVisible(true);
        table.setHeaderVisible(true);
        if (entry.isPerObject()) {
            addTableColumn(table, getStringHint(HINT_SECTION_COL_TITLE, "Section"), 160);
            addTableColumn(table, getStringHint(HINT_OBJECT_COL_TITLE, "Object"), 100);
            addTableColumn(table, getStringHint(HINT_VALUE_COL_TITLE, "Value"), 160);
        }
        else {
            addTableColumn(table, getStringHint(HINT_SECTION_COL_TITLE, "Section"), 160);
            addTableColumn(table, getStringHint(HINT_VALUE_COL_TITLE, "Value"), 260);
        }
        int numLines = entry.isPerObject() ? 6 : 3;
        int width = 5;
        for (TableColumn column : table.getColumns())
            width += column.getWidth();
        int height = table.getHeaderHeight()+numLines*table.getItemHeight()+2;
        table.setLayoutData(new GridData(Math.max(width, 425), height));

        final int objectColumnIndex = entry.isPerObject() ? 1 : -1;
        final int valueColumnIndex = entry.isPerObject() ? 2 : 1;

        // set up tableViewer, label provider
        final TableViewer tableViewer = new TableViewer(table);
        tableViewer.setLabelProvider(new TableLabelProvider() {
            @Override
            public String getColumnText(Object element, int columnIndex) {
                SectionKey sectionKey = (SectionKey) element;
                if (columnIndex == 0)
                    return "["+sectionKey.section+"]";
                if (columnIndex == objectColumnIndex)
                    return StringUtils.removeEnd(sectionKey.key, "."+entry.getName());
                if (columnIndex == valueColumnIndex)
                    return getValueFromFile(sectionKey.section, sectionKey.key);
                throw new IllegalArgumentException();
            }

            @Override
            public Image getColumnImage(Object element, int columnIndex) {
                if (columnIndex == valueColumnIndex) {
                    SectionKey sectionKey = (SectionKey) element;
                    IMarker[] markers = InifileUtils.getProblemMarkersFor(sectionKey.section, sectionKey.key, inifile);
                    return getProblemImage(markers, true, true);
                }
                return null;
            }
        });

        // set up cell editor for value column
        tableViewer.setColumnProperties(entry.isPerObject() ?
                new String[] {"section", "object", "value"} :
                new String[] {"section", "value"});
        final TableTextCellEditor[] cellEditors = entry.isPerObject() ?
                new TableTextCellEditor[] {null, new TableTextCellEditor(tableViewer,1), new TableTextCellEditor(tableViewer,2)} :
                new TableTextCellEditor[] {null, new TableTextCellEditor(tableViewer,1)};
        tableViewer.setCellEditors(cellEditors);

        tableViewer.setCellModifier(new ICellModifier() {
            public boolean canModify(Object element, String property) {
                return property.equals("value") || property.equals("object");
            }

            public Object getValue(Object element, String property) {
                SectionKey sectionKey = (SectionKey) element;
                if (property.equals("value"))
                    return getValueFromFile(sectionKey.section, sectionKey.key);
                else if (property.equals("object"))
                    return StringUtils.removeEnd(sectionKey.key, "."+entry.getName());
                else
                    throw new IllegalArgumentException();
            }

            public void modify(Object element, String property, Object value) {
                if (element instanceof Item)
                    element = ((Item) element).getData(); // workaround, see super's comment
                SectionKey sectionKey = (SectionKey) element;
                if (property.equals("value"))
                    setValueInFile(sectionKey.section, sectionKey.key, (String) value);
                else if (property.equals("object"))
                    renameKeyInInifile(sectionKey.section, sectionKey.key, value+"."+entry.getName());
                else
                    throw new IllegalArgumentException();
                tableViewer.refresh();
            }
        });

        // content assist for the Value column
        IContentProposalProvider valueProposalProvider = new InifileValueContentProposalProvider(null, null, inifile, null, false) {
            @Override
            public IContentProposal[] getProposals(String contents, int position) {
                // we need to reconfigure the proposal provider on the fly to know about the current section
                SectionKey sectionKey = (SectionKey)( (IStructuredSelection)tableViewer.getSelection()).getFirstElement();
                setInifileEntry(sectionKey.section, sectionKey.key); // set context for proposal calculation
                return super.getProposals(contents, position);
            }
        };

        ContentAssistUtil.configureTableColumnContentAssist(tableViewer, valueColumnIndex, valueProposalProvider, null, true);

        return tableViewer;
    }
}
