/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IMarker;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.common.ui.SWTFactory;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.ConfigOption.DataType;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;

/**
 * Combobox-based editor for inifile entries.
 *
 * NOTE: This class edits the [General] section ONLY. All other sections
 * are ignored. For example, the Reset button only removes the setting
 * from the [General] section. When a setting is present outside
 * [General], the table-based field editor has to be used.
 *
 * @author Andras
 */
//XXX disable Combo when value is not editable (comes from included file)?
//XXX set up content assist (like in TextFieldEditor)
public class ComboFieldEditor extends FieldEditor {
    private Combo combo;
    private ToolItem resetButton;
    private boolean isEdited;
    private ControlDecoration problemDecoration;
    private boolean enableTypein;

    public ComboFieldEditor(Composite parent, ConfigOption entry, IInifileDocument inifile, FormPage formPage, Map<String,Object> hints, boolean enableTypein) {
        super(parent, SWT.NONE, entry, inifile, formPage, hints);
        this.enableTypein = enableTypein;

        // layout
        GridLayout gridLayout = new GridLayout(3, false);
        gridLayout.marginHeight = 2;
        setLayout(gridLayout);

        // child widgets
        combo = createCombo();
        addTooltipSupport(combo);
        resetButton = createResetButton();
        SWTFactory.autoHide(resetButton.getParent(), this);

        problemDecoration = new ControlDecoration(combo, SWT.LEFT | SWT.TOP);
        problemDecoration.setShowOnlyOnFocus(false);

        // set layout data
        int width = (entry.getDataType()==DataType.CFG_INT || entry.getDataType()==DataType.CFG_DOUBLE) ? 80 : 250;
        combo.setLayoutData(new GridData(width, SWT.DEFAULT));
        ((GridData)combo.getLayoutData()).horizontalIndent = 3; // room for the decoration

        reread();

        // enable Reset button on editing
        combo.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                if (!isEdited) {
                    isEdited = true;
                    setGrayed(false);
                    resetButton.setEnabled(true);
                }
            }
        });
        // commit the editfield on Enter
        combo.addSelectionListener(new SelectionAdapter() {
            public void widgetDefaultSelected(SelectionEvent e) {
                commit();
            }
        });

        // commit on losing focus
        addFocusListenerTo(combo);

        // when the background gets clicked, transfer focus to the text widget
        addFocusTransfer(this, combo);

    }

    protected void setGrayed(boolean grayed) {
        UIUtils.setWidgetClassName(combo, grayed ? "grayed" : "");
    }

    protected Combo createCombo() {
        Combo combo = new Combo(this, SWT.SINGLE | SWT.BORDER | SWT.DROP_DOWN | (enableTypein ? SWT.NONE : SWT.READ_ONLY));
        return combo;
    }

    @Override
    public void reread() {
        // update text and reset button
        String key = entry.isPerObject() ? "**."+entry.getName() : entry.getName();
        String value = getValueFromFile(GENERAL, key);
        if (value==null) {
            String defaultValue = entry.getDefaultValue()==null ? "" : entry.getDefaultValue().toString();
            combo.setText(defaultValue);
            setGrayed(true);
            resetButton.setEnabled(false);
        }
        else {
            combo.setText(value);
            setGrayed(false);
            resetButton.setEnabled(true);
        }
        isEdited = false;

        refreshDecorations();
    }

    public void setComboContents(List<String> list) {
        String oldValue = combo.getText();
        combo.removeAll();
        if (list != null)
            for (String i : list)
                combo.add(i);
        combo.setText(oldValue);
        reread();
    }

    @Override
    public void refreshDecorations() {
        String key = entry.isPerObject() ? "**."+entry.getName() : entry.getName();
        IMarker[] markers = InifileUtils.getProblemMarkersFor(GENERAL, key, inifile);
        problemDecoration.setImage(getProblemImage(markers, true, true));
        problemDecoration.setDescriptionText(getProblemsText(markers));
        redraw(); // otherwise an obsolete error decoration doesn't disappear
    }

    @Override
    public void commit() {
        if (isEdited) {
            String key = entry.isPerObject() ? "**."+entry.getName() : entry.getName();
            String value = combo.getText();
            setValueInFile(GENERAL, key, value);
            isEdited = false;
        }
    }
}
