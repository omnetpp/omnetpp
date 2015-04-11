/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.Map;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;

/**
 * Checkbox-based editor for boolean inifile entries. This editor commits changes
 * immediately, not only on losing the focus.
 *
 * NOTE: This class edits the [General] section ONLY. All other sections
 * are ignored. For example, the Reset button only removes the setting
 * from the [General] section. When the setting is present outside
 * [General], the table-based field editor has to be used.
 *
 * @author Andras
 */
//XXX disable checkbox when value is not editable (comes from included file)?
public class CheckboxFieldEditor extends FieldEditor {
    private Button checkbox;
    private Label label;
    private ControlDecoration problemDecoration;
    private ToolItem resetButton;

    public CheckboxFieldEditor(Composite parent, ConfigOption entry, IInifileDocument inifile, FormPage formPage, String labelText, Map<String,Object> hints) {
        super(parent, SWT.NONE, entry, inifile, formPage, hints);

        Assert.isTrue(entry.getDataType()==ConfigOption.DataType.CFG_BOOL);

        GridLayout gridLayout = new GridLayout(4, false);
        gridLayout.marginHeight = 0;
        setLayout(gridLayout);

        checkbox = new Button(this, SWT.CHECK);
        checkbox.setBackground(BGCOLOR);
        addTooltipSupport(checkbox);
        problemDecoration = new ControlDecoration(checkbox, SWT.RIGHT | SWT.TOP);
        problemDecoration.setShowOnlyOnFocus(false);
        label = createLabel(entry, labelText);
        resetButton = createResetButton();

        checkbox.setLayoutData(new GridData());
        label.setLayoutData(new GridData());
        ((GridData)label.getLayoutData()).horizontalIndent = 5; // room for the problem decoration

        reread();

        // enable Reset button on editing
        checkbox.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                commit();
                resetButton.setEnabled(true);
            }
        });

        addFocusTransfer(label, checkbox);
        addFocusTransfer(this, checkbox);
    }

    @Override
    public void reread() {
        String key = entry.isPerObject() ? "**."+entry.getName() : entry.getName();
        String value = getValueFromFile(GENERAL, key);
        if (value==null) {
            boolean defaultValue = entry.getDefaultValue()==null ? false : entry.getDefaultValue().equals("true");
            checkbox.setSelection(defaultValue);
            resetButton.setEnabled(false);
        }
        else {
            checkbox.setSelection(InifileUtils.parseAsBool(value));
            resetButton.setEnabled(true);
        }

        // update problem decoration
        IMarker[] markers = InifileUtils.getProblemMarkersFor(GENERAL, entry.getName(), inifile);
        problemDecoration.setImage(getProblemImage(markers, true, true));
        problemDecoration.setDescriptionText(getProblemsText(markers));
        redraw();
    }

    @Override
    public void commit() {
        String key = entry.isPerObject() ? "**."+entry.getName() : entry.getName();
        boolean value = checkbox.getSelection();
        setValueInFile(GENERAL, key, value ? "true" : "false");
    }
}
