/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import java.util.ArrayList;
import java.util.Map;

import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;
import org.omnetpp.inifile.editor.model.SectionKey;

/**
 * Base class for field editors that allow the user edit a setting
 * with section granularity.
 *
 * @author Andras
 */
public abstract class TableFieldEditor extends FieldEditor {
    public static final String HINT_SECTION_COL_TITLE = "section.column.title";
    public static final String HINT_OBJECT_COL_TITLE = "object.column.title";
    public static final String HINT_VALUE_COL_TITLE = "value.column.title";

    protected TableViewer tableViewer;  // input: section names as String[]
    protected Button addButton;
    protected Button removeButton;

    public TableFieldEditor(Composite parent, ConfigOption entry, IInifileDocument inifile, FormPage formPage, String labelText, Map<String,Object> hints) {
        super(parent, SWT.NONE, entry, inifile, formPage, hints);

        GridLayout gridLayout = new GridLayout(1, false);
        gridLayout.marginTop = gridLayout.marginBottom = gridLayout.marginHeight = gridLayout.verticalSpacing = 0;
        gridLayout.marginHeight = 2;
        setLayout(gridLayout);

        Label label = createLabel(entry, labelText+":  (default: "+(entry.getDefaultValue()==null ? "none" : entry.getDefaultValue().toString())+")");
        createTableWithButtons(this);
        addFocusTransfer(label, tableViewer.getTable());
        addFocusTransfer(this, tableViewer.getTable());

        reread();
    }

    protected void createTableWithButtons(Composite parent) {
        Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
        composite.setLayout(new GridLayout(2, false));

        tableViewer = createTableViewer(composite);
        tableViewer.setContentProvider(new ArrayContentProvider());

        // export the table's selection as editor selection (here we assume that table elements are section name strings)
        tableViewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                IStructuredSelection sel = (IStructuredSelection) event.getSelection();
                SectionKey sectionKey = (SectionKey) sel.getFirstElement();
                if (sectionKey!=null)
                    formPage.setEditorSelection(sectionKey.section, sectionKey.key);
            }
        });

        // add button group
        Composite buttonGroup = new Composite(composite, SWT.NONE);
        buttonGroup.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, true));

        GridLayout gridLayout = new GridLayout(1, false);
        gridLayout.marginTop = gridLayout.marginBottom = gridLayout.marginHeight = 0;
        buttonGroup.setLayout(gridLayout);
        addButton = createButton(buttonGroup, "Add...");
        removeButton = createButton(buttonGroup, "Remove");

        addButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                addNewEntry();
            }
        });

        removeButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                removeSelected();
            }
        });

        // set up hotkey support
        tableViewer.getTable().addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.character == SWT.DEL)
                    removeSelected();
            }
        });

        // add hover support
        formPage.addTooltipSupport(tableViewer.getTable(), new IHoverInfoProvider() {
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                Item item = tableViewer.getTable().getItem(new Point(x,y));
                SectionKey sectionKey = (SectionKey) (item==null ? null : item.getData());
                if (sectionKey != null) {
                    if (entry.isPerObject())
                        return new HtmlHoverInfo(InifileHoverUtils.getPerObjectConfigHoverText(sectionKey.section, sectionKey.key, inifile));
                    else
                        return new HtmlHoverInfo(InifileHoverUtils.getConfigHoverText(sectionKey.section, sectionKey.key, inifile));
                }
                else
                    return null;
            }
        });

    }

    protected void removeSelected() {
        IStructuredSelection selection = (IStructuredSelection) tableViewer.getSelection();
        for (Object o : selection.toArray()) {
            SectionKey sectionKey = (SectionKey) o;
            removeFromFile(sectionKey.section, sectionKey.key);
        }
        reread();
    }

    protected void addNewEntry() {
        String section = askTargetSection();
        if (section != null) {
            String key = entry.isPerObject() ? "**."+entry.getName() : entry.getName();
            setValueInFile(section, key, getDefaultValueFor(section));
            reread();
        }
    }

    /**
     * When creating a new entry: prompts the user for the section in which to insert it.
     */
    protected String askTargetSection() {
        // collect section which not yet contain this key
        ArrayList<String> list = new ArrayList<String>();
        for (String section : inifile.getSectionNames())
            if (entry.isPerObject() || !inifile.containsKey(section, entry.getName()))
                list.add(section);

        // and pop up a chooser dialog
        ListDialog dialog = new ListDialog(getShell());
        dialog.setLabelProvider(new LabelProvider());
        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setInput(list.toArray());
        dialog.setMessage("Select target section:");
        dialog.setTitle("New Configuration Key");
        if (dialog.open() == ListDialog.OK && dialog.getResult().length > 0)
            return dialog.getResult()[0].toString();
        return null;
    }

    /**
     * Generate default value for a new entry (in the given section).
     */
    protected String getDefaultValueFor(String section) {  //XXX needed?
        if (entry.getDefaultValue() != null)
            return entry.getDefaultValue().toString();
        switch (entry.getDataType()) {
            case CFG_BOOL: return "true";
            case CFG_INT:  return "0";
            case CFG_DOUBLE: return "0.0";
            case CFG_FILENAME: return "";
            case CFG_FILENAMES: return "";
            case CFG_PATH: return "";
            case CFG_STRING: return "";
        }
        return "";
    }

    /**
     * Create the table and add label provider. Content provider and input will be
     * managed by the TableFieldEditor base class.
     */
    abstract protected TableViewer createTableViewer(Composite composite);

    /**
     * Utility method for subclasses, to add a table column. (Unused here.)
     */
    protected TableColumn addTableColumn(Table table, String label, int width) {
        TableColumn column = new TableColumn(table, SWT.NONE);
        column.setText(label);
        column.pack();
        if (column.getWidth() < width)
            column.setWidth(width);
        return column;
    }

    protected Button createButton(Composite buttonGroup, String label) {
        Button button = new Button(buttonGroup, SWT.PUSH);
        button.setText(label);
        button.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
        return button;
    }

    /**
     * Does nothing.
     */
    @Override
    public void commit() {
        // table cell editors will auto-commit when losing the focus
    }

    /**
     * Refresh table contents.
     */
    @Override
    public void reread() {
        ArrayList<SectionKey> list = new ArrayList<SectionKey>();
        if (!entry.isPerObject()) {
            // find out in which sections this key occurs
            for (String section : inifile.getSectionNames())
                if (inifile.containsKey(section, entry.getName()))
                    list.add(new SectionKey(section, entry.getName()));
        }
        else {
            // find matching entries from all sections
            String keySuffix = "."+entry.getName();
            for (String section : inifile.getSectionNames())
                for (String key : inifile.getKeys(section))
                    if (key.endsWith(keySuffix))
                        list.add(new SectionKey(section, key));
        }

        // give it to the table as input
        tableViewer.setInput(list.toArray(new SectionKey[list.size()]));
    }

}
