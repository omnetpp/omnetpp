/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.Map;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.IInifileChangeListener;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * An inifile field editor, which displays either a TextFieldEditor or a TableFieldEditor,
 * and lets the user toggle between them.
 *
 * @author Andras
 */
//XXX when focus is in the table, form cannot be scrolled. why??
public abstract class ExpandableFieldEditor extends FieldEditor  {
    private static Image IMAGE_EXPAND = InifileEditorPlugin.getCachedImage("icons/full/elcl16/expand.png");
    private static Image IMAGE_COLLAPSE = InifileEditorPlugin.getCachedImage("icons/full/elcl16/collapse.png");

    protected String labelText;
    protected FieldEditor fieldEditor;
    protected boolean isExpanded;
    protected ToolItem toggleButton;

    private IInifileChangeListener inifileChangeListener = new IInifileChangeListener() {
        public void modelChanged() {
            Display.getDefault().asyncExec(new Runnable() {
                public void run() {
                    updateToggleButton();
                }
            });
        }
    };

    public ExpandableFieldEditor(Composite parent, ConfigOption entry, IInifileDocument inifile, FormPage formPage, String labelText, Map<String,Object> hints) {
        super(parent, SWT.NONE, entry, inifile, formPage, hints);
        this.labelText = labelText;
        GridLayout gridLayout = new GridLayout(2, false);
        gridLayout.marginHeight = gridLayout.marginWidth = 0;
        setLayout(gridLayout);

        isExpanded = (!entry.isGlobal() && !canBeCollapsed());
        createControl();

        // need to update the toggle button's state on inifile changes
        inifile.addInifileChangeListener(inifileChangeListener);
        addDisposeListener(new DisposeListener() {
            public void widgetDisposed(DisposeEvent e) {
                ExpandableFieldEditor.this.inifile.removeInifileChangeListener(inifileChangeListener);
            }
        });
    }

    protected void recreateControl() {
        for (Control c : getChildren())
            c.dispose();
        createControl();
        formPage.layoutForm();
    }

    protected void createControl() {
        fieldEditor = createFieldEditor(isExpanded);
        fieldEditor.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));

        toggleButton = FieldEditor.createFlatImageButton(this);
        toggleButton.setImage(isExpanded ? IMAGE_COLLAPSE : IMAGE_EXPAND);
        toggleButton.setToolTipText(isExpanded ? "Specify value globally, in the [General] section": "Specify values individually for different sections");
        toggleButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                isExpanded = !isExpanded;
                recreateControl();
            }
        });
        updateToggleButton();
    }

    abstract protected FieldEditor createFieldEditor(boolean isExpanded);

    protected boolean canBeCollapsed() {
        if (!entry.isPerObject()) {
            for (String section : inifile.getSectionNames())
                if (inifile.containsKey(section, entry.getName()) && !section.equals(GENERAL))
                    return false;
        }
        else {
            // if it contains any occurrence of this key except **.<key> in [General], it cannot be collapsed
            for (String section : inifile.getSectionNames())
                for (String key : inifile.getKeys(section))
                    if (key.endsWith("."+entry.getName()) && (!section.equals(GENERAL) || !key.equals("**."+entry.getName())))
                        return false;
        }
        return true;
    }

    @Override
    public void commit() {
        fieldEditor.commit();
    }

    @Override
    public void reread() {
        fieldEditor.reread();
    }

    protected void updateToggleButton() {
        // need to update the toggle button's state when the last per-section value is removed
        boolean shouldBeEnabled = !isExpanded || canBeCollapsed();
        if (toggleButton.getEnabled() != shouldBeEnabled)
            toggleButton.setEnabled(shouldBeEnabled);
    }

    public boolean isExpanded() {
        return isExpanded;
    }

    public FieldEditor getInnerFieldEditor() {
        return fieldEditor;
    }
}
